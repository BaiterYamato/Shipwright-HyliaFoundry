#include "ShipLuaBootstrap.h"
#include "OotActorProvider.h"
#include "OotHotkeyRegistry.h"
#include "OotWorldAdapter.h"
#include "ShipLuaPuppet.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"

#include <filesystem>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <memory>
#include <optional>
#include <string>
#include <vector>

#ifdef _WIN32
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#endif

#include <spdlog/spdlog.h>

#include <cstring>

#include <ship/Context.h>
#include <ship/resource/File.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/archive/ArchiveManager.h>
#include <ship/resource/archive/O2rArchive.h>
#include <shiplua/generated/ApiBindings.h>
#include <shiplua/host/ModHost.h>
#include <shiplua/runtime/LuaRuntime.h>
#include <shiplua/storage/AtomicFile.h>
#include <shiplua/world/WorldHandoff.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/ShipInit.hpp"
// OPEN_DISPS declara FrameInterpolation_* em escopo de bloco com linkage C++;
// este header traz as declarações extern "C" corretas.
#include "soh/frame_interpolation.h"
#include "soh/cvar_prefixes.h"
#include <libultraship/bridge.h>

extern "C" {
#include "functions.h"
#include "lauxlib.h"
#include "lua.h"
#include "macros.h"
#include "variables.h"
#include "z64.h"
extern PlayState* gPlayState;
extern u8 gWalkSpeedToggle;

// Emite uma display list arbitrária no buffer opaco. Precisa de linkage C: o
// macro OPEN_DISPS declara FrameInterpolation_* em escopo de bloco, e dentro
// de código C++ isso vira um símbolo mangled que não existe.
// Declarada em z_player.c; inicia um novo rolamento (usada para encadear).
// O parâmetro se chama "this" no C original — aqui precisa de outro nome.
void Player_SetupRoll(Player* player, PlayState* play);

static void ShipLuaEmitDisplayList(PlayState* play, const char* path) {
    OPEN_DISPS(play->state.gfxCtx);
    gSPDisplayList(POLY_OPA_DISP++, (Gfx*)path);
    CLOSE_DISPS(play->state.gfxCtx);
}
}

namespace ShipLuaHost {
namespace {

std::unique_ptr<ShipLua::ModHost> gModHost;
std::shared_ptr<OotActorProvider> gActorProvider;
std::shared_ptr<ShipLua::CapabilityRegistry> gCapabilityRegistry;
std::shared_ptr<OotHotkeyRegistry> gHotkeys;
std::shared_ptr<OotWorldAdapter> gWorldAdapter;
HOOK_ID gLoadGameHook = 0;
HOOK_ID gImportTickHook = 0;
std::optional<ShipLua::WorldHandoff> gPendingHandoff;
HOOK_ID gActorDestroyHook = 0;
HOOK_ID gPlayDestroyHook = 0;

constexpr int kSwitchWorldExitCode = 73;

struct BridgeConfig {
    std::filesystem::path sessionDirectory;
    std::filesystem::path handoffPath;
    std::array<std::byte, 16> sessionId{};
    std::array<std::byte, 32> authenticationKey{};
    std::uint64_t sequence = 0;
};

int HexDigit(char value) {
    if (value >= '0' && value <= '9') {
        return value - '0';
    }
    if (value >= 'a' && value <= 'f') {
        return value - 'a' + 10;
    }
    if (value >= 'A' && value <= 'F') {
        return value - 'A' + 10;
    }
    return -1;
}

template <std::size_t Size> bool ParseHex(const char* text, std::array<std::byte, Size>& output) {
    if (text == nullptr || std::char_traits<char>::length(text) != Size * 2) {
        return false;
    }
    for (std::size_t index = 0; index < Size; ++index) {
        const int high = HexDigit(text[index * 2]);
        const int low = HexDigit(text[index * 2 + 1]);
        if (high < 0 || low < 0) {
            return false;
        }
        output[index] = static_cast<std::byte>((high << 4) | low);
    }
    return true;
}

std::optional<BridgeConfig> GetBridgeConfig() {
    const char* sessionDirectory = std::getenv("LINKSPAN_SESSION_DIR");
    const char* handoffPath = std::getenv("LINKSPAN_HANDOFF_PATH");
    const char* sequence = std::getenv("LINKSPAN_SEQUENCE");
    if (sessionDirectory == nullptr || handoffPath == nullptr || sequence == nullptr) {
        return std::nullopt;
    }
    BridgeConfig config;
    config.sessionDirectory = sessionDirectory;
    config.handoffPath = handoffPath;
    if (!ParseHex(std::getenv("LINKSPAN_SESSION_ID"), config.sessionId) ||
        !ParseHex(std::getenv("LINKSPAN_AUTH_KEY"), config.authenticationKey)) {
        return std::nullopt;
    }
    char* end = nullptr;
    config.sequence = std::strtoull(sequence, &end, 10);
    if (end == sequence || *end != '\0' || config.sequence == 0) {
        return std::nullopt;
    }
    return config;
}

bool BothGamesAvailable() {
    const char* available = std::getenv("LINKSPAN_AVAILABLE_GAMES");
    if (available == nullptr) {
        return false;
    }
    const std::string games(available);
    return games.find("oot") != std::string::npos && games.find("mm") != std::string::npos;
}

[[noreturn]] void ExitForWorldSwitch() {
#ifdef _WIN32
    // RequestWorldTravel runs inside a Lua callback. std::exit executes CRT
    // teardown while that callback stack is still active and can trigger the
    // Windows fail-fast code 0xC0000409 before the launcher receives code 73.
    ::ExitProcess(static_cast<UINT>(kSwitchWorldExitCode));
#else
    std::_Exit(kSwitchWorldExitCode);
#endif
}

ShipLua::Result<void> RequestWorldTravel(const ShipLua::WorldDestination& destination) {
    const auto config = GetBridgeConfig();
    if (!config.has_value() || !BothGamesAvailable() || destination.world != ShipLua::WorldId::Mm ||
        gWorldAdapter == nullptr) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::Unsupported, "ponte Link-Span para MM indisponível");
    }
    if (gPlayState == nullptr || gPlayState->sceneNum != SCENE_LINKS_HOUSE) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::InvalidState,
                                          "o teleporte deve ser usado dentro da casa do Link");
    }
    const auto player = gWorldAdapter->CapturePlayerState();
    if (!player.isOk()) {
        return ShipLua::Result<void>::err(player.code, player.message);
    }
    ShipLua::WorldHandoff handoff;
    handoff.sessionId = config->sessionId;
    handoff.sequence = config->sequence;
    handoff.source = ShipLua::WorldId::Oot;
    handoff.destination = destination;
    handoff.player = *player.value;
    const auto written = ShipLua::WorldHandoffCodec::WriteFile(config->handoffPath, handoff, config->authenticationKey);
    if (!written.isOk()) {
        return written;
    }
    const auto requested = ShipLua::AtomicFile::Write(config->sessionDirectory / "next-world", "mm\n");
    if (!requested.isOk()) {
        std::error_code ignored;
        std::filesystem::remove(config->handoffPath, ignored);
        return requested;
    }
    SPDLOG_INFO("Link-Span exportou o estado OoT e solicitou troca para MM ({})", destination.id);
    spdlog::apply_all([](const std::shared_ptr<spdlog::logger>& logger) { logger->flush(); });
    ExitForWorldSwitch();
}

void TryConsumeWorldHandoff() {
    const auto config = GetBridgeConfig();
    if (!config.has_value() || gWorldAdapter == nullptr || !std::filesystem::is_regular_file(config->handoffPath)) {
        return;
    }
    const auto handoff = ShipLua::WorldHandoffCodec::ReadFile(config->handoffPath, config->authenticationKey);
    if (!handoff.isOk()) {
        SPDLOG_ERROR("Link-Span rejeitou o handoff OoT: {}", handoff.message);
        return;
    }
    if (handoff.value->sessionId != config->sessionId || handoff.value->sequence + 1 != config->sequence ||
        handoff.value->destination.world != ShipLua::WorldId::Oot) {
        SPDLOG_ERROR("Link-Span rejeitou um handoff destinado a outra sessão ou jogo");
        return;
    }
    // Nada é aplicado aqui: o hook de load também dispara fora de um jogo
    // carregado (intro/arquivo novo), e escrever o save + forçar entrance
    // nesse ponto corrompe o estado. Guarda o handoff — e o arquivo em disco —
    // até um frame realmente jogável.
    gPendingHandoff = *handoff.value;
    SPDLOG_INFO("Link-Span recebeu um handoff para OoT ({}) — aguardando um save carregado",
                handoff.value->destination.id);
}

// Só importa com um jogo de verdade rodando: PlayState ativo, save com vida
// (arquivo carregado, não a intro) e nenhuma transição em curso.
bool WorldImportIsSafe() {
    if (gPlayState == nullptr) {
        return false;
    }
    if (gSaveContext.healthCapacity <= 0) {
        return false;
    }
    return gPlayState->transitionTrigger == TRANS_TRIGGER_OFF;
}

void TickWorldImport() {
    if (!gPendingHandoff.has_value() || gWorldAdapter == nullptr || !WorldImportIsSafe()) {
        return;
    }
    const auto config = GetBridgeConfig();
    if (!config.has_value()) {
        gPendingHandoff.reset();
        return;
    }

    const ShipLua::WorldHandoff handoff = *gPendingHandoff;
    const auto prepared = gWorldAdapter->PrepareImport(handoff.player, handoff.destination);
    if (!prepared.isOk()) {
        gPendingHandoff.reset();
        SPDLOG_ERROR("Link-Span não preparou a importação OoT: {}", prepared.message);
        return;
    }
    const auto committed = gWorldAdapter->CommitImport();
    if (!committed.isOk()) {
        gWorldAdapter->AbortImport();
        gPendingHandoff.reset();
        SPDLOG_ERROR("Link-Span não confirmou a importação OoT: {}", committed.message);
        return;
    }
    // Só agora o handoff pode sumir do disco: a viagem foi mesmo aplicada.
    gPendingHandoff.reset();
    std::error_code error;
    std::filesystem::remove(config->handoffPath, error);
    SPDLOG_INFO("Link-Span importou o estado compartilhado em OoT ({})", handoff.destination.id);
}

#ifdef _WIN32
std::filesystem::path RuntimeRoot() {
    if (const wchar_t* configured = _wgetenv(L"LINKSPAN_ROOT"); configured != nullptr && *configured != L'\0') {
        return configured;
    }
    std::wstring executable(32768, L'\0');
    const DWORD length = GetModuleFileNameW(nullptr, executable.data(), static_cast<DWORD>(executable.size()));
    if (length == 0 || length >= executable.size()) {
        return std::filesystem::current_path();
    }
    executable.resize(length);
    return std::filesystem::path(executable).parent_path();
}

std::wstring QuotePowerShellLiteral(std::wstring value) {
    std::size_t position = 0;
    while ((position = value.find(L'\'', position)) != std::wstring::npos) {
        value.insert(position, 1, L'\'');
        position += 2;
    }
    return L"'" + value + L"'";
}
#endif

ShipLua::Logger CreateLogger() {
    return ShipLua::Logger([](ShipLua::LogLevel level, const std::string& modId, const std::string& message) {
        switch (level) {
            case ShipLua::LogLevel::Debug:
                SPDLOG_DEBUG("ShipLua [{}]: {}", modId, message);
                break;
            case ShipLua::LogLevel::Info:
                SPDLOG_INFO("ShipLua [{}]: {}", modId, message);
                break;
            case ShipLua::LogLevel::Warn:
                SPDLOG_WARN("ShipLua [{}]: {}", modId, message);
                break;
            case ShipLua::LogLevel::Error:
                SPDLOG_ERROR("ShipLua [{}]: {}", modId, message);
                break;
        }
    });
}

std::string GetHostVersion() {
    return std::to_string(gBuildVersionMajor) + "." + std::to_string(gBuildVersionMinor) + "." +
           std::to_string(gBuildVersionPatch);
}

std::int16_t DegreesToBinang(double degrees) {
    const double normalized = std::remainder(degrees, 360.0);
    return static_cast<std::int16_t>(std::lround(normalized * (65536.0 / 360.0)));
}

std::shared_ptr<OotActorProvider> CreateActorProvider() {
    OotActorProviderHooks hooks;
    hooks.objectReady = [](std::int16_t objectId) {
        if (gPlayState == nullptr) {
            return false;
        }
        const s32 objectIndex = Object_GetIndex(&gPlayState->objectCtx, objectId);
        return objectIndex >= 0 && Object_IsLoaded(&gPlayState->objectCtx, objectIndex);
    };
    hooks.spawn = [](const OotActorDefinition& definition, const ShipLua::ActorSpawnRequest& request) -> void* {
        if (gPlayState == nullptr) {
            return nullptr;
        }
        float x = static_cast<float>(request.x);
        float y = static_cast<float>(request.y);
        float z = static_cast<float>(request.z);
        std::int16_t rotationY = DegreesToBinang(request.rotationY);
        // Posição (0,0,0) significa "na frente do player": os mods ainda não
        // têm ship.transform.relative_to_player (plan-sdk §8.4).
        if (request.x == 0 && request.y == 0 && request.z == 0) {
            if (Player* player = GET_PLAYER(gPlayState); player != nullptr) {
                const float forward = 60.0f;
                x = player->actor.world.pos.x + Math_SinS(player->actor.shape.rot.y) * forward;
                y = player->actor.world.pos.y;
                z = player->actor.world.pos.z + Math_CosS(player->actor.shape.rot.y) * forward;
                rotationY = static_cast<std::int16_t>(player->actor.shape.rot.y + 0x8000);
            }
        }
        Actor* spawned = Actor_Spawn(&gPlayState->actorCtx, gPlayState, definition.actorId, x, y, z,
                                     DegreesToBinang(request.rotationX), rotationY,
                                     DegreesToBinang(request.rotationZ), definition.params);
        if (spawned != nullptr && definition.key == "oot.link_child_puppet") {
            if (!ShipLuaPuppet_Attach(spawned, gPlayState)) {
                Actor_Kill(spawned);
                return nullptr;
            }
        }
        if (spawned != nullptr && definition.key == "oot.mm_elegy_statue") {
            if (!ShipLuaPuppet_AttachStatue(spawned)) {
                Actor_Kill(spawned);
                return nullptr;
            }
        }
        return spawned;
    };
    hooks.kill = [](void* actor) {
        if (actor != nullptr) {
            Actor_Kill(static_cast<Actor*>(actor));
        }
    };
    std::vector<OotActorDefinition> allowlist{
        { "oot.en_dog", ACTOR_EN_DOG, OBJECT_DOG, static_cast<std::int16_t>(0x8000) },
        { "oot.en_torch2", ACTOR_EN_TORCH2, OBJECT_TORCH2, 0 },
        // Host: En_Item00 (gameplay_keep, sempre carregado); update/draw são
        // substituídos por ShipLuaPuppet_Attach logo após o spawn.
        { "oot.link_child_puppet", ACTOR_EN_ITEM00, OBJECT_GAMEPLAY_KEEP, 0 },
        // Estátua da Elegia do MM: assets transplantados via mm-elegy-assets.o2r.
        { "oot.mm_elegy_statue", ACTOR_EN_ITEM00, OBJECT_GAMEPLAY_KEEP, 0 },
    };
    return std::make_shared<OotActorProvider>(std::move(allowlist), std::move(hooks), CreateLogger(), ACTOR_PLAYER);
}

ShipLua::Result<void> RegisterHostCapability(const std::string& id, const std::string& description) {
    if (gCapabilityRegistry == nullptr) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::InvalidState, "capability registry is unavailable");
    }
    const auto providerVersion = ShipLua::SemVersion::Parse(GetHostVersion());
    const auto capabilityVersion = ShipLua::SemVersion::Parse(std::string(ShipLua::Generated::kApiVersion));
    if (!providerVersion.isOk() || !capabilityVersion.isOk()) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::HostFailure, "invalid Shipwright or ShipLua version");
    }
    ShipLua::CapabilityProvider offer;
    offer.name = "shipwright-native";
    offer.providerVersion = *providerVersion.value;
    offer.capabilityVersion = *capabilityVersion.value;
    offer.games = { "oot" };
    offer.stability = ShipLua::CapabilityStability::Experimental;
    offer.description = description;
    return gCapabilityRegistry->Register(id, std::move(offer));
}

ShipLua::Result<ShipLua::LuaApiHostContext> CreateHostContext() {
    ShipLua::LuaApiHostContext context;
    context.gameId = "oot";
    context.hostVersion = GetHostVersion();
    context.capabilities = { "oot.player.jump",         "oot.spawn_dog",      "oot.player.bunny_hood",
                             "oot.player.mask",         "player.speed",       "player.fields",
                             "oot.player.attach_model", "mod.assets",         "oot.player.immunity",
                             "oot.player.weight",       "oot.player.roll",    "hooks.bridge" };
    context.hotkeys = gHotkeys;
    context.capabilityRegistry = gCapabilityRegistry;
    context.actors = gActorProvider;
    auto registered = RegisterHostCapability("oot.player.jump", "Apply a validated jump impulse to OoT Link.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.spawn_dog", "Spawn the legacy OoT dog demo actor.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.bunny_hood",
                                        "Equip the OoT Bunny Hood with Majora's Mask speed and jump behaviour.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.mask", "Equip any OoT mask by logical name.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("player.speed", "Scale the player's movement speed by a validated factor.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("player.fields", "Read and write named player fields with range validation.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.attach_model",
                                        "Draw an arbitrary display list on the player by resource path.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered =
        RegisterHostCapability("mod.assets", "Mount mod-provided archives under the mod/<name>/ namespace.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.immunity", "Grant the player immunity to a damage kind.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.weight", "Switch the player between normal and heavy weight.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.player.roll", "Enable continuous, steerable chained rolling.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability(
        "hooks.bridge", "Generic bridge to native VB_*/On* GameInteractor hooks via ship.events.on(\"hook.*\").");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    if (const char* available = std::getenv("LINKSPAN_AVAILABLE_GAMES"); available != nullptr) {
        const std::string games(available);
        if (games.find("oot") != std::string::npos) {
            context.availableGames.push_back("oot");
        }
        if (games.find("mm") != std::string::npos) {
            context.availableGames.push_back("mm");
        }
    }
    if (BothGamesAvailable() && GetBridgeConfig().has_value()) {
        context.capabilities.push_back("world.travel");
        context.worldTravel = RequestWorldTravel;
        registered =
            RegisterHostCapability("world.travel", "Travel to a logical destination in the other Link-Span host.");
        if (!registered.isOk()) {
            return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
        }
    }
    return ShipLua::Result<ShipLua::LuaApiHostContext>::ok(std::move(context));
}

int LuaPlayerJump(lua_State* state) {
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || (player->stateFlags1 & PLAYER_STATE1_DEAD) != 0 || (player->actor.bgCheckFlags & 1) == 0) {
        lua_pushboolean(state, 0);
        return 1;
    }
    player->actor.velocity.y = 6.34375f;
    lua_pushboolean(state, 1);
    return 1;
}

// ship.player.set_speed_multiplier(factor): primitiva comum aos dois jogos.
// Dirige o SpeedModifier que o host já implementa, mas em modo incondicional
// (o padrão exige segurar um botão). Passar 1.0 restaura o estado anterior.
bool gSpeedForced = false;
float gSpeedPreviousValue = 1.0f;
int gSpeedPreviousToggleMode = 0;
u8 gSpeedPreviousToggleState = 0;

int LuaSetSpeedMultiplier(lua_State* state) {
    const double requested = luaL_checknumber(state, 1);
    if (!std::isfinite(requested) || requested < 0.1 || requested > 5.0) {
        SPDLOG_WARN("ShipLua set_speed_multiplier: fator fora da faixa 0.1–5.0");
        lua_pushboolean(state, 0);
        return 1;
    }

    const bool restore = std::fabs(requested - 1.0) < 0.0001;
    if (!gSpeedForced && !restore) {
        gSpeedPreviousValue = CVarGetFloat(CVAR_CHEAT("SpeedModifier.Value"), 1.0f);
        gSpeedPreviousToggleMode = CVarGetInteger(CVAR_CHEAT("SpeedModifier.SpeedToggle"), 0);
        gSpeedPreviousToggleState = gWalkSpeedToggle;
        gSpeedForced = true;
    }

    if (restore) {
        if (gSpeedForced) {
            CVarSetFloat(CVAR_CHEAT("SpeedModifier.Value"), gSpeedPreviousValue);
            CVarSetInteger(CVAR_CHEAT("SpeedModifier.SpeedToggle"), gSpeedPreviousToggleMode);
            gWalkSpeedToggle = gSpeedPreviousToggleState;
            gSpeedForced = false;
        }
        SPDLOG_INFO("ShipLua set_speed_multiplier: velocidade restaurada");
    } else {
        CVarSetFloat(CVAR_CHEAT("SpeedModifier.Value"), static_cast<float>(requested));
        // Modo toggle ligado + toggle ativo = multiplicador sempre valendo,
        // sem depender de o jogador segurar o botão modificador.
        CVarSetInteger(CVAR_CHEAT("SpeedModifier.SpeedToggle"), 1);
        gWalkSpeedToggle = 1;
        SPDLOG_INFO("ShipLua set_speed_multiplier: velocidade x{:.2f}", requested);
    }
    // Os hooks COND_VB_SHOULD do SpeedModifiers avaliam a condição no momento
    // do REGISTRO — e só os widgets do menu chamam ShipInit::Init ao mudar o
    // CVar. Sem esta chamada, mudar o valor programaticamente não liga nada.
    ShipInit::Init(CVAR_CHEAT("SpeedModifier.Value"));

    lua_pushboolean(state, 1);
    return 1;
}

// ---------------------------------------------------------------------------
// Primitivas de habilidade. Genéricas de propósito: servem para a forma Goron,
// mas também para qualquer outra forma, item ou mod de dificuldade.
// ---------------------------------------------------------------------------

// ship.oot.player.set_damage_immunity(kind, enabled): hoje só "fire". Apaga o
// corpo em chamas a cada frame, o que anula o dano contínuo de queimadura.
bool gFireImmunity = false;

// ship.oot.player.set_weight(kind): "heavy" usa a mecânica das botas de ferro
// (afunda na água, resiste a vento/empurrão); "normal" devolve as botas
// anteriores. É a tradução honesta de "peso de Goron" para os sistemas do OoT.
bool gHeavyWeight = false;
int8_t gPreviousBoots = 0;

// ship.oot.player.set_roll_mode(mode): "chain" encadeia rolamentos
// indefinidamente e permite dirigir durante o rolamento — a aproximação do
// rolamento contínuo do Goron usando a instrumentação que o engine já tem.
bool gChainRoll = false;
constexpr s16 kRollSteerStep = 0x400;   // giro por frame durante o rolamento
constexpr s32 kRollStickDeadzone = 10;  // abaixo disso, o rolamento acaba

int LuaSetDamageImmunity(lua_State* state) {
    const char* kind = luaL_checkstring(state, 1);
    const bool enabled = lua_toboolean(state, 2) != 0;
    if (std::strcmp(kind, "fire") != 0) {
        SPDLOG_WARN("ShipLua set_damage_immunity: tipo '{}' n\xC3\xA3o suportado (use 'fire')", kind);
        lua_pushboolean(state, 0);
        return 1;
    }
    gFireImmunity = enabled;
    SPDLOG_INFO("ShipLua set_damage_immunity: fogo {}", enabled ? "imune" : "normal");
    lua_pushboolean(state, 1);
    return 1;
}

int LuaSetWeight(lua_State* state) {
    const char* kind = luaL_optstring(state, 1, "normal");
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr) {
        SPDLOG_WARN("ShipLua set_weight: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }

    if (std::strcmp(kind, "heavy") == 0) {
        if (!gHeavyWeight) {
            gPreviousBoots = player->currentBoots;
            gHeavyWeight = true;
        }
        player->currentBoots = PLAYER_BOOTS_IRON;
        Player_SetBootData(play, player);
        SPDLOG_INFO("ShipLua set_weight: peso pesado (mec\xC3\xA2nica das botas de ferro)");
    } else if (std::strcmp(kind, "normal") == 0) {
        if (gHeavyWeight) {
            player->currentBoots = gPreviousBoots;
            Player_SetBootData(play, player);
            gHeavyWeight = false;
        }
        SPDLOG_INFO("ShipLua set_weight: peso normal");
    } else {
        SPDLOG_WARN("ShipLua set_weight: valor '{}' desconhecido (use 'heavy' ou 'normal')", kind);
        lua_pushboolean(state, 0);
        return 1;
    }

    lua_pushboolean(state, 1);
    return 1;
}

int LuaSetRollMode(lua_State* state) {
    const char* mode = luaL_optstring(state, 1, "vanilla");
    if (std::strcmp(mode, "chain") == 0) {
        gChainRoll = true;
    } else if (std::strcmp(mode, "vanilla") == 0) {
        gChainRoll = false;
    } else {
        SPDLOG_WARN("ShipLua set_roll_mode: modo '{}' desconhecido (use 'chain' ou 'vanilla')", mode);
        lua_pushboolean(state, 0);
        return 1;
    }
    SPDLOG_INFO("ShipLua set_roll_mode: {}", mode);
    lua_pushboolean(state, 1);
    return 1;
}

// ship.oot.player.set_mask(name): veste qualquer máscara do OoT pelo nome
// lógico, mantendo-a equipada sem ocupar um botão C. "none" remove.
struct MaskEntry {
    const char* name;
    std::uint8_t mask;
};

constexpr std::array<MaskEntry, 8> kOotMasks = { {
    { "keaton", PLAYER_MASK_KEATON },
    { "skull", PLAYER_MASK_SKULL },
    { "spooky", PLAYER_MASK_SPOOKY },
    { "bunny_hood", PLAYER_MASK_BUNNY },
    { "goron", PLAYER_MASK_GORON },
    { "zora", PLAYER_MASK_ZORA },
    { "gerudo", PLAYER_MASK_GERUDO },
    { "truth", PLAYER_MASK_TRUTH },
} };

bool gMaskForced = false;
int gMaskPreviousPersistent = 0;

int LuaSetMask(lua_State* state) {
    const char* requested = luaL_optstring(state, 1, "none");
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || (player->stateFlags1 & PLAYER_STATE1_DEAD) != 0) {
        SPDLOG_WARN("ShipLua set_mask: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }

    if (std::strcmp(requested, "none") == 0) {
        player->currentMask = PLAYER_MASK_NONE;
        gSaveContext.ship.maskMemory = PLAYER_MASK_NONE;
        if (gMaskForced) {
            CVarSetInteger(CVAR_ENHANCEMENT("PersistentMasks"), gMaskPreviousPersistent);
            gMaskForced = false;
        }
        SPDLOG_INFO("ShipLua set_mask: máscara removida");
        lua_pushboolean(state, 1);
        return 1;
    }

    const auto found = std::find_if(kOotMasks.begin(), kOotMasks.end(), [requested](const MaskEntry& entry) {
        return std::strcmp(entry.name, requested) == 0;
    });
    if (found == kOotMasks.end()) {
        SPDLOG_WARN("ShipLua set_mask: máscara desconhecida '{}'", requested);
        lua_pushboolean(state, 0);
        return 1;
    }

    if (!gMaskForced) {
        gMaskPreviousPersistent = CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0);
        gMaskForced = true;
    }
    // Sem PersistentMasks o jogo zera currentMask no frame seguinte quando a
    // máscara não está num botão C.
    CVarSetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 1);
    player->currentMask = found->mask;
    gSaveContext.ship.maskMemory = found->mask;
    SPDLOG_INFO("ShipLua set_mask: máscara '{}' equipada", requested);
    lua_pushboolean(state, 1);
    return 1;
}

// PASSO 2 — ship.oot.player.attach_model(slot, path).
// Desenha uma display list ARBITRÁRIA no player, endereçada por caminho de
// resource — inclusive assets de mod ("mod/<id>/...") e do jogo vizinho
// ("mm/..."). É o que permite conteúdo novo (uma máscara que não existe em
// nenhum dos dois jogos) sem precisar de um slot no enum do engine.
//
// Hoje o slot suportado é "head": o host força uma máscara-veículo para que o
// bloco de desenho de máscara execute e substitui a DL pela do mod.
std::string gAttachedHeadModel;
HOOK_ID gMaskDrawHook = 0;
HOOK_ID gFireImmunityHook = 0;
HOOK_ID gRollChainHook = 0;
HOOK_ID gRollSteerHook = 0;

// ---------------------------------------------------------------------------
// Ponte de hooks — v1 curada. Os ~476 pontos VB_*/On* do GameInteractor já
// existiam; nenhum mod alcançava nenhum deles. Em vez de escrever uma função
// nativa por habilidade (o padrão de toda a noite), cada hook curado aqui só
// republica seus argumentos como ship.events.on("hook.oot....", fn) — a
// escolha de comportamento fica inteira no Lua. Ver ship.hooks.result() na
// lib compartilhada para o caminho de volta (gate/modify).
HOOK_ID gHookRunSpeedHook = 0;
HOOK_ID gHookFallDamageHook = 0;
HOOK_ID gHookItemReceiveHook = 0;
HOOK_ID gHookHealthChangeHook = 0;
HOOK_ID gHookBonkHook = 0;

// Dispara um evento "should"/"modify": devolve o EventValue que um callback
// Lua gravou via ship.hooks.result(), se algum gravou.
std::optional<ShipLua::EventValue> DispatchHookTransform(const char* name, ShipLua::EventPayload payload) {
    if (gModHost == nullptr) {
        return std::nullopt;
    }
    const auto outcome = gModHost->DispatchEvent(name, payload);
    if (!outcome.isOk()) {
        return std::nullopt;
    }
    const auto found = payload.find("__hook_result");
    if (found == payload.end()) {
        return std::nullopt;
    }
    return found->second;
}

// Dispara um evento puro de notificação — nenhum retorno é lido.
void DispatchHookEvent(const char* name, ShipLua::EventPayload payload) {
    if (gModHost == nullptr) {
        return;
    }
    gModHost->DispatchEvent(name, payload);
}

int LuaAttachModel(lua_State* state) {
    const char* slot = luaL_checkstring(state, 1);
    const char* path = luaL_optstring(state, 2, nullptr);
    if (std::strcmp(slot, "head") != 0) {
        SPDLOG_WARN("ShipLua attach_model: slot '{}' n\xC3\xA3o suportado (use 'head')", slot);
        lua_pushboolean(state, 0);
        return 1;
    }

    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr) {
        SPDLOG_WARN("ShipLua attach_model: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }

    if (path == nullptr || *path == '\0') {
        gAttachedHeadModel.clear();
        player->currentMask = PLAYER_MASK_NONE;
        gSaveContext.ship.maskMemory = PLAYER_MASK_NONE;
        SPDLOG_INFO("ShipLua attach_model: modelo removido do slot 'head'");
        lua_pushboolean(state, 1);
        return 1;
    }

    if (!ResourceMgr_FileExists(path)) {
        SPDLOG_WARN("ShipLua attach_model: asset '{}' n\xC3\xA3o encontrado", path);
        lua_pushboolean(state, 0);
        return 1;
    }

    // O interpretador gráfico só trata um ponteiro como caminho de recurso
    // quando ele começa com "__OTR__". Sem o prefixo, ele executa os bytes da
    // string como comandos de display list — e o jogo morre em opcodes ASCII.
    gAttachedHeadModel = std::string("__OTR__") + path;
    // Máscara-veículo: o engine só entra no bloco de desenho quando há uma
    // máscara equipada. A DL dela é substituída pela do mod no hook.
    CVarSetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 1);
    player->currentMask = PLAYER_MASK_KEATON;
    gSaveContext.ship.maskMemory = PLAYER_MASK_KEATON;
    SPDLOG_INFO("ShipLua attach_model: '{}' anexado ao slot 'head'", path);
    lua_pushboolean(state, 1);
    return 1;
}

// PASSO 3 — ship.player.get/set: acesso a campos do player por nome, com
// validação, em vez de uma função nativa dedicada por ideia.
enum class FieldKind { Health, HealthCapacity, Magic, Rupees, PosX, PosY, PosZ, RotY, Speed };

struct PlayerField {
    const char* name;
    FieldKind kind;
    double min;
    double max;
    bool writable;
};

constexpr std::array<PlayerField, 9> kPlayerFields = { {
    { "health", FieldKind::Health, 0, 20 * 16, true },
    { "health_capacity", FieldKind::HealthCapacity, 16, 20 * 16, true },
    { "magic", FieldKind::Magic, 0, 96, true },
    { "rupees", FieldKind::Rupees, 0, 999, true },
    { "pos_x", FieldKind::PosX, -100000, 100000, true },
    { "pos_y", FieldKind::PosY, -100000, 100000, true },
    { "pos_z", FieldKind::PosZ, -100000, 100000, true },
    { "rot_y", FieldKind::RotY, -32768, 32767, true },
    { "speed", FieldKind::Speed, -50, 50, true },
} };

const PlayerField* FindPlayerField(const char* name) {
    const auto found = std::find_if(kPlayerFields.begin(), kPlayerFields.end(),
                                    [name](const PlayerField& f) { return std::strcmp(f.name, name) == 0; });
    return found == kPlayerFields.end() ? nullptr : &*found;
}

double ReadPlayerField(const PlayerField& field, Player* player) {
    switch (field.kind) {
        case FieldKind::Health:
            return gSaveContext.health;
        case FieldKind::HealthCapacity:
            return gSaveContext.healthCapacity;
        case FieldKind::Magic:
            return gSaveContext.magic;
        case FieldKind::Rupees:
            return gSaveContext.rupees;
        case FieldKind::PosX:
            return player->actor.world.pos.x;
        case FieldKind::PosY:
            return player->actor.world.pos.y;
        case FieldKind::PosZ:
            return player->actor.world.pos.z;
        case FieldKind::RotY:
            return player->actor.shape.rot.y;
        case FieldKind::Speed:
            return player->linearVelocity;
    }
    return 0.0;
}

void WritePlayerField(const PlayerField& field, Player* player, double value) {
    switch (field.kind) {
        case FieldKind::Health:
            gSaveContext.health = static_cast<int16_t>(value);
            break;
        case FieldKind::HealthCapacity:
            gSaveContext.healthCapacity = static_cast<int16_t>(value);
            break;
        case FieldKind::Magic:
            gSaveContext.magic = static_cast<int8_t>(value);
            break;
        case FieldKind::Rupees:
            gSaveContext.rupees = static_cast<int16_t>(value);
            break;
        case FieldKind::PosX:
            player->actor.world.pos.x = static_cast<float>(value);
            break;
        case FieldKind::PosY:
            player->actor.world.pos.y = static_cast<float>(value);
            break;
        case FieldKind::PosZ:
            player->actor.world.pos.z = static_cast<float>(value);
            break;
        case FieldKind::RotY:
            player->actor.shape.rot.y = static_cast<int16_t>(value);
            break;
        case FieldKind::Speed:
            player->linearVelocity = static_cast<float>(value);
            break;
    }
}

int LuaPlayerGet(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    const PlayerField* field = FindPlayerField(name);
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (field == nullptr || player == nullptr) {
        lua_pushnil(state);
        return 1;
    }
    lua_pushnumber(state, ReadPlayerField(*field, player));
    return 1;
}

int LuaPlayerSet(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    const double value = luaL_checknumber(state, 2);
    const PlayerField* field = FindPlayerField(name);
    if (field == nullptr) {
        SPDLOG_WARN("ShipLua player.set: campo '{}' desconhecido", name);
        lua_pushboolean(state, 0);
        return 1;
    }
    if (!field->writable || !std::isfinite(value) || value < field->min || value > field->max) {
        SPDLOG_WARN("ShipLua player.set: valor inv\xC3\xA1lido para '{}'", name);
        lua_pushboolean(state, 0);
        return 1;
    }
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr) {
        lua_pushboolean(state, 0);
        return 1;
    }
    WritePlayerField(*field, player, value);
    lua_pushboolean(state, 1);
    return 1;
}

// ship.oot.player.set_bunny_hood(equipped): veste a Bunny Hood do OoT e liga
// o comportamento de Majora's Mask (corrida mais rápida e pulo maior), que o
// próprio host já implementa atrás do enhancement MMBunnyHood. Ao desequipar,
// o valor anterior do enhancement é restaurado.
bool gBunnyHoodForced = false;
int gBunnyHoodPreviousMode = BUNNY_HOOD_VANILLA;
int gBunnyHoodPreviousPersistent = 0;

int LuaSetBunnyHood(lua_State* state) {
    const bool equip = lua_toboolean(state, 1) != 0;
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || (player->stateFlags1 & PLAYER_STATE1_DEAD) != 0) {
        SPDLOG_WARN("ShipLua set_bunny_hood: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }

    if (equip) {
        if (!gBunnyHoodForced) {
            gBunnyHoodPreviousMode = CVarGetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_VANILLA);
            gBunnyHoodPreviousPersistent = CVarGetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 0);
            gBunnyHoodForced = true;
        }
        CVarSetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), BUNNY_HOOD_FAST_AND_JUMP);
        // Sem PersistentMasks, Player_ProcessItemButtons zera currentMask no
        // frame seguinte quando a máscara não está num botão C — e a Bunny
        // Hood some antes de ser desenhada.
        CVarSetInteger(CVAR_ENHANCEMENT("PersistentMasks"), 1);
        player->currentMask = PLAYER_MASK_BUNNY;
        gSaveContext.ship.maskMemory = PLAYER_MASK_BUNNY;
        SPDLOG_INFO("ShipLua set_bunny_hood: Bunny Hood equipada com o comportamento de MM");
    } else {
        if (player->currentMask == PLAYER_MASK_BUNNY) {
            player->currentMask = PLAYER_MASK_NONE;
        }
        gSaveContext.ship.maskMemory = PLAYER_MASK_NONE;
        if (gBunnyHoodForced) {
            CVarSetInteger(CVAR_ENHANCEMENT("MMBunnyHood"), gBunnyHoodPreviousMode);
            CVarSetInteger(CVAR_ENHANCEMENT("PersistentMasks"), gBunnyHoodPreviousPersistent);
            gBunnyHoodForced = false;
        }
        SPDLOG_INFO("ShipLua set_bunny_hood: Bunny Hood removida");
    }
    // Mesmo motivo do set_speed_multiplier: sem re-registrar, o hook de
    // velocidade da Bunny Hood não é instalado e a máscara fica só visual.
    ShipInit::Init(CVAR_ENHANCEMENT("MMBunnyHood"));

    lua_pushboolean(state, 1);
    return 1;
}

int LuaSpawnDog(lua_State* state) {
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || (play->sceneNum != SCENE_MARKET_DAY && play->sceneNum != SCENE_MARKET_NIGHT) ||
        Object_GetIndex(&play->objectCtx, OBJECT_DOG) < 0) {
        lua_pushboolean(state, 0);
        return 1;
    }
    Actor* dog =
        Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DOG, player->actor.world.pos.x, player->actor.world.pos.y,
                    player->actor.world.pos.z, 0, player->actor.shape.rot.y, 0, static_cast<s16>(0x8000));
    lua_pushboolean(state, dog != nullptr);
    return 1;
}

void InstallOotApi(lua_State* state) {
    if (state == nullptr) {
        return;
    }
    lua_getglobal(state, "require");
    lua_pushstring(state, "ship");
    if (lua_pcall(state, 1, 1, 0) != LUA_OK || !lua_istable(state, -1)) {
        lua_pop(state, 1);
        return;
    }
    const int shipTable = lua_gettop(state);
    lua_getfield(state, shipTable, "oot");
    if (!lua_istable(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
    }
    const int ootTable = lua_gettop(state);
    lua_pushcfunction(state, LuaSpawnDog);
    lua_setfield(state, ootTable, "spawn_dog");
    lua_getfield(state, ootTable, "player");
    if (!lua_istable(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
    }
    lua_pushcfunction(state, LuaPlayerJump);
    lua_setfield(state, -2, "jump");
    lua_pushcfunction(state, LuaSetBunnyHood);
    lua_setfield(state, -2, "set_bunny_hood");
    lua_pushcfunction(state, LuaSetMask);
    lua_setfield(state, -2, "set_mask");
    lua_pushcfunction(state, LuaAttachModel);
    lua_setfield(state, -2, "attach_model");
    lua_pushcfunction(state, LuaSetDamageImmunity);
    lua_setfield(state, -2, "set_damage_immunity");
    lua_pushcfunction(state, LuaSetWeight);
    lua_setfield(state, -2, "set_weight");
    lua_pushcfunction(state, LuaSetRollMode);
    lua_setfield(state, -2, "set_roll_mode");
    lua_setfield(state, ootTable, "player");
    lua_setfield(state, shipTable, "oot");

    // Primitiva comum aos dois jogos: ship.player.set_speed_multiplier.
    // Precisa ficar ANTES do pop final — depois dele o índice shipTable já
    // não é válido.
    lua_getfield(state, shipTable, "player");
    if (!lua_istable(state, -1)) {
        lua_pop(state, 1);
        lua_newtable(state);
    }
    lua_pushcfunction(state, LuaSetSpeedMultiplier);
    lua_setfield(state, -2, "set_speed_multiplier");
    lua_pushcfunction(state, LuaPlayerGet);
    lua_setfield(state, -2, "get");
    lua_pushcfunction(state, LuaPlayerSet);
    lua_setfield(state, -2, "set");
    lua_setfield(state, shipTable, "player");

    lua_pop(state, 1);
}

// Prefixo de namespace dos assets do MM dentro do OOT: todo o mm.o2r fica
// endereçável como "mm/<caminho original>".
constexpr const char* kMmNamespace = "mm/";

// Archive que lê DIRETO do mm.o2r da instalação do MM, sem copiar nada:
// - toda entrada é exposta com o prefixo "mm/" (100% do arquivo, sem colisão);
// - entradas cujo caminho original NÃO existe no OOT também ganham alias no
//   hash original, para as referências internas dos resources (DL→textura
//   etc., gravadas por hash do caminho sem prefixo) resolverem sozinhas.
// Só as ~5% de entradas que colidem com caminhos do OOT ficam sem alias.
class MmCrossWorldArchive final : public Ship::Archive {
  public:
    // O prefixo é parametrizável: "mm/" para o archive do jogo vizinho e
    // "mod/<id>/" para os assets próprios de um mod.
    MmCrossWorldArchive(const std::string& path, Ship::ArchiveManager* manager, std::string prefix = kMmNamespace)
        : Ship::Archive(path), mInner(std::make_shared<Ship::O2rArchive>(path)), mManager(manager),
          mPrefix(std::move(prefix)) {
    }

    bool Open() override {
        if (!mInner->Open()) {
            SPDLOG_WARN("ShipLua n\xC3\xA3o abriu o archive interno '{}'", GetPath());
            return false;
        }
        std::size_t aliased = 0;
        std::size_t blocked = 0;
        const auto files = mInner->ListFiles();
        for (const auto& [hash, filePath] : *files) {
            IndexFile(mPrefix + filePath);
            // Alias no hash original apenas para dados de render: sistemas como
            // o de áudio do SoH ENUMERAM o índice global (audio/*) e quebram ao
            // encontrar entradas do MM em formato próprio. objects/ e textures/
            // só são resolvidos por referência direta (hash/caminho), nunca por
            // varredura — seguros de aliasar.
            const bool renderData = filePath.rfind("objects/", 0) == 0 || filePath.rfind("textures/", 0) == 0;
            if (renderData && mManager != nullptr && !mManager->HasFile(filePath)) {
                IndexFile(filePath);
                ++aliased;
            } else {
                ++blocked;
            }
        }
        mOwnIndex = ListFiles();
        SPDLOG_INFO("ShipLua exp\xC3\xB4s {} assets sob '{}' ({} com alias direto, {} bloqueados por "
                    "colis\xC3\xA3o de caminho)",
                    files->size(), mPrefix, aliased, blocked);
        return !files->empty();
    }

    bool Close() override {
        mOwnIndex.reset();
        return mInner->Close();
    }

    std::shared_ptr<Ship::File> LoadFile(const std::string& filePath) override {
        if (!HasFile(filePath)) {
            return nullptr;
        }
        std::shared_ptr<Ship::File> file;
        if (filePath.rfind(mPrefix, 0) == 0) {
            file = mInner->LoadFile(filePath.substr(mPrefix.size()));
        } else {
            file = mInner->LoadFile(filePath);
        }
        SanitizeMmDisplayList(file);
        return file;
    }

    // Display lists do 2ship contêm comandos fora do dialeto do SoH:
    // G_DL_INDEX (0x3D) salta para a tabela de setup-DLs pela convenção do
    // 2ship (segmento inexistente aqui — o salto cairia em lixo e derruba o
    // interpretador), e 0x43+ têm numeração divergente entre os dois LUS.
    // Neutraliza/remapeia no load; os comandos de material inline da própria
    // DL permanecem, e o host já aplica o setup do OOT antes do draw.
    static void SanitizeMmDisplayList(const std::shared_ptr<Ship::File>& file) {
        if (file == nullptr || file->Buffer == nullptr || file->Buffer->size() < 0x48) {
            return;
        }
        std::vector<char>& bytes = *file->Buffer;
        if (std::memcmp(bytes.data() + 4, "TLDO", 4) != 0) {
            return;
        }
        for (std::size_t i = 0x40; i + 8 <= bytes.size();) {
            const uint8_t opcode = static_cast<uint8_t>(bytes[i + 3]);
            std::size_t advance = 8;
            switch (opcode) {
                case 0x20: // SETTIMG_OTR_HASH
                case 0x24: // VTX_OTR_FILEPATH
                case 0x25: // SETTIMG_OTR_FILEPATH
                case 0x27: // DL_OTR_FILEPATH
                case 0x29: // MTX_OTR_FILEPATH
                case 0x31: // DL_OTR_HASH
                case 0x32: // VTX_OTR_HASH
                case 0x33: // MARKER
                case 0x35: // BRANCH_Z_OTR
                case 0x36: // MTX_OTR
                case 0x42: // MOVEMEM_HASH
                    advance = 16;
                    break;
                case 0x3D: // G_DL_INDEX (convenção 2ship) — neutraliza
                case 0x43: // G_LOAD_SHADER do MM (inexistente no SoH) — neutraliza
                    std::memset(bytes.data() + i, 0, 8);
                    break;
                case 0x44: // SETTILESIZE_INTERP: MM 0x44 → SoH 0x45
                    bytes[i + 3] = 0x45;
                    break;
                case 0x45: // SETTARGETINTERPINDEX: MM 0x45 → SoH 0x46
                    bytes[i + 3] = 0x46;
                    break;
                default:
                    break;
            }
            i += advance;
            if (opcode == 0xDF) { // G_ENDDL
                break;
            }
        }
    }

    std::shared_ptr<Ship::File> LoadFile(uint64_t hash) override {
        if (mOwnIndex == nullptr) {
            return nullptr;
        }
        const auto it = mOwnIndex->find(hash);
        if (it == mOwnIndex->end()) {
            return nullptr;
        }
        return LoadFile(it->second);
    }

    bool WriteFile(const std::string&, const std::vector<uint8_t>&) override {
        return false;
    }

  private:
    std::shared_ptr<Ship::O2rArchive> mInner;
    Ship::ArchiveManager* mManager = nullptr;
    std::string mPrefix;
    std::shared_ptr<std::unordered_map<uint64_t, std::string>> mOwnIndex;
};

// Monta em runtime o mm.o2r da instalação irmã do MM (../MM por convenção,
// override via SHIPLUA_MM_ROOT). Vice-versa equivalente vive no host MM.
void MountCrossWorldArchives() {
    Ship::Context* shipContext = Ship::Context::GetRawInstance();
    if (shipContext == nullptr || shipContext->GetResourceManager() == nullptr) {
        return;
    }
    const auto archiveManager = shipContext->GetResourceManager()->GetArchiveManager();
    if (archiveManager == nullptr) {
        return;
    }

    // Sonda os layouts conhecidos: override explícito, pacote do launcher
    // (raiz/hosts/mm), host irmão ao lado do exe e, por último, o CWD.
    std::error_code ec;
    std::vector<std::filesystem::path> candidates;
    // O override é exclusivo: quem aponta SHIPLUA_MM_ROOT espera aquele
    // caminho, não uma busca automática em volta dele.
    if (const char* configured = std::getenv("SHIPLUA_MM_ROOT"); configured != nullptr && *configured != '\0') {
        candidates.emplace_back(configured);
    } else {
    // O layout do launcher mantém os dois archives na raiz do pacote (é onde
    // o próprio oot.o2r é encontrado), então a pasta do app vem primeiro.
    const std::filesystem::path cwd = std::filesystem::absolute(Ship::Context::GetAppDirectoryPath(""), ec);
    candidates.push_back(cwd);
    candidates.push_back(cwd / "hosts" / "mm");
    candidates.push_back(cwd.parent_path());
    candidates.push_back(cwd.parent_path() / "mm");
    candidates.push_back(cwd.parent_path() / "MM");
    candidates.push_back(cwd.parent_path().parent_path() / "hosts" / "mm");
#ifdef _WIN32
    const std::filesystem::path runtimeRoot = RuntimeRoot();
    candidates.push_back(runtimeRoot);
    candidates.push_back(runtimeRoot / "hosts" / "mm");
    candidates.push_back(runtimeRoot.parent_path());
    candidates.push_back(runtimeRoot.parent_path() / "mm");
    candidates.push_back(runtimeRoot.parent_path().parent_path());
    candidates.push_back(runtimeRoot.parent_path().parent_path() / "hosts" / "mm");
#endif
    }

    std::filesystem::path mmArchive;
    for (const std::filesystem::path& candidate : candidates) {
        const std::filesystem::path probe = candidate / "mm.o2r";
        if (std::filesystem::is_regular_file(probe, ec)) {
            mmArchive = probe;
            break;
        }
    }
    if (mmArchive.empty()) {
        std::string tried;
        for (const auto& candidate : candidates) {
            tried += (tried.empty() ? "" : "; ") + candidate.string();
        }
        SPDLOG_INFO("ShipLua: mm.o2r n\xC3\xA3o encontrado (procurado em: {}) — assets do MM indispon\xC3\xADveis "
                    "no OOT",
                    tried);
        return;
    }

    const auto crossWorld = std::make_shared<MmCrossWorldArchive>(mmArchive.string(), archiveManager.get());
    crossWorld->Load();
    if (crossWorld->IsLoaded() && archiveManager->AddArchive(crossWorld) != nullptr) {
        SPDLOG_INFO("ShipLua montou o mm.o2r do MM em modo cross-world: {}", mmArchive.string());
    } else {
        SPDLOG_WARN("ShipLua n\xC3\xA3o conseguiu montar '{}'", mmArchive.string());
    }
}

// PASSO 1 — assets próprios de mod.
// Todo archive (.o2r/.otr) encontrado na pasta de mods vira endereçável sob
// "mod/<nome>/", sem colidir com os assets do jogo. Aceita tanto um archive
// solto (mods/rito_mask.o2r) quanto a convenção de pasta
// (mods/rito-mask/assets/*.o2r). É o que permite a um mod trazer conteúdo
// NOVO, em vez de apenas remixar o que já existe nos dois jogos.
void MountModAssetArchives() {
    Ship::Context* shipContext = Ship::Context::GetRawInstance();
    if (shipContext == nullptr || shipContext->GetResourceManager() == nullptr) {
        return;
    }
    const auto archiveManager = shipContext->GetResourceManager()->GetArchiveManager();
    if (archiveManager == nullptr) {
        return;
    }

    std::error_code ec;
    const std::filesystem::path modsRoot =
        Ship::Context::GetPathRelativeToAppDirectory("mods", Ship::Context::GetRawInstance()->GetShortName());
    if (!std::filesystem::is_directory(modsRoot, ec)) {
        return;
    }

    std::vector<std::filesystem::path> archives;
    for (const auto& entry : std::filesystem::directory_iterator(modsRoot, ec)) {
        if (entry.is_regular_file()) {
            const std::string extension = entry.path().extension().string();
            if (extension == ".o2r" || extension == ".otr") {
                archives.push_back(entry.path());
            }
        } else if (entry.is_directory()) {
            const std::filesystem::path assetsDir = entry.path() / "assets";
            if (!std::filesystem::is_directory(assetsDir, ec)) {
                continue;
            }
            for (const auto& asset : std::filesystem::directory_iterator(assetsDir, ec)) {
                const std::string extension = asset.path().extension().string();
                if (asset.is_regular_file() && (extension == ".o2r" || extension == ".otr")) {
                    archives.push_back(asset.path());
                }
            }
        }
    }

    std::size_t mounted = 0;
    for (const std::filesystem::path& archivePath : archives) {
        const std::string id = archivePath.stem().string();
        const auto modArchive =
            std::make_shared<MmCrossWorldArchive>(archivePath.string(), archiveManager.get(), "mod/" + id + "/");
        modArchive->Load();
        if (modArchive->IsLoaded() && archiveManager->AddArchive(modArchive) != nullptr) {
            ++mounted;
            SPDLOG_INFO("ShipLua montou assets do mod '{}' sob 'mod/{}/': {}", id, id, archivePath.string());
        } else {
            SPDLOG_WARN("ShipLua n\xC3\xA3o conseguiu montar os assets de mod '{}'", archivePath.string());
        }
    }
    if (mounted > 0) {
        SPDLOG_INFO("ShipLua: {} pacote(s) de assets de mod montado(s)", mounted);
    }
}

void LoadModsAndDispatchReady(const ShipLua::LuaApiHostContext& context) {
    Ship::Context* shipContext = Ship::Context::GetRawInstance();
    if (shipContext == nullptr) {
        SPDLOG_ERROR("ShipLua n\xC3\xA3o encontrou o contexto do aplicativo");
        return;
    }

    const std::string appName = shipContext->GetShortName();
    const std::filesystem::path modsRoot = Ship::Context::GetPathRelativeToAppDirectory("mods", appName);
    const std::filesystem::path cacheRoot = modsRoot / ".shiplua-cache";
    std::error_code error;
    std::filesystem::create_directories(modsRoot, error);
    if (error) {
        SPDLOG_ERROR("ShipLua n\xC3\xA3o conseguiu criar a pasta de mods '{}': {}", modsRoot.string(), error.message());
        return;
    }

    auto loaded = gModHost->LoadModsFromRoot(modsRoot, cacheRoot);
    if (!loaded.isOk()) {
        SPDLOG_ERROR("ShipLua n\xC3\xA3o conseguiu carregar a pasta de mods '{}': {}", modsRoot.string(),
                     loaded.message);
        return;
    }
    for (const auto& [modId, reason] : loaded.value->rejected) {
        SPDLOG_WARN("ShipLua rejeitou o mod '{}': {}", modId, reason);
    }
    SPDLOG_INFO("ShipLua carregou {} mod(s) de '{}'", loaded.value->loadedIds.size(), modsRoot.string());

    for (const std::string& modId : loaded.value->loadedIds) {
        ShipLua::LuaRuntime* runtime = gModHost->GetRuntime(modId);
        if (runtime != nullptr) {
            InstallOotApi(runtime->State());
        }
    }

    ShipLua::EventPayload payload{
        { "game_id", context.gameId },
        { "host_version", context.hostVersion },
        { "runtime_version", context.runtimeVersion },
        { "api_version", std::string(ShipLua::Generated::kApiVersion) },
    };
    auto ready = gModHost->DispatchEvent("game.ready", payload);
    if (!ready.isOk()) {
        SPDLOG_ERROR("ShipLua n\xC3\xA3o conseguiu publicar game.ready: {}", ready.message);
        return;
    }
    for (const ShipLua::CallbackFailure& failure : ready.value->failures) {
        SPDLOG_ERROR("ShipLua [{}] falhou em game.ready: {}", failure.modId, failure.message);
    }
}

} // namespace

void Initialize() {
    if (gModHost != nullptr) {
        SPDLOG_WARN("ShipLua j\xC3\xA1 foi inicializado");
        return;
    }

    gHotkeys = std::make_shared<OotHotkeyRegistry>();
    gCapabilityRegistry = std::make_shared<ShipLua::CapabilityRegistry>();
    gActorProvider = CreateActorProvider();
    const auto actorCapabilities = gActorProvider->RegisterCapabilities(*gCapabilityRegistry);
    if (!actorCapabilities.isOk()) {
        SPDLOG_ERROR("ShipLua failed to register the OoT actor provider: {}", actorCapabilities.message);
        gActorProvider.reset();
        gCapabilityRegistry.reset();
        gHotkeys.reset();
        return;
    }
    auto catalog = ShipLua::PortableItemCatalog::CreateDefault();
    if (!catalog.isOk()) {
        SPDLOG_ERROR("ShipLua não conseguiu criar o catálogo portátil OoT: {}", catalog.message);
        gActorProvider.reset();
        gCapabilityRegistry.reset();
        gHotkeys.reset();
        return;
    }
    gWorldAdapter = std::make_shared<OotWorldAdapter>(std::move(*catalog.value));
    gLoadGameHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnLoadGame>(
        [](int32_t) { TryConsumeWorldHandoff(); });
    gImportTickHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() { TickWorldImport(); });
    // attach_model: substitui a DL da máscara-veículo pela do mod. O hook roda
    // já dentro do contexto de matriz da cabeça, então basta emitir a DL.
    // Imunidade a fogo: apaga o corpo em chamas antes que o dano contínuo seja
    // aplicado. Rodar por frame é o suficiente — Player_UpdateBodyBurn só age
    // enquanto bodyIsBurning estiver ligado.
    gFireImmunityHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!gFireImmunity || gPlayState == nullptr) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        if (player != nullptr && player->bodyIsBurning) {
            player->bodyIsBurning = false;
            for (int i = 0; i < PLAYER_BODYPART_MAX; ++i) {
                player->bodyFlameTimers[i] = 0;
            }
        }
    });
    // Rolamento contínuo e dirigível, modelado no Player_Action_96 do MM:
    // - o rolamento se re-arma sozinho enquanto houver direção no analógico
    //   (no MM não se aperta nada para continuar rolando);
    // - a direção acompanha o analógico de verdade;
    // - bater na parede continua caindo no "bonk" do próprio OoT, que roda
    //   antes deste ponto — é o "até bater em algo".
    // Estes hooks NÃO são um simples true/false: o CHAIN precisa chamar
    // Player_SetupRoll e o STEER precisa girar o player ele mesmo.
    gRollChainHook = REGISTER_VB_SHOULD(VB_PLAYER_ROLL_CHAIN, {
        Player* player = va_arg(args, Player*);
        PlayState* play = va_arg(args, PlayState*);
        Input* controlInput = va_arg(args, Input*);
        const s32 floorType = va_arg(args, s32);
        if (!gChainRoll || player == nullptr || play == nullptr || controlInput == nullptr) {
            return;
        }
        // floorType 7 é a superfície onde o vanilla proíbe encadear.
        if ((player->skelAnime.curFrame < 15.0f) || (floorType == 7)) {
            return;
        }
        // Sem direção no analógico o rolamento termina naturalmente.
        const s32 stickX = controlInput->rel.stick_x;
        const s32 stickY = controlInput->rel.stick_y;
        if (((stickX * stickX) + (stickY * stickY)) < (kRollStickDeadzone * kRollStickDeadzone)) {
            return;
        }
        Player_SetupRoll(player, play);
        *should = true;
    });
    gRollSteerHook = REGISTER_VB_SHOULD(VB_PLAYER_ROLL_STEER, {
        Player* player = va_arg(args, Player*);
        [[maybe_unused]] PlayState* play = va_arg(args, PlayState*);
        const s16 yawTarget = static_cast<s16>(va_arg(args, int));
        if (!gChainRoll || player == nullptr) {
            return;
        }
        // Passo maior que o do "improved roll" do SoH: o Goron do MM vira
        // rápido enquanto rola.
        Math_ScaledStepToS(&player->actor.shape.rot.y, yawTarget, kRollSteerStep);
        *should = false;
    });
    gMaskDrawHook = REGISTER_VB_SHOULD(VB_DRAW_PLAYER_MASK, {
        if (gAttachedHeadModel.empty()) {
            return;
        }
        va_arg(args, uint32_t); // currentMask (não usado: o modelo é do mod)
        PlayState* play = va_arg(args, PlayState*);
        if (play == nullptr) {
            return;
        }
        ShipLuaEmitDisplayList(play, gAttachedHeadModel.c_str());
        *should = false; // não desenha a máscara vanilla por cima
    });

    // hook.oot.player.speed.run — VB_PLAYER_MODIFY_RUN_SPEED(Player*, f32* speedTarget).
    // O retorno booleano do hook é ignorado pelo próprio engine no call site
    // (chamada solta, sem if): só o *speedTarget mutado importa.
    gHookRunSpeedHook = REGISTER_VB_SHOULD(VB_PLAYER_MODIFY_RUN_SPEED, {
        va_arg(args, Player*);
        f32* speedTarget = va_arg(args, f32*);
        if (speedTarget == nullptr) {
            return;
        }
        const auto result =
            DispatchHookTransform("hook.oot.player.speed.run", ShipLua::EventPayload{
                                                                    {"speed", static_cast<double>(*speedTarget)},
                                                                });
        if (result.has_value() && std::holds_alternative<double>(result->value)) {
            *speedTarget = static_cast<f32>(std::get<double>(result->value));
        }
    });
    // hook.oot.player.fall_damage — VB_RECIEVE_FALL_DAMAGE(Actor*); default true
    // (dano aplicado). Lua devolvendo false via ship.hooks.result cancela o dano.
    gHookFallDamageHook = REGISTER_VB_SHOULD(VB_RECIEVE_FALL_DAMAGE, {
        va_arg(args, Actor*);
        const auto result = DispatchHookTransform("hook.oot.player.fall_damage", ShipLua::EventPayload{});
        if (result.has_value() && std::holds_alternative<bool>(result->value)) {
            *should = std::get<bool>(result->value);
        }
    });
    gHookItemReceiveHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnItemReceive>([](GetItemEntry itemEntry) {
            DispatchHookEvent("hook.oot.item.receive",
                              ShipLua::EventPayload{
                                  {"item_id", static_cast<std::int64_t>(itemEntry.itemId)},
                                  {"get_item_id", static_cast<std::int64_t>(itemEntry.getItemId)},
                              });
        });
    gHookHealthChangeHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerHealthChange>([](int16_t amount) {
            DispatchHookEvent("hook.oot.player.health_change",
                              ShipLua::EventPayload{{"amount", static_cast<std::int64_t>(amount)}});
        });
    gHookBonkHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBonk>(
        []() { DispatchHookEvent("hook.oot.player.bonk", ShipLua::EventPayload{}); });

    gActorDestroyHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnActorDestroy>([](void* actor) {
        ShipLuaPuppet_HandleActorDestroy(actor);
        if (gActorProvider == nullptr) {
            return;
        }
        const auto destroyed = gActorProvider->OnNativeActorDestroyed(actor);
        if (!destroyed.isOk()) {
            SPDLOG_ERROR("ShipLua failed to invalidate a destroyed OoT actor: {}", destroyed.message);
        }
    });
    gPlayDestroyHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDestroy>([]() {
        ShipLuaPuppet_Reset();
        if (gActorProvider == nullptr) {
            return;
        }
        const auto cleaned = gActorProvider->OnSceneChange();
        if (!cleaned.isOk()) {
            SPDLOG_ERROR("ShipLua failed to clean OoT actors during scene teardown: {}", cleaned.message);
        }
    });
    auto contextResult = CreateHostContext();
    if (!contextResult.isOk()) {
        SPDLOG_ERROR("ShipLua failed to create the OoT host context: {}", contextResult.message);
        Shutdown();
        return;
    }
    ShipLua::LuaApiHostContext context = std::move(*contextResult.value);
    SPDLOG_INFO("ShipLua inicializando para {} {} (commit {})", context.gameId, context.hostVersion, gGitCommitHash);
    gModHost = std::make_unique<ShipLua::ModHost>(context, CreateLogger());
    MountCrossWorldArchives();
    MountModAssetArchives();
    LoadModsAndDispatchReady(context);
    SPDLOG_INFO("ShipLua inicializado");
}

void Shutdown() {
    if (gModHost == nullptr && gActorProvider == nullptr) {
        return;
    }

    if (gActorProvider != nullptr) {
        const auto cleaned = gActorProvider->Shutdown();
        if (!cleaned.isOk()) {
            SPDLOG_ERROR("ShipLua failed to shut down the OoT actor provider: {}", cleaned.message);
        }
    }
    gModHost.reset();
    if (gLoadGameHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnLoadGame>(gLoadGameHook);
        gLoadGameHook = 0;
    }
    if (gImportTickHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnGameFrameUpdate>(gImportTickHook);
        gImportTickHook = 0;
    }
    gPendingHandoff.reset();
    if (gActorDestroyHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnActorDestroy>(gActorDestroyHook);
        gActorDestroyHook = 0;
    }
    if (gPlayDestroyHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayDestroy>(gPlayDestroyHook);
        gPlayDestroyHook = 0;
    }
    gActorProvider.reset();
    gCapabilityRegistry.reset();
    gWorldAdapter.reset();
    gHotkeys.reset();
    SPDLOG_INFO("ShipLua finalizado");
}

ShipLua::ModHost* GetModHost() {
    return gModHost.get();
}

OotActorProvider* ActorProvider() {
    return gActorProvider.get();
}

OotHotkeyRegistry* Hotkeys() {
    return gHotkeys.get();
}

OotWorldAdapter* WorldAdapter() {
    return gWorldAdapter.get();
}

void OpenLogWindow() {
#ifdef _WIN32
    const std::filesystem::path log = RuntimeRoot() / "logs" / "Ship of Harkinian.log";
    std::filesystem::create_directories(log.parent_path());
    std::wstring command =
        L"powershell.exe -NoLogo -NoProfile -NoExit -Command \"$host.UI.RawUI.WindowTitle='Link-Span - log OoT'; "
        L"Write-Host 'Aguardando o log de OoT...'; while(-not (Test-Path -LiteralPath " +
        QuotePowerShellLiteral(log.wstring()) + L")){Start-Sleep -Milliseconds 250}; Get-Content -LiteralPath " +
        QuotePowerShellLiteral(log.wstring()) + L" -Tail 200 -Wait\"";
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION process{};
    if (!CreateProcessW(nullptr, command.data(), nullptr, nullptr, FALSE, CREATE_NEW_CONSOLE, nullptr,
                        RuntimeRoot().c_str(), &startup, &process)) {
        SPDLOG_ERROR("ShipLua não conseguiu abrir a janela de log (erro {})", GetLastError());
        return;
    }
    SPDLOG_INFO("ShipLua abriu uma nova janela para acompanhar o log OoT");
    CloseHandle(process.hThread);
    CloseHandle(process.hProcess);
#else
    SPDLOG_WARN("ShipLua OpenLogWindow só está disponível no Windows");
#endif
}

} // namespace ShipLuaHost

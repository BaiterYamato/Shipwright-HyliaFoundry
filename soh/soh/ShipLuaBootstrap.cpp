#include "ShipLuaBootstrap.h"
#include "OotActorProvider.h"
#include "OotHotkeyRegistry.h"
#include "OotWorldAdapter.h"
#include "ShipLuaPuppet.h"
#include "soh/Enhancements/item-tables/ItemTableTypes.h"
#include "soh/Enhancements/item-tables/ItemTableManager.h"
// gMagicMeterFillTex: textura de preenchimento sólido reaproveitada por
// ship.hud.draw_rect (mesma que o medidor de magia usa).
#include "textures/parameter_static/parameter_static.h"

#include <filesystem>
#include <algorithm>
#include <array>
#include <cmath>
#include <cstdlib>
#include <cstdint>
#include <map>
#include <memory>
#include <optional>
#include <set>
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
#include <shiplua/storage/KeyValueStorage.h>
#include <shiplua/timer/FrameTimerScheduler.h>
#include <shiplua/world/WorldHandoff.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Enhancements/enhancementTypes.h"
#include "soh/ResourceManagerHelpers.h"
#include "soh/mmaudio/MmSoundFont.h"
#include "soh/mmaudio/MmSfxPlayer.h"
#include "align_asset_macro.h"
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
// Declaradas em z_player.c; iniciam e identificam o rolamento nativo do OoT.
// O parâmetro se chama "this" no C original — aqui precisa de outro nome.
void Player_SetupRoll(Player* player, PlayState* play);
void Player_Action_Roll(Player* player, PlayState* play);
// Solta o jogador da escada e o deixa cair: limpa CLIMBING_LADDER, transiciona
// a ação e dá um empurrão para trás. É o mesmo caminho que o engine usa quando
// o jogador larga a escada (z_player.c:7890) — não é invenção nossa.
void func_8083FB7C(Player* player, PlayState* play);
void Player_Action_80845EF8(Player* player, PlayState* play);

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
// Timers por frame (ship.timer.after/every). Precisa de Tick() todo frame —
// ver o hook OnGameFrameUpdate no Initialize. Sem isto, core.timers fica
// indisponível e mods que dependem de sequenciamento (por exemplo a animação
// de colocar máscara antes de trocar o corpo) degradam para ação instantânea.
std::shared_ptr<ShipLua::FrameTimerScheduler> gTimers;
// Stores persistentes. Guardados aqui para o flush periódico e o do shutdown —
// a gravação é adiada de propósito (ver KeyValueStorage::EnablePersistence).
std::shared_ptr<ShipLua::KeyValueStorage> gStorage;
std::shared_ptr<ShipLua::KeyValueStorage> gSharedStorage;
std::shared_ptr<OotWorldAdapter> gWorldAdapter;
HOOK_ID gLoadGameHook = 0;
HOOK_ID gImportTickHook = 0;
std::optional<ShipLua::WorldHandoff> gPendingHandoff;
HOOK_ID gActorDestroyHook = 0;
HOOK_ID gPlayDestroyHook = 0;
HOOK_ID gCustomBodyRestoreHook = 0;

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
                             "oot.player.weight",       "oot.player.roll",    "hooks.bridge",
                             "oot.player.custom_body",  "oot.player.held_item_model",
                             "hud.draw",                "oot.env",
                             "oot.cutscene" };
    context.hotkeys = gHotkeys;
    context.capabilityRegistry = gCapabilityRegistry;
    context.actors = gActorProvider;
    context.timers = gTimers;
    context.capabilities.push_back("core.timers");
    auto registered = RegisterHostCapability("oot.player.jump", "Apply a validated jump impulse to OoT Link.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("core.timers", "Per-frame timers owned by each mod (ship.timer).");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability(
        "hud.draw", "Draw arbitrary rectangles and text over the HUD from hook.<game>.hud.draw.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability("oot.env", "Read ambient world state: time of day, night flag and scene id.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability(
        "oot.cutscene", "Take over the camera with a dedicated sub-camera for a bounded number of frames.");
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
    registered = RegisterHostCapability(
        "oot.player.custom_body",
        "Replace the player's visual body with any mod-provided skeleton and named animations, surviving scene "
        "changes automatically.");
    if (!registered.isOk()) {
        return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
    }
    registered = RegisterHostCapability(
        "oot.player.held_item_model",
        "Draw an arbitrary display list anchored to the player's hand bones, updated every frame.");
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

    // ship.storage: store persistente por mod, gravado no diretório do app (um
    // arquivo para todos os mods, namespaced internamente por mod). Antes disto
    // o host real nunca conectava um KeyValueStorage — ship.storage.set caía em
    // Unsupported e nada sobrevivia à sessão. Um arquivo corrompido não trava o
    // host (começa vazio e é sobrescrito na próxima gravação).
    {
        auto storage = std::make_shared<ShipLua::KeyValueStorage>();
        const std::filesystem::path storagePath = Ship::Context::GetPathRelativeToAppDirectory(
            "shiplua-storage.bin", Ship::Context::GetRawInstance()->GetShortName());
        const auto loaded = storage->EnablePersistence(storagePath);
        if (!loaded.isOk()) {
            SPDLOG_WARN("ShipLua: storage persistente corrompido ({}), recomeçando vazio: {}",
                        storagePath.string(), loaded.message);
        }
        gStorage = storage;
        context.storage = storage;
        context.capabilities.push_back("core.storage");
        registered = RegisterHostCapability(
            "core.storage", "Per-mod key-value storage persisted to disk across sessions.");
        if (!registered.isOk()) {
            return ShipLua::Result<ShipLua::LuaApiHostContext>::err(registered.code, registered.message);
        }
    }

    // ship.storage.shared: store COMPARTILHADO entre os dois jogos, num arquivo
    // no diretório de sessão do launcher. Só existe sob o launcher (BridgeConfig);
    // standalone não tem para onde compartilhar, então fica ausente e degrada.
    if (const auto& bridge = GetBridgeConfig(); bridge.has_value()) {
        auto shared = std::make_shared<ShipLua::KeyValueStorage>();
        const std::filesystem::path sharedPath = bridge->sessionDirectory / "shared-storage.bin";
        const auto loaded = shared->EnablePersistence(sharedPath);
        if (!loaded.isOk()) {
            SPDLOG_WARN("ShipLua: storage compartilhado corrompido ({}), recomeçando vazio: {}",
                        sharedPath.string(), loaded.message);
        }
        gSharedStorage = shared;
        context.sharedStorage = shared;
        context.capabilities.push_back("core.storage.shared");
        registered = RegisterHostCapability(
            "core.storage.shared", "Cross-game key-value storage shared between OoT and MM under the launcher.");
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

// ---------------------------------------------------------------------------
// Corpo visual customizado do player — v2, generalizada a partir da v1 (que
// só sabia fazer Goron). O mecanismo inteiro já era genérico; só a API não
// era. Nenhum nome de forma é conhecido pelo host: skeleton e animações são
// caminhos de resource arbitrários que o MOD fornece via ship.oot.player.
// set_body({skeleton=..., anims={nome=path,...}, default_anim=...}) — Zora,
// Deku, um lobo (Wolfos/Wolf Link), ou qualquer skeleton compatível com
// SkelAnime_InitLink cabem sem tocar em C++ de novo.
//
// O bloqueio original era real: skeletons com mais limbs que PLAYER_LIMB_MAX
// (22) não cabem nos buffers fixos do Player. A saída (achada minerando um
// fork de referência que já resolveu isto em produção) é NÃO reaproveitar o
// skelAnime do Player: usar um SkelAnime independente, inicializado com
// jointTable/morphTable NULL — SkelAnime_InitLink então aloca do tamanho real
// do header, seja lá qual for. Para esconder o corpo vanilla enquanto o nosso
// desenha, usamos PLAYER_STATE2_DISABLE_DRAW — flag vanilla já existente (o
// balconista a usa para esconder o Link atrás do balcão), não invenção nossa.
// O ator hospedeiro (En_Item00) segue a posição do player a cada frame; seu
// próprio draw roda pelo Actor_Draw normal, então a matriz de mundo já vem
// pronta — mesmo padrão do puppet do Kafei/Rauru, só que seguindo o player.
//
// Qual animação toca a cada frame é decidido pelo hook.oot.player.
// body_anim_select (Transform): o mod recebe speed/on_ground e devolve o
// NOME da animação (uma chave da tabela `anims` que ele mesmo definiu) — o
// host não sabe o que "andar" ou "nadar" significa para a forma de cada mod.
// Sem callback (ou resultado inválido), cai num fallback só quando o mod usa
// as chaves convencionais "idle"/"walk"/"run" (compat com a v1 e o caminho
// mais comum).
//
// Nota: "misc/link_animetion/*_Data" é o blob bruto de keyframes; o
// LinkAnimationHeader de verdade (o que SkelAnime_InitLink/LinkAnimation_Change
// esperam) é o símbolo SEM o sufixo _Data, em objects/gameplay_keep/. Usar o
// _Data direto faz o parser ler a estrutura errada e crashar dentro do
// próprio SkelAnime_InitLink (achado por um crash real, não suposição).
// Definida mais abaixo (seção "ponte de hooks") — declarada aqui porque
// CustomBodyActorUpdate, logo abaixo, precisa dela antes dessa seção existir
// no arquivo.
std::optional<ShipLua::EventValue> DispatchHookTransform(const char* name, ShipLua::EventPayload payload);

// ATENÇÃO ao mexer aqui: LinkAnimation_Change guarda o ponteiro ORIGINAL que
// recebe (z_skelanime.c:1292, `skelAnime->animation = ogAnim`) e o re-resolve
// a cada frame dentro de LinkAnimation_Update. O mesmo vale para o skeleton.
// Ou seja: as strings de caminho passadas ao engine precisam sobreviver
// enquanto o corpo estiver ativo — passar uma std::string temporária dá
// use-after-free no frame SEGUINTE (crash real em AnimationContext_SetLoadFrame,
// não teórico). Por isso os caminhos já prefixados vivem aqui, na spec global,
// e nunca em locais. std::map é node-based: o endereço de um value é estável
// enquanto o elemento não for removido/reatribuído.
struct CustomBodySpec {
    std::string skeletonPath;                 // como o mod informou (para get_body)
    std::map<std::string, std::string> anims; // nome lógico (do mod) -> path do mod
    // Segmentos N64 que a display list do esqueleto lê diretamente (por exemplo,
    // 0x08 para os olhos dos modelos de Player). O host só aceita 1..15.
    std::map<int, std::string> segments;
    // Chaves em `anims` que devem executar em loop reverso. O asset continua
    // único; isso representa fielmente ações como descida de escada, onde MM
    // usa o mesmo header de subida com playSpeed negativo.
    std::set<std::string> reverseAnims;
    // Modelos alternativos associados a estados nativos do Player. Hoje "roll"
    // é usado quando Player_Action_Roll está ativo; a chave continua explícita
    // para não amarrar o contrato à forma Goron.
    std::map<std::string, std::string> models;
    // Postura opcional que pode usar outro skeleton e outro formato de animação
    // (por exemplo, a defesa enrolada do Goron de MM). Diferente de `anims`,
    // estes assets não precisam usar LinkAnimationHeader nem o mesmo número
    // de membros do corpo principal.
    std::string shieldSkeletonPath;
    std::string shieldAnimationPath;
    std::string defaultAnim;
    f32 groundOffset = 0.0f;
    f32 rollOffset = 0.0f;
    // Opt-in de semântica de forma: em água funda, o corpo enrola, afunda e
    // usa o respawn/void-out nativo. O padrão é manter a natação normal do OoT.
    bool waterVoid = false;
    // Goron não agarra bordas no Player de MM. É opt-in para não mudar a ação
    // humana de outros corpos externos.
    bool blockLedgeGrab = false;
    // Espelhos com "__OTR__" já aplicado — é DESTES que os ponteiros passados
    // ao engine saem, e é por isso que eles precisam ser membros persistentes.
    std::string skeletonPathPrefixed;
    std::map<std::string, std::string> animsPrefixed;
    std::map<int, std::string> segmentsPrefixed;
    std::map<std::string, std::string> modelsPrefixed;
    std::string shieldSkeletonPathPrefixed;
    std::string shieldAnimationPathPrefixed;
};

bool gCustomBodyActive = false;
Actor* gCustomBodyActor = nullptr;
SkelAnime gCustomBodySkelAnime{};
SkelAnime gCustomBodyShieldSkelAnime{};
std::string gCustomBodyCurrentAnim;
CustomBodySpec gCustomBodySpec;

// Chamado pelo gate nativo de Player_ActionHandler_12. A função tem linkage C
// porque z_player.c é compilado como C; não reaproveita o toggle global de
// Crowd Control, portanto não altera nem é alterada por esse efeito externo.
extern "C" u8 ShipLua_ShouldBlockLedgeGrabs(void) {
    return gCustomBodyActive && gCustomBodySpec.blockLedgeGrab;
}

// ship.oot.player.set_roll_blocked(bool): veta o rolamento no portão nativo
// (Player_TryRoll). Um sistema de stamina precisa disto — só drenar o medidor
// não impede a ação; sem bloquear, rolar com a barra vazia continua saindo.
bool gRollBlocked = false;

extern "C" u8 ShipLua_ShouldBlockRoll(void) {
    return gRollBlocked ? 1 : 0;
}

int LuaSetRollBlocked(lua_State* state) {
    gRollBlocked = lua_toboolean(state, 1) != 0;
    lua_pushboolean(state, 1);
    return 1;
}

// ---------------------------------------------------------------------------
// Cutscene de transformação: subcâmera orbitando o jogador.
//
// É a peça que faltava para a troca de máscara parecer a de MM/OoTMM. Até
// aqui a transformação congelava o input e cobria a troca com um flash, mas a
// câmera continuava normal — o momento não tinha peso nenhum.
//
// O ciclo de vida da câmera é a parte delicada (o design de providers nativos
// listava justamente isto como risco): a arbitragem entre câmera principal e
// subcâmera precisa ser exata, senão o renderer fica com estado inconsistente.
// A sequência abaixo é a mesma que os chefes do próprio jogo usam
// (z_boss_dodongo.c): entra em modo cutscene, congela atores, limpa
// subcâmeras, cria a nova, põe a principal em WAIT e a nova em ACTIVE. Na
// saída, devolve com func_800C08AC + func_80064534 + solta os atores.
bool gCutsceneActive = false;
s16 gCutsceneCamId = 0;
int gCutsceneFrames = 0;
int gCutsceneElapsed = 0;
float gCutsceneStartDist = 160.0f;
float gCutsceneEndDist = 62.0f;
float gCutsceneHeight = 28.0f;
float gCutsceneSpin = 0.0f;

// Quando true, a cutscene também congelou atores/entrou em csMode e precisa
// desfazer isso na saída. Por padrão a primitiva é SÓ câmera.
bool gCutsceneFroze = false;

void CutsceneUpdate(PlayState* play); // definida abaixo; start() já posiciona

void CutsceneStop(PlayState* play) {
    if (!gCutsceneActive) {
        return;
    }
    gCutsceneActive = false;
    if (play == nullptr) {
        gCutsceneCamId = 0;
        gCutsceneFroze = false;
        return;
    }
    if (gCutsceneCamId != 0) {
        // Devolve o controle à câmera principal antes de descartar a nossa.
        func_800C08AC(play, gCutsceneCamId, 0);
        gCutsceneCamId = 0;
    }
    if (gCutsceneFroze) {
        gCutsceneFroze = false;
        func_80064534(play, &play->csCtx);
        Player* player = GET_PLAYER(play);
        Player_SetCsActionWithHaltedActors(play, player != nullptr ? &player->actor : nullptr, 7);
    }
}

// ship.oot.cutscene.start(frames, opções): assume a câmera por N frames.
int LuaCutsceneStart(lua_State* state) {
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || gSaveContext.gameMode != GAMEMODE_NORMAL) {
        SPDLOG_WARN("ShipLua cutscene.start: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }
    if (gCutsceneActive) {
        lua_pushboolean(state, 0);
        return 1;
    }
    const int frames = static_cast<int>(luaL_checkinteger(state, 1));
    if (frames <= 0 || frames > 600) {
        SPDLOG_WARN("ShipLua cutscene.start: frames fora da faixa 1..600");
        lua_pushboolean(state, 0);
        return 1;
    }
    // Opções são todas opcionais; os padrões dão uma aproximação frontal.
    if (lua_istable(state, 2)) {
        lua_getfield(state, 2, "start_distance");
        gCutsceneStartDist = static_cast<float>(luaL_optnumber(state, -1, 160.0));
        lua_pop(state, 1);
        lua_getfield(state, 2, "end_distance");
        gCutsceneEndDist = static_cast<float>(luaL_optnumber(state, -1, 62.0));
        lua_pop(state, 1);
        lua_getfield(state, 2, "height");
        gCutsceneHeight = static_cast<float>(luaL_optnumber(state, -1, 28.0));
        lua_pop(state, 1);
        lua_getfield(state, 2, "spin");
        gCutsceneSpin = static_cast<float>(luaL_optnumber(state, -1, 0.0));
        lua_pop(state, 1);
        lua_getfield(state, 2, "freeze_player");
        gCutsceneFroze = lua_toboolean(state, -1) != 0;
        lua_pop(state, 1);
    } else {
        gCutsceneStartDist = 160.0f;
        gCutsceneEndDist = 62.0f;
        gCutsceneHeight = 28.0f;
        gCutsceneSpin = 0.0f;
        gCutsceneFroze = false;
    }

    // Por padrão a primitiva é SÓ câmera. Congelar o jogador aqui atropelaria
    // qualquer animação que o mod tenha acabado de tocar — foi exatamente o
    // que quebrou a primeira versão desta cutscene: Player_SetCsActionWithHaltedActors
    // substitui a ação do Link, apagando a animação de colocar a máscara.
    // Quem quiser o congelamento completo pede freeze_player = true.
    if (gCutsceneFroze) {
        func_80064520(play, &play->csCtx);
        Player_SetCsActionWithHaltedActors(play, &player->actor, 1);
    }
    Play_ClearAllSubCameras(play);
    gCutsceneCamId = Play_CreateSubCamera(play);
    if (gCutsceneCamId == SUBCAM_NONE) {
        SPDLOG_WARN("ShipLua cutscene.start: sem slot de subc\xC3\xA2mera dispon\xC3\xADvel");
        lua_pushboolean(state, 0);
        return 1;
    }
    // Semeia com a visão atual antes de ativar, como z_onepointdemo.c faz —
    // sem isto o primeiro frame pode sair de uma posição indefinida.
    if (Camera* seeded = Play_GetCamera(play, gCutsceneCamId); seeded != nullptr) {
        seeded->at = play->view.lookAt;
        seeded->eye = play->view.eye;
        seeded->eyeNext = play->view.eye;
        seeded->fov = play->view.fovy;
    }
    Play_ChangeCameraStatus(play, CAM_ID_MAIN, CAM_STAT_WAIT);
    Play_ChangeCameraStatus(play, gCutsceneCamId, CAM_STAT_ACTIVE);
    gCutsceneFrames = frames;
    gCutsceneElapsed = 0;
    gCutsceneActive = true;
    // Posiciona já neste frame: esperar o próximo update deixaria um frame com
    // a subcâmera em posição indefinida.
    CutsceneUpdate(play);
    SPDLOG_INFO("ShipLua cutscene.start: assumindo a c\xC3\xA2mera por {} frames", frames);
    lua_pushboolean(state, 1);
    return 1;
}

int LuaCutsceneStop(lua_State* state) {
    CutsceneStop(gPlayState);
    lua_pushboolean(state, 1);
    return 1;
}

int LuaCutsceneActive(lua_State* state) {
    lua_pushboolean(state, gCutsceneActive ? 1 : 0);
    return 1;
}

// Roda por frame: aproxima a câmera e encerra sozinha ao fim.
void CutsceneUpdate(PlayState* play) {
    if (!gCutsceneActive || play == nullptr) {
        return;
    }
    Player* player = GET_PLAYER(play);
    if (player == nullptr || gSaveContext.gameMode != GAMEMODE_NORMAL) {
        CutsceneStop(play);
        return;
    }
    if (gCutsceneElapsed >= gCutsceneFrames) {
        CutsceneStop(play);
        return;
    }

    constexpr double kPi = 3.14159265358979323846;
    const double t = static_cast<double>(gCutsceneElapsed) / static_cast<double>(gCutsceneFrames);
    // Ease-out: aproxima rápido e desacelera, dando peso ao instante da troca.
    const double eased = 1.0 - (1.0 - t) * (1.0 - t);
    const double dist = gCutsceneStartDist + (gCutsceneEndDist - gCutsceneStartDist) * eased;

    // Ângulo: parte de frente para o Link e gira conforme "spin" (em voltas).
    const double baseYaw = static_cast<double>(player->actor.shape.rot.y) * (kPi / 32768.0);
    const double angle = baseYaw + kPi + gCutsceneSpin * 2.0 * kPi * eased;

    Vec3f at;
    at.x = player->actor.world.pos.x;
    at.y = player->actor.world.pos.y + gCutsceneHeight;
    at.z = player->actor.world.pos.z;

    Vec3f eye;
    eye.x = at.x + static_cast<f32>(std::sin(angle) * dist);
    eye.y = at.y + gCutsceneHeight * 0.6f;
    eye.z = at.z + static_cast<f32>(std::cos(angle) * dist);

    // Escreve direto na struct da câmera, como os chefes do jogo fazem
    // (z_boss_dodongo.c). NÃO usar Play_CameraSetAtEye aqui: ela termina
    // gravando atLERPStepScale = 0.01f, uma interpolação lentíssima — a câmera
    // se arrasta tão devagar rumo ao alvo que parece que nada aconteceu. Foi
    // por isso que a primeira versão desta cutscene não mostrava nada apesar
    // de iniciar corretamente.
    Camera* cam = Play_GetCamera(play, gCutsceneCamId);
    if (cam != nullptr) {
        cam->at = at;
        cam->eye = eye;
        cam->eyeNext = eye;
    }
    ++gCutsceneElapsed;
}

// ship.oot.env.get(campo): estado do AMBIENTE, não do jogador. Fica separado
// de player.get de propósito — hora do dia e cena não são propriedades do
// Link, e misturar as duas coisas envelhece mal.
//
//   time_of_day  0..1 (0 = meia-noite, 0.5 = meio-dia)
//   is_night     0/1, pela mesma flag que o engine usa (IS_NIGHT)
//   scene_id     número da cena atual
//
// Não há "is_indoors" aqui: o engine não expõe isso de forma direta e uma
// heurística chutada seria pior que nada — o mod já distingue ambientes pela
// própria tabela de cenas.
int LuaEnvGet(lua_State* state) {
    const char* field = luaL_checkstring(state, 1);
    PlayState* play = gPlayState;
    if (field == nullptr) {
        lua_pushnil(state);
        return 1;
    }
    if (std::strcmp(field, "time_of_day") == 0) {
        lua_pushnumber(state, static_cast<double>(gSaveContext.dayTime) / 65535.0);
        return 1;
    }
    if (std::strcmp(field, "is_night") == 0) {
        lua_pushnumber(state, IS_NIGHT ? 1 : 0);
        return 1;
    }
    if (std::strcmp(field, "scene_id") == 0) {
        lua_pushnumber(state, play != nullptr ? static_cast<double>(play->sceneNum) : -1.0);
        return 1;
    }
    lua_pushnil(state);
    return 1;
}

// hook.oot.item.give (transform). Chamado do funil GiveItemEntryFromActor
// (z_actor.c) logo antes do item ser entregue ao Player. O Lua recebe o item
// atual e pode devolver, via ship.hooks.result, um NOVO get_item_id — o host
// reconstrói o GetItemEntry pela tabela vanilla (MOD_NONE) e substitui in
// place. É o ponto que um randomizer em Lua precisa: "esta check dá o quê?".
// Ponto frio (só ao pegar item), mesma disciplina do bridge de ledge-grab.
extern "C" void ShipLua_TransformGivenItem(GetItemEntry* entry) {
    if (entry == nullptr || gModHost == nullptr) {
        return;
    }
    const auto result = DispatchHookTransform(
        "hook.oot.item.give",
        ShipLua::EventPayload{
            {"item_id", static_cast<std::int64_t>(entry->itemId)},
            {"get_item_id", static_cast<std::int64_t>(entry->getItemId)},
        });
    if (!result.has_value() || !std::holds_alternative<std::int64_t>(result->value)) {
        return;
    }
    const std::int64_t replacement = std::get<std::int64_t>(result->value);
    if (replacement == entry->getItemId || replacement <= 0 || replacement > 0xFFFF) {
        return;
    }
    GetItemEntry replaced =
        ItemTableManager::Instance->RetrieveItemEntry(MOD_NONE, static_cast<uint16_t>(replacement));
    if (replaced.itemId != ITEM_NONE) {
        *entry = replaced;
    }
}
std::optional<std::string> gCustomBodyOverrideAnim;
bool gCustomBodyOverrideOnce = false;
int gCustomBodyRollCharge = 0;
bool gCustomBodyRollSpikesActive = false;
int gCustomBodyRollMagicDrainTimer = 0;
int gCustomBodyRollWallBounceTimer = 0;
enum class CustomBodyRollPhase : uint8_t { None, Enter, Rolling, Exit };
CustomBodyRollPhase gCustomBodyRollPhase = CustomBodyRollPhase::None;
enum class CustomBodyWaterVoidPhase : uint8_t { None, Curl, Ball, Triggered };
CustomBodyWaterVoidPhase gCustomBodyWaterVoidPhase = CustomBodyWaterVoidPhase::None;
bool gCustomBodyWaterVoidOwnsInputDisable = false;
int gCustomBodyWaterVoidTimer = 0;
enum class CustomBodyGroundPoundPhase : uint8_t { None, Rising, Falling, Impact };
CustomBodyGroundPoundPhase gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::None;
int gCustomBodyGroundPoundTimer = 0;
// Verdadeiro somente no intervalo de dano de um soco especial. Exposto pelo
// getter para que a spec Lua possa observar o estado sem ter de inferi-lo da
// animação e de frames internos do engine.
bool gCustomBodyPunchHitActive = false;
// Igual a av2.actionVar2 do Player de MM: B durante punch A/B não reinicia a
// animação atual; apenas agenda o próximo golpe quando ela terminar.
bool gCustomBodyPunchQueued = false;
// Estado original do cylinder do Player antes da bola Goron assumir o ataque.
// O collider pertence ao Player e é reutilizado pelo engine; por isso não pode
// ficar com raio/dano de bola depois de qualquer saída de roll ou troca de cena.
bool gCustomBodyRollColliderArmed = false;
s16 gCustomBodyRollColliderRadius = 0;
s16 gCustomBodyRollColliderHeight = 0;
s16 gCustomBodyRollColliderYShift = 0;
bool gCustomBodyShieldActive = false;
// A troca de um corpo externo acontece depois de gPlayerAnim_cl_setmask. Sem
// uma transicao propria, o Player pode andar durante a animacao e a troca de
// skeleton fica exposta em um frame. OoTMM e o PR #1 congelam o Player e
// escondem esse instante sob um flash branco; mantemos a mesma semantica sem
// sequestrar camera ou iniciar uma cutscene do OoT.
enum class MaskTransitionPhase : uint8_t { None, Conceal, Reveal };
MaskTransitionPhase gMaskTransitionPhase = MaskTransitionPhase::None;
int gMaskTransitionFramesRemaining = 0;
int gMaskTransitionRevealTimeout = 0;
s16 gMaskTransitionFlashAlpha = 0;
bool gMaskTransitionOwnsInputDisable = false;
// true entre OnPlayDestroy e o próximo OnSceneInit quando o corpo estava
// ativo na cena anterior — sinaliza para religar automaticamente na cena
// nova, usando a spec já guardada em gCustomBodySpec (nunca limpa por troca
// de cena, só pelo toggle manual).
bool gCustomBodyPendingRestore = false;

void CustomBodyFreeSkelBuffers(SkelAnime& skelAnime) {
    if (skelAnime.jointTable != nullptr) {
        ZELDA_ARENA_FREE_DEBUG(skelAnime.jointTable);
        skelAnime.jointTable = nullptr;
    }
    if (skelAnime.morphTable != nullptr) {
        ZELDA_ARENA_FREE_DEBUG(skelAnime.morphTable);
        skelAnime.morphTable = nullptr;
    }
    // Sem uma postura opcional na próxima spec, o update não pode conservar
    // o ponteiro do skeleton anterior junto com as tabelas já liberadas.
    skelAnime.skeleton = nullptr;
    skelAnime.skeletonHeader = nullptr;
    skelAnime.dListCount = 0;
}

void CustomBodyFreeSkelBuffers() {
    CustomBodyFreeSkelBuffers(gCustomBodySkelAnime);
    CustomBodyFreeSkelBuffers(gCustomBodyShieldSkelAnime);
}

void CustomBodyWaterVoidReset(Player* player) {
    if (player != nullptr && gCustomBodyWaterVoidOwnsInputDisable) {
        player->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;
    }
    gCustomBodyWaterVoidPhase = CustomBodyWaterVoidPhase::None;
    gCustomBodyWaterVoidOwnsInputDisable = false;
    gCustomBodyWaterVoidTimer = 0;
}

void MaskTransitionReset(Player* player) {
    if (player != nullptr && gMaskTransitionOwnsInputDisable) {
        player->stateFlags1 &= ~PLAYER_STATE1_INPUT_DISABLED;
    }
    gMaskTransitionPhase = MaskTransitionPhase::None;
    gMaskTransitionFramesRemaining = 0;
    gMaskTransitionRevealTimeout = 0;
    gMaskTransitionFlashAlpha = 0;
    gMaskTransitionOwnsInputDisable = false;
}

void MaskTransitionStart(Player* player, int animationFrames) {
    MaskTransitionReset(player);
    gMaskTransitionPhase = MaskTransitionPhase::Conceal;
    gMaskTransitionFramesRemaining = std::max(animationFrames, 1);
    gMaskTransitionOwnsInputDisable = true;
    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    player->linearVelocity = 0.0f;
    player->actor.velocity.y = 0.0f;
}

void MaskTransitionBeginReveal(Player* player) {
    if (gMaskTransitionPhase != MaskTransitionPhase::Conceal) {
        return;
    }
    // A timer Lua troca o corpo no fim da animacao. Fixar alpha no pico neste
    // ponto garante que nenhum frame mostre Link e Goron simultaneamente.
    gMaskTransitionPhase = MaskTransitionPhase::Reveal;
    gMaskTransitionFramesRemaining = 0;
    gMaskTransitionRevealTimeout = 0;
    gMaskTransitionFlashAlpha = 255;
    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
}

void MaskTransitionUpdate(Player* player) {
    if (gMaskTransitionPhase == MaskTransitionPhase::None) {
        return;
    }

    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    player->linearVelocity = 0.0f;
    player->actor.velocity.y = 0.0f;

    if (gMaskTransitionPhase == MaskTransitionPhase::Conceal) {
        constexpr int kFlashBuildFrames = 6;
        if (gMaskTransitionFramesRemaining > 0) {
            --gMaskTransitionFramesRemaining;
        }
        if (gMaskTransitionFramesRemaining <= kFlashBuildFrames) {
            const int builtFrames = kFlashBuildFrames - gMaskTransitionFramesRemaining + 1;
            gMaskTransitionFlashAlpha = static_cast<s16>(std::clamp(builtFrames * 255 / kFlashBuildFrames, 0, 255));
        }
        if (gMaskTransitionFramesRemaining == 0) {
            // Uma falha no callback Lua nao pode deixar o jogador sem input.
            // O body normalmente e ativado no mesmo frame; esta margem so e
            // usada como escape seguro se o mod for descarregado no meio.
            if (++gMaskTransitionRevealTimeout >= 15) {
                MaskTransitionReset(player);
            }
        }
        return;
    }

    gMaskTransitionFlashAlpha = std::max<s16>(0, gMaskTransitionFlashAlpha - 20);
    if (gMaskTransitionFlashAlpha == 0) {
        MaskTransitionReset(player);
    }
}

void CustomBodyClearRollHitbox(Player* player, PlayState* play);

// Desliga o corpo e destrói o ator hospedeiro se ainda existir. clearSpec
// distingue o toggle manual (limpa tudo, inclusive a spec guardada) da
// limpeza interna de troca de cena (mantém a spec para religar sozinho).
void CustomBodyDeactivate(PlayState* play, bool clearSpec) {
    if (gCustomBodyActive) {
        Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
        if (player != nullptr) {
            CustomBodyClearRollHitbox(player, play);
            player->stateFlags2 &= ~PLAYER_STATE2_DISABLE_DRAW;
            CustomBodyWaterVoidReset(player);
        }
        if (gCustomBodyActor != nullptr && play != nullptr) {
            Actor_Kill(gCustomBodyActor);
        }
    }
    gCustomBodyActive = false;
    gCustomBodyActor = nullptr;
    CustomBodyFreeSkelBuffers();
    gCustomBodyCurrentAnim.clear();
    gCustomBodyOverrideAnim.reset();
    gCustomBodyOverrideOnce = false;
    gCustomBodyRollCharge = 0;
    gCustomBodyRollSpikesActive = false;
    gCustomBodyRollMagicDrainTimer = 0;
    gCustomBodyRollWallBounceTimer = 0;
    gCustomBodyRollPhase = CustomBodyRollPhase::None;
    CustomBodyWaterVoidReset(nullptr);
    gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::None;
    gCustomBodyGroundPoundTimer = 0;
    gCustomBodyPunchHitActive = false;
    gCustomBodyPunchQueued = false;
    gCustomBodyRollColliderArmed = false;
    gCustomBodyShieldActive = false;
    if (clearSpec) {
        gCustomBodySpec = CustomBodySpec{};
        gCustomBodyPendingRestore = false;
    }
}

// Chamado em OnPlayDestroy: a troca de cena já descartou (ou está
// descartando) a arena Zelda de onde jointTable/morphTable foram alocados —
// chamar ZELDA_ARENA_FREE_DEBUG aqui seria double-free num ponteiro pendente
// (o mesmo bug que o fork de referência documentou e corrigiu). Só zera os
// ponteiros; o ator hospedeiro já foi destruído pela própria troca de cena.
// Marca pending-restore para OnSceneInit religar sozinho na cena nova.
void CustomBodyResetForSceneChange() {
    // A cena nova cria câmeras próprias; segurar o id da subcâmera antiga
    // deixaria o jogador sem controle de câmera. Só descarta o estado — não
    // chama CutsceneStop, que mexeria em ponteiros já em teardown.
    gCutsceneActive = false;
    gCutsceneCamId = 0;
    gCutsceneElapsed = 0;
    // O Player antigo já está em teardown; só descartamos o estado do flash.
    // A nova cena cria um Player limpo, portanto não há flag de input a limpar.
    MaskTransitionReset(nullptr);
    gCustomBodyPendingRestore = gCustomBodyActive;
    gCustomBodyActive = false;
    gCustomBodyActor = nullptr;
    gCustomBodySkelAnime.jointTable = nullptr;
    gCustomBodySkelAnime.morphTable = nullptr;
    gCustomBodyShieldSkelAnime.jointTable = nullptr;
    gCustomBodyShieldSkelAnime.morphTable = nullptr;
    gCustomBodyShieldSkelAnime.skeleton = nullptr;
    gCustomBodyShieldSkelAnime.skeletonHeader = nullptr;
    gCustomBodyShieldSkelAnime.dListCount = 0;
    gCustomBodyCurrentAnim.clear();
    gCustomBodyOverrideAnim.reset();
    gCustomBodyOverrideOnce = false;
    gCustomBodyRollCharge = 0;
    gCustomBodyRollSpikesActive = false;
    gCustomBodyRollMagicDrainTimer = 0;
    gCustomBodyRollWallBounceTimer = 0;
    gCustomBodyRollPhase = CustomBodyRollPhase::None;
    CustomBodyWaterVoidReset(nullptr);
    gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::None;
    gCustomBodyGroundPoundTimer = 0;
    gCustomBodyPunchHitActive = false;
    gCustomBodyPunchQueued = false;
    gCustomBodyRollColliderArmed = false;
    gCustomBodyShieldActive = false;
}

const char* CustomBodyRollPhaseName() {
    switch (gCustomBodyRollPhase) {
        case CustomBodyRollPhase::Enter:
            return "enter";
        case CustomBodyRollPhase::Rolling:
            return "rolling";
        case CustomBodyRollPhase::Exit:
            return "exit";
        default:
            return "none";
    }
}

const char* CustomBodyGroundPoundPhaseName() {
    switch (gCustomBodyGroundPoundPhase) {
        case CustomBodyGroundPoundPhase::Rising:
            return "ground_pound_rise";
        case CustomBodyGroundPoundPhase::Falling:
            return "ground_pound_fall";
        case CustomBodyGroundPoundPhase::Impact:
            return "ground_pound_impact";
        default:
            return nullptr;
    }
}

const char* CustomBodyCurrentRollPhaseName() {
    const char* groundPound = CustomBodyGroundPoundPhaseName();
    return groundPound != nullptr ? groundPound : CustomBodyRollPhaseName();
}

const char* CustomBodyWaterVoidPhaseName() {
    switch (gCustomBodyWaterVoidPhase) {
        case CustomBodyWaterVoidPhase::Curl:
            return "curl";
        case CustomBodyWaterVoidPhase::Ball:
            return "ball";
        case CustomBodyWaterVoidPhase::Triggered:
            return "triggered";
        default:
            return "none";
    }
}

// Player_Action_96 de MM reflete a bola em paredes quando ela já alcançou
// velocidade de ataque. O roll vanilla de OoT tem escala menor (e pode acabar
// antes do callback de ator), portanto o hook OnPlayerBonk chama este helper
// no mesmo frame. AT_HIT é preservado para dyna/objetos quebráveis receberem
// seu dano antes de qualquer reflexão.
void CustomBodyHandleRollWallBounce(Player* player, PlayState* play) {
    constexpr f32 kMinimumBounceSpeed = 6.0f;
    constexpr int kBounceCooldownFrames = 4;
    if (!gCustomBodyActive || !gChainRoll || player == nullptr || play == nullptr ||
        gCustomBodyRollPhase != CustomBodyRollPhase::Rolling || gCustomBodyRollWallBounceTimer > 0 ||
        fabsf(player->linearVelocity) < kMinimumBounceSpeed || (player->actor.bgCheckFlags & BGCHECKFLAG_WALL) == 0 ||
        (gCustomBodyRollColliderArmed && (player->cylinder.base.atFlags & AT_HIT) != 0)) {
        return;
    }

    // Mesma reflexão usada pelo enhancement BounceOffWalls do host e
    // equivalente à rotação por wallYaw do Player_Action_96.
    player->yaw = ((player->actor.wallYaw - player->yaw) + player->actor.wallYaw) - 0x8000;
    player->actor.shape.rot.y = player->yaw;
    player->actor.world.rot.y = player->yaw;
    player->linearVelocity = std::max(kMinimumBounceSpeed, fabsf(player->linearVelocity) * 0.85f);
    gCustomBodyRollWallBounceTimer = kBounceCooldownFrames;
    Player_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
    Player_SetupRoll(player, play);
}

bool CustomBodyStartAnimation(PlayState* play, const std::string& name, f32 speed, bool once, bool reverse = false) {
    const auto path = gCustomBodySpec.animsPrefixed.find(name);
    if (play == nullptr || path == gCustomBodySpec.animsPrefixed.end()) {
        return false;
    }
    auto* loaded = ResourceMgr_LoadAnimByName(path->second.c_str());
    if (loaded == nullptr) {
        return false;
    }
    const f32 lastFrame = Animation_GetLastFrame(reinterpret_cast<LinkAnimationHeader*>(loaded));
    const f32 startFrame = reverse ? lastFrame : 0.0f;
    const f32 endFrame = reverse ? 0.0f : lastFrame;
    LinkAnimation_Change(play, &gCustomBodySkelAnime,
                          reinterpret_cast<LinkAnimationHeader*>(const_cast<char*>(path->second.c_str())),
                          reverse ? -speed : speed, startFrame, endFrame, once ? ANIMMODE_ONCE : ANIMMODE_LOOP, 0.0f);
    if (name == "punch_a" || name == "punch_b" || name == "punch_c") {
        // O primeiro frame apenas captura a origem: nao pode herdar prevTransl
        // de idle, caminhada ou de outro golpe.
        gCustomBodySkelAnime.movementFlags |= ANIM_FLAG_NOMOVE;
    }
    gCustomBodyCurrentAnim = name;
    gCustomBodyOverrideAnim = name;
    gCustomBodyOverrideOnce = once;
    return true;
}

// O Player de MM não permite que Goron nade. O fork de referência faz a
// sequência pg_maru_change -> bola afundando -> Play_TriggerVoidOut quando a
// profundidade passa de 20. Mantemos essa regra como opt-in da spec para que
// qualquer outro corpo externo continue usando a água normal do OoT.
void CustomBodyUpdateWaterVoid(Player* player, PlayState* play) {
    if (!gCustomBodySpec.waterVoid || player == nullptr || play == nullptr ||
        gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Triggered) {
        return;
    }
    if (gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::None) {
        if (player->actor.yDistToWater <= 20.0f || (player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) != 0 ||
            player->getItemId != GI_NONE) {
            return;
        }
        gCustomBodyWaterVoidPhase = CustomBodyWaterVoidPhase::Curl;
        gCustomBodyWaterVoidOwnsInputDisable = true;
        gCustomBodyWaterVoidTimer = 0;
        player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
        player->linearVelocity = 0.0f;
        player->actor.velocity.y = -2.0f;
        player->actor.gravity = -0.5f;
        CustomBodyClearRollHitbox(player, play);
        CustomBodyStartAnimation(play, "roll_enter", 0.67f, true);
        Player_PlaySfx(&player->actor, NA_SE_EV_DIVE_INTO_WATER);
        Player_PlaySfx(&player->actor, NA_SE_PL_BODY_HIT);
        return;
    }

    player->stateFlags1 |= PLAYER_STATE1_INPUT_DISABLED;
    player->linearVelocity = 0.0f;
    if (gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Curl) {
        player->actor.velocity.y = -2.0f;
        player->actor.gravity = -0.5f;
        const auto curl = gCustomBodySpec.animsPrefixed.find("roll_enter");
        auto* loaded = curl == gCustomBodySpec.animsPrefixed.end() ? nullptr : ResourceMgr_LoadAnimByName(curl->second.c_str());
        if (loaded == nullptr || gCustomBodySkelAnime.curFrame >=
                                  Animation_GetLastFrame(reinterpret_cast<LinkAnimationHeader*>(loaded)) - 0.5f) {
            gCustomBodyWaterVoidPhase = CustomBodyWaterVoidPhase::Ball;
            gCustomBodyOverrideAnim.reset();
            gCustomBodyOverrideOnce = false;
            player->actor.gravity = -2.0f;
        }
        return;
    }

    // A bola não gira: ela só afunda por aproximadamente 15 frames antes da
    // transição. Isso replica a separação de fases do Player de MM.
    player->actor.velocity.y = -2.0f;
    player->actor.gravity = -2.0f;
    if (++gCustomBodyWaterVoidTimer >= 15) {
        gCustomBodyWaterVoidPhase = CustomBodyWaterVoidPhase::Triggered;
        Player_PlaySfx(&player->actor, NA_SE_OC_ABYSS);
        Play_TriggerVoidOut(play);
    }
}

// Perfil compacto e fechado para os socos de Goron. A forma usa os quads de
// ataque que o Player do OoT já possui; assim não há collider alocado por Lua
// nem parâmetros de dano arbitrários vindos de mod. Os nomes são convenções da
// spec, e só ganham colisão quando a animação correspondente foi declarada.
struct CustomBodyPunchInfo {
    const char* animation;
    f32 hitStart;
    f32 hitEnd;
    f32 nearDist;
    f32 farDist;
    f32 sideOffset;
    f32 halfWidth;
    f32 yBottom;
    f32 yTop;
};

constexpr std::array<CustomBodyPunchInfo, 3> kCustomBodyGoronPunches = {{
    // MM sMeleeAttackAnimInfo: punch A {6, 8}, punch B {12, 18}, punch C {8, 14}.
    { "punch_a", 6.0f, 8.0f, 20.0f, 55.0f, -15.0f, 15.0f, 20.0f, 55.0f },
    { "punch_b", 12.0f, 18.0f, 20.0f, 55.0f, 15.0f, 15.0f, 20.0f, 55.0f },
    { "punch_c", 8.0f, 14.0f, -10.0f, 30.0f, 0.0f, 30.0f, 5.0f, 30.0f },
}};

const CustomBodyPunchInfo* CustomBodyGetPunchInfo() {
    for (const auto& punch : kCustomBodyGoronPunches) {
        if (gCustomBodyCurrentAnim == punch.animation &&
            gCustomBodySpec.animsPrefixed.find(punch.animation) != gCustomBodySpec.animsPrefixed.end()) {
            return &punch;
        }
    }
    return nullptr;
}

const char* CustomBodyPunchRecoveryAnimation(const char* punchName, bool moving) {
    if (std::strcmp(punchName, "punch_a") == 0) {
        return moving ? "punch_a_end_run" : "punch_a_end";
    }
    if (std::strcmp(punchName, "punch_b") == 0) {
        return moving ? "punch_b_end_run" : "punch_b_end";
    }
    if (std::strcmp(punchName, "punch_c") == 0) {
        return moving ? "punch_c_end_run" : "punch_c_end";
    }
    return nullptr;
}

bool CustomBodyStartPunchRecovery(PlayState* play, const char* punchName, const Player* player) {
    const char* preferred = CustomBodyPunchRecoveryAnimation(punchName, fabsf(player->linearVelocity) >= 0.5f);
    if (preferred != nullptr && CustomBodyStartAnimation(play, preferred, 1.0f, true)) {
        return true;
    }
    // Um mod pode fornecer somente a recuperação parada; ela é melhor que
    // cortar o punch diretamente para idle quando o Player ainda se move.
    if (preferred != nullptr && std::strstr(preferred, "_run") != nullptr) {
        std::string fallback(preferred);
        fallback.erase(fallback.size() - 4);
        return CustomBodyStartAnimation(play, fallback, 1.0f, true);
    }
    return false;
}

const char* CustomBodyNextPunchAnimation(const char* punchName) {
    if (std::strcmp(punchName, "punch_a") == 0) {
        return "punch_b";
    }
    if (std::strcmp(punchName, "punch_b") == 0) {
        return "punch_c";
    }
    return nullptr;
}

bool CustomBodyStartQueuedPunch(PlayState* play, const char* punchName) {
    if (!gCustomBodyPunchQueued) {
        return false;
    }
    gCustomBodyPunchQueued = false;
    const char* nextPunch = CustomBodyNextPunchAnimation(punchName);
    return nextPunch != nullptr && CustomBodyStartAnimation(play, nextPunch, 1.0f, true);
}

// Os punches de Goron em MM usam a translacao da junta raiz como movimento
// real. LinkAnimation_Update deixa essa translacao em jointTable[0]; o helper
// nativo abaixo extrai o delta, restaura a junta para baseTransl (para a malha
// nao se afastar do ator) e conserva prevTransl para o proximo frame. Aplicar
// somente no perfil fechado dos tres punches impede que uma animacao arbitraria
// declarada por Lua vire uma forma de mover o Player.
void CustomBodyApplyPunchRootMotion(Actor* hostActor, Player* player) {
    if (CustomBodyGetPunchInfo() == nullptr) {
        return;
    }

    Vec3f delta{};
    SkelAnime_UpdateTranslation(&gCustomBodySkelAnime, &delta, player->actor.shape.rot.y);
    player->actor.world.pos.x += delta.x * player->actor.scale.x;
    player->actor.world.pos.z += delta.z * player->actor.scale.z;
    // O ator hospedeiro atualiza depois do Player. Sincroniza-o tambem neste
    // frame para que o corpo e a colisao nao aparentem ficar defasados.
    hostActor->world.pos = player->actor.world.pos;
}

void CustomBodySetPunchQuadVertices(Player* player, const CustomBodyPunchInfo& punch) {
    const f32 sinYaw = Math_SinS(player->yaw);
    const f32 cosYaw = Math_CosS(player->yaw);
    const f32 rightX = cosYaw;
    const f32 rightZ = -sinYaw;
    const Vec3f& pos = player->actor.world.pos;

    const f32 farX = pos.x + sinYaw * punch.farDist + rightX * punch.sideOffset;
    const f32 farZ = pos.z + cosYaw * punch.farDist + rightZ * punch.sideOffset;
    const f32 nearX = pos.x + sinYaw * punch.nearDist + rightX * punch.sideOffset;
    const f32 nearZ = pos.z + cosYaw * punch.nearDist + rightZ * punch.sideOffset;
    Vec3f a = { farX - rightX * punch.halfWidth, pos.y + punch.yTop, farZ - rightZ * punch.halfWidth };
    Vec3f b = { farX + rightX * punch.halfWidth, pos.y + punch.yTop, farZ + rightZ * punch.halfWidth };
    Vec3f c = { nearX + rightX * punch.halfWidth, pos.y + punch.yBottom, nearZ + rightZ * punch.halfWidth };
    Vec3f d = { nearX - rightX * punch.halfWidth, pos.y + punch.yBottom, nearZ - rightZ * punch.halfWidth };
    Collider_SetQuadVertices(&player->meleeWeaponQuads[0], &a, &b, &c, &d);
}

// Player_Update já produziu os quads da espada antes do ator hospedeiro
// atualizar (ACTORCAT_PLAYER vem antes de ACTORCAT_ITEMACTION). Enquanto a
// animação Goron é a dona visual do golpe, elimina ambos para a espada
// invisível não acertar junto do punho.
void CustomBodyUpdatePunchHitbox(Player* player, PlayState* play) {
    gCustomBodyPunchHitActive = false;
    const CustomBodyPunchInfo* punch = CustomBodyGetPunchInfo();
    if (punch == nullptr) {
        return;
    }

    ColliderQuad* primary = &player->meleeWeaponQuads[0];
    ColliderQuad* secondary = &player->meleeWeaponQuads[1];
    Collider_ResetQuadAT(play, &primary->base);
    Collider_ResetQuadAT(play, &secondary->base);
    primary->info.toucher.dmgFlags = 0;
    secondary->info.toucher.dmgFlags = 0;

    const f32 frame = gCustomBodySkelAnime.curFrame;
    if (frame < punch->hitStart || frame > punch->hitEnd) {
        return;
    }

    CustomBodySetPunchQuadVertices(player, *punch);
    primary->base.atFlags = AT_ON | AT_TYPE_PLAYER;
    // Equivalente OoT de DMG_GORON_PUNCH: impacto pesado, não um corte da
    // espada oculta. O valor 2 segue a força transformada do MM e é fechado
    // no host para não virar uma API de dano irrestrito para Lua.
    primary->info.toucher.dmgFlags = DMG_HAMMER_SWING;
    primary->info.toucher.damage = 2;
    primary->info.toucherFlags = TOUCH_ON | TOUCH_NEAREST;
    CollisionCheck_SetAT(play, &play->colChkCtx, &primary->base);
    gCustomBodyPunchHitActive = true;
}

void CustomBodyClearRollHitbox(Player* player, PlayState* play) {
    if (!gCustomBodyRollColliderArmed) {
        return;
    }
    Collider_ResetCylinderAT(play, &player->cylinder.base);
    player->cylinder.base.atFlags = AT_NONE;
    player->cylinder.info.toucherFlags = TOUCH_NONE;
    player->cylinder.info.toucher.dmgFlags = 0;
    player->cylinder.info.toucher.damage = 0;
    player->cylinder.dim.radius = gCustomBodyRollColliderRadius;
    player->cylinder.dim.height = gCustomBodyRollColliderHeight;
    player->cylinder.dim.yShift = gCustomBodyRollColliderYShift;
    Collider_UpdateCylinder(&player->actor, &player->cylinder);
    gCustomBodyRollColliderArmed = false;
}

// MM Player_Action_96 arma o cylinder do próprio Player para a bola (raio 25,
// dano 1). O equivalente OoT seguro do impacto é DMG_HAMMER_SWING. Só usamos
// este caminho depois da transição curl e acima da velocidade mínima, para que
// a animação de entrada/saída nunca cause dano invisível.
void CustomBodyUpdateRollHitbox(Player* player, PlayState* play, bool rolling) {
    const bool groundPoundImpact = gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::Impact;
    const bool attackActive = groundPoundImpact || (rolling && fabsf(player->linearVelocity) > 2.0f);
    if (!attackActive) {
        CustomBodyClearRollHitbox(player, play);
        return;
    }

    if (!gCustomBodyRollColliderArmed) {
        gCustomBodyRollColliderArmed = true;
        gCustomBodyRollColliderRadius = player->cylinder.dim.radius;
        gCustomBodyRollColliderHeight = player->cylinder.dim.height;
        gCustomBodyRollColliderYShift = player->cylinder.dim.yShift;
    }
    Collider_ResetCylinderAT(play, &player->cylinder.base);
    player->cylinder.base.atFlags = AT_ON | AT_TYPE_PLAYER;
    player->cylinder.info.elemType = ELEMTYPE_UNK2;
    player->cylinder.info.toucherFlags = TOUCH_ON | TOUCH_NEAREST | TOUCH_SFX_NORMAL;
    player->cylinder.info.toucher.dmgFlags = DMG_HAMMER_SWING;
    player->cylinder.info.toucher.damage = groundPoundImpact ? 4 : 1;
    // 25 é o raio canônico de Player_SetCylinderForAttack durante a esfera
    // normal e durante os espinhos. O impacto usa 60, como DMG_GORON_POUND.
    player->cylinder.dim.radius = groundPoundImpact ? 60 : 25;
    player->cylinder.dim.height = 34;
    player->cylinder.dim.yShift = 0;
    Collider_UpdateCylinder(&player->actor, &player->cylinder);
    CollisionCheck_SetAT(play, &play->colChkCtx, &player->cylinder.base);
}

// O MM não tem um "asset de ground pound": é uma fase física dentro de
// Player_Action_96. O host conserva Player_Action_Roll para o pipeline de
// movimento do OoT e injeta somente o arco vertical/impacto no frame posterior
// ao update vanilla. Assim não substituímos Player_Update nem o bg check.
void CustomBodyUpdateGroundPound(Player* player, PlayState* play, bool rolling) {
    if (gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::None) {
        if (rolling && !gCustomBodyRollSpikesActive && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
            gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::Rising;
            gCustomBodyGroundPoundTimer = 0;
            gCustomBodyRollCharge = 0;
            Player_SetupRoll(player, play);
            player->linearVelocity = 0.0f;
            player->actor.velocity.y = 14.0f;
            player->actor.gravity = -1.2f;
        }
        return;
    }

    // O roll vanilla termina sua animação enquanto o Goron ainda está no ar;
    // rearmá-lo impede que Idle assuma antes de a bola tocar o solo. O skeleton
    // humano continua oculto e a renderização externa permanece a bola.
    if (player->actionFunc != Player_Action_Roll) {
        Player_SetupRoll(player, play);
    }
    player->linearVelocity = 0.0f;

    if (gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::Rising) {
        if (player->actor.velocity.y <= 0.0f) {
            gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::Falling;
        }
    } else if (gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::Falling) {
        if (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) {
            gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::Impact;
            gCustomBodyGroundPoundTimer = 6;
            player->actor.velocity.y = 0.0f;
            Actor_RequestQuake(play, 3, 12);
            Rumble_Request(0.0f, 180, 12, 100);
        }
    } else {
        player->actor.velocity.y = 0.0f;
        if (--gCustomBodyGroundPoundTimer <= 0) {
            gCustomBodyGroundPoundPhase = CustomBodyGroundPoundPhase::None;
        }
    }
}

// MM ativa os espinhos com custo inicial 2 e, enquanto estão ativos, drena 1
// unidade a cada 10 frames. O save de OoT expõe o mesmo medidor; alterar o
// valor somente em MAGIC_STATE_IDLE evita disputar uma poção/flecha mágica ou
// outra transição de HUD em curso.
void CustomBodyUpdateRollSpikes(PlayState* play, bool rolling) {
    if (!rolling || gCustomBodyGroundPoundPhase != CustomBodyGroundPoundPhase::None) {
        gCustomBodyRollSpikesActive = false;
        gCustomBodyRollMagicDrainTimer = 0;
        if (!rolling) {
            gCustomBodyRollCharge = std::max(gCustomBodyRollCharge - 2, 0);
        }
        return;
    }
    const bool holdingA = CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_A);
    if (gCustomBodyRollSpikesActive) {
        if (!holdingA || gSaveContext.magic <= 0) {
            gCustomBodyRollSpikesActive = false;
            gCustomBodyRollMagicDrainTimer = 0;
            gCustomBodyRollCharge = 4;
            return;
        }
        if (gSaveContext.magicState == MAGIC_STATE_IDLE && --gCustomBodyRollMagicDrainTimer <= 0) {
            gSaveContext.magic = std::max(0, static_cast<int>(gSaveContext.magic) - 1);
            gCustomBodyRollMagicDrainTimer = 10;
            if (gSaveContext.magic == 0) {
                gCustomBodyRollSpikesActive = false;
                gCustomBodyRollCharge = 4;
            }
        }
        return;
    }
    if (holdingA) {
        gCustomBodyRollCharge = std::min(gCustomBodyRollCharge + 1, 60);
    } else {
        gCustomBodyRollCharge = std::max(gCustomBodyRollCharge - 2, 0);
    }
    if (gCustomBodyRollCharge >= 60) {
        if (gSaveContext.magicState == MAGIC_STATE_IDLE && gSaveContext.magic >= 2) {
            gSaveContext.magic -= 2;
            gCustomBodyRollSpikesActive = true;
            gCustomBodyRollMagicDrainTimer = 10;
        } else {
            // Mantém a pré-carga visível, mas nunca ativa o estado de espinho
            // sem o custo de magia que o MM exige.
            gCustomBodyRollCharge = 59;
        }
    }
}

// Núcleo comum entre o toggle manual e a auto-restauração pós-troca-de-cena:
// aloca o skelAnime a partir de gCustomBodySpec e spawna o ator hospedeiro.
// Não mexe em gCustomBodySpec — quem chama já garantiu que está preenchida.
// A custom body hides Link's shield limb, so the native Player draw path never
// refreshes shieldQuad. Reuse that quad here to preserve native block and
// projectile reflection behavior without creating a second broad collider.
void CustomBodyUpdateShieldCollider(Player* player, PlayState* play) {
    const s16 yaw = player->actor.shape.rot.y;
    SkinMatrix_SetTranslateRotateYXZScale(&player->shieldMf, 1.0f, 1.0f, 1.0f, 0, yaw + 0x8000, 0, 0.0f, 0.0f,
                                          0.0f);

    const f32 sinYaw = Math_SinS(yaw);
    const f32 cosYaw = Math_CosS(yaw);
    const f32 rightX = cosYaw;
    const f32 rightZ = -sinYaw;
    constexpr f32 frontDistance = 12.0f;
    constexpr f32 halfWidth = 25.0f;
    constexpr f32 bottomY = -10.0f;
    constexpr f32 topY = 75.0f;
    const Vec3f& position = player->actor.world.pos;
    const f32 centerX = position.x + sinYaw * frontDistance;
    const f32 centerZ = position.z + cosYaw * frontDistance;

    Vec3f a = { centerX - rightX * halfWidth, position.y + topY, centerZ - rightZ * halfWidth };
    Vec3f b = { centerX + rightX * halfWidth, position.y + topY, centerZ + rightZ * halfWidth };
    Vec3f c = { centerX + rightX * halfWidth, position.y + bottomY, centerZ + rightZ * halfWidth };
    Vec3f d = { centerX - rightX * halfWidth, position.y + bottomY, centerZ - rightZ * halfWidth };

    // The Deku shield is wood; every other selection, including none, is metal.
    player->shieldQuad.base.colType = player->currentShield == PLAYER_SHIELD_DEKU ? COLTYPE_WOOD : COLTYPE_METAL;
    Collider_ResetQuadAC(play, &player->shieldQuad.base);
    Collider_ResetQuadAT(play, &player->shieldQuad.base);
    Collider_SetQuadVertices(&player->shieldQuad, &a, &b, &c, &d);
    CollisionCheck_SetAC(play, &play->colChkCtx, &player->shieldQuad.base);
    CollisionCheck_SetAT(play, &play->colChkCtx, &player->shieldQuad.base);
}

bool CustomBodyActivateFromSpec(PlayState* play);

extern "C" void CustomBodyActorUpdate(Actor* actor, PlayState* play) {
    if (!gCustomBodyActive) {
        return;
    }
    Player* player = GET_PLAYER(play);
    if (player == nullptr) {
        CustomBodyDeactivate(play, false);
        return;
    }
    // Reafirma todo frame: certas transições de ação do próprio vanilla
    // reescrevem stateFlags2 inteiro (não fazem só |=/&= de bits pontuais),
    // o que apaga a flag se ela só for setada uma vez no toggle — sintoma
    // visto em jogo: braço de manga verde do Link vazando atrás da cabeça
    // custom. Setar aqui garante que ela nunca fica limpa por mais de 1 frame.
    player->stateFlags2 |= PLAYER_STATE2_DISABLE_DRAW;
    actor->world.pos = player->actor.world.pos;
    actor->shape.rot = player->actor.shape.rot;
    if (gCustomBodyRollWallBounceTimer > 0) {
        --gCustomBodyRollWallBounceTimer;
    }
    const bool nativeRolling = player->actionFunc == Player_Action_Roll &&
                               gCustomBodySpec.modelsPrefixed.find("roll") != gCustomBodySpec.modelsPrefixed.end();
    CustomBodyUpdateWaterVoid(player, play);
    const bool waterVoidActive = gCustomBodyWaterVoidPhase != CustomBodyWaterVoidPhase::None;
    if (!waterVoidActive && nativeRolling && gCustomBodyRollPhase == CustomBodyRollPhase::None) {
        if (CustomBodyStartAnimation(play, "roll_enter", 0.67f, true)) {
            gCustomBodyRollPhase = CustomBodyRollPhase::Enter;
        } else {
            gCustomBodyRollPhase = CustomBodyRollPhase::Rolling;
        }
    }
    if (!waterVoidActive && !nativeRolling && gCustomBodyRollPhase == CustomBodyRollPhase::Rolling &&
        gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::None) {
        if (CustomBodyStartAnimation(play, "roll_exit", 0.67f, true, true)) {
            gCustomBodyRollPhase = CustomBodyRollPhase::Exit;
        } else {
            gCustomBodyRollPhase = CustomBodyRollPhase::None;
        }
    }
    bool rolling = (!waterVoidActive && nativeRolling && gCustomBodyRollPhase == CustomBodyRollPhase::Rolling) ||
                   gCustomBodyGroundPoundPhase != CustomBodyGroundPoundPhase::None ||
                   gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Ball;
    if (!waterVoidActive) {
        CustomBodyUpdateGroundPound(player, play, rolling);
    }
    rolling = (!waterVoidActive && nativeRolling && gCustomBodyRollPhase == CustomBodyRollPhase::Rolling) ||
              gCustomBodyGroundPoundPhase != CustomBodyGroundPoundPhase::None ||
              gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Ball;
    CustomBodyUpdateRollSpikes(play, rolling && !waterVoidActive);
    // R still uses the native Player input path. A custom shield posture must
    // explicitly restore the Player shield quad because Link's hand draw is
    // hidden while the external body is active.
    const bool shieldWasActive = gCustomBodyShieldActive;
    gCustomBodyShieldActive = !rolling && gCustomBodyGroundPoundPhase == CustomBodyGroundPoundPhase::None &&
                               gCustomBodyShieldSkelAnime.skeleton != nullptr &&
                               CHECK_BTN_ALL(play->state.input[0].cur.button, BTN_R);
    if (gCustomBodyShieldActive) {
        player->stateFlags1 |= PLAYER_STATE1_SHIELDING;
        CustomBodyUpdateShieldCollider(player, play);
        SkelAnime_Update(&gCustomBodyShieldSkelAnime);
    } else if (shieldWasActive) {
        player->stateFlags1 &= ~PLAYER_STATE1_SHIELDING;
    }
    // O draw nativo do MM posiciona o centro da esfera 12 unidades acima do
    // chão e a amplia 15%. Actor_Draw aplica yOffset * scale, portanto o
    // offset abaixo preserva exatamente esses 12 com a escala 0.0115.
    Actor_SetScale(actor, rolling ? 0.0115f : 0.01f);
    // O ator Player do OoT tem sua origem de colisão abaixo do ponto onde o
    // root do skeleton Goron de MM pousa. Sem compensação, o corpo ereto fica
    // visivelmente ~30 unidades acima do solo, embora a física do Player já
    // esteja em contato com ele. Actor_Draw multiplica yOffset pela escala,
    // portanto -3000.0f em 0.01 baixa o visual exatamente 30 unidades. O
    // modelo enrolado é uma DL separada e preserva o offset positivo do draw
    // nativo de MM (centro da esfera 12 unidades acima do chão).
    const f32 worldOffset = rolling ? gCustomBodySpec.rollOffset : gCustomBodySpec.groundOffset;
    actor->shape.yOffset = worldOffset / actor->scale.y;
    // Actor_Draw já aplica shape.rot antes de chamar este draw. Durante o
    // rolamento usamos esse transform para fazer a display list enrolada girar,
    // sem depender das macros de matriz que só existem no caminho C do engine.
    if (rolling && gCustomBodyWaterVoidPhase != CustomBodyWaterVoidPhase::Ball) {
        actor->shape.rot.x = player->actor.shape.rot.x + static_cast<s16>(play->gameplayFrames * -0x1200);
    } else if (gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Ball) {
        actor->shape.rot.x = 0;
    }

    std::string wanted;
    const f32 speed = fabsf(player->linearVelocity);
    const bool climbing = (player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) != 0;
    const s16 climbInput = play->state.input[0].rel.stick_y;
    const char* climbDirection = !climbing ? "none" : climbInput > 0 ? "up" : climbInput < 0 ? "down" : "idle";
    // Porta com macaneta: este actionFunc e instalado somente depois de A ter
    // confirmado a abertura. Portas de correr/falsas continuam inteiramente no
    // caminho nativo, pois nao usam a animacao de empurrar/puxar do Goron.
    const bool handleDoorOpening = player->actionFunc == Player_Action_80845EF8 && player->doorActor != nullptr &&
                                   player->doorType == PLAYER_DOORTYPE_HANDLE;
    // O fluxo de baú marca GETTING_ITEM antes de disparar a animacao da caixa.
    // O actor-alvo diferencia esse caso de qualquer outro get-item/cutscene.
    const bool chestOpening = (player->stateFlags1 & PLAYER_STATE1_GETTING_ITEM) != 0 &&
                              player->interactRangeActor != nullptr && player->interactRangeActor->id == ACTOR_EN_BOX;
    if (gCustomBodyOverrideAnim.has_value()) {
        wanted = *gCustomBodyOverrideAnim;
    } else {
        const auto result = DispatchHookTransform(
            "hook.oot.player.body_anim_select",
            ShipLua::EventPayload{
                {"speed", static_cast<double>(speed)},
                {"on_ground", (player->actor.bgCheckFlags & 1) != 0},
                {"rolling", rolling},
                {"roll_charge", static_cast<std::int64_t>(gCustomBodyRollCharge)},
                {"roll_phase", std::string(CustomBodyCurrentRollPhaseName())},
                {"falling", (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) == 0 && player->actor.velocity.y < 0.0f},
                {"landing", (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND_TOUCH) != 0},
                {"climbing", climbing},
                {"climb_direction", std::string(climbDirection)},
                // O Player alterna av2.actionVar2 ao trocar o pé da escada.
                // O bit baixo se mantém estável inclusive nos valores negativos
                // de entrada (-2/-4), sem expor ponteiros ou ação interna ao Lua.
                {"climb_step", static_cast<std::int64_t>(player->av2.actionVar2 & 1)},
                {"climb_starting", climbing && player->av2.actionVar2 < 0},
                {"door_opening", handleDoorOpening},
                {"door_direction", std::string(!handleDoorOpening ? "none"
                                                                     : player->doorDirection < 0 ? "left" : "right")},
                {"chest_opening", chestOpening},
                {"instrument", (player->stateFlags1 & PLAYER_STATE1_IN_ITEM_CS) != 0 &&
                                   (player->heldItemAction == PLAYER_IA_OCARINA_FAIRY ||
                                    player->heldItemAction == PLAYER_IA_OCARINA_OF_TIME)},
                // MM escolhe gakkiplayA/L/D/U/R por ocarinaButtonIndex. OoT
                // não preserva esse índice no contexto de mensagem, mas o
                // frame de pressão já é a mesma fonte de cada nota. A string
                // só descreve a nota atual; Lua ainda valida a animação na spec.
                {"instrument_note", std::string(CHECK_BTN_ALL(play->state.input[0].press.button, BTN_A) ? "a"
                                                   : CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CLEFT) ? "l"
                                                   : CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CDOWN) ? "d"
                                                   : CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CUP) ? "u"
                                                   : CHECK_BTN_ALL(play->state.input[0].press.button, BTN_CRIGHT) ? "r"
                                                                                                            : "none")},
                // O host não inventa uma forma Goron no Player do OoT: ele
                // apenas expõe o começo/fim do ataque nativo para que uma
                // spec de corpo possa disparar sua animação one-shot própria.
                // O id é diagnóstico/telemetria; não deve ser escrito por Lua.
                {"attacking", player->meleeWeaponState != 0},
                {"attack_animation", static_cast<std::int64_t>(player->meleeWeaponAnimation)},
            });
        if (result.has_value() && std::holds_alternative<std::string>(result->value)) {
            const std::string& requested = std::get<std::string>(result->value);
            if (gCustomBodySpec.anims.find(requested) != gCustomBodySpec.anims.end()) {
                wanted = requested;
            }
        }
    }
    if (wanted.empty() && !gCustomBodyOverrideAnim.has_value()) {
        // Fallback sem callback (ou resultado inválido): só funciona se o
        // mod usou as chaves convencionais — senão fica parado no default.
        if ((player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) == 0 &&
            gCustomBodySpec.anims.find("jump") != gCustomBodySpec.anims.end()) {
            wanted = "jump";
        } else {
            const char* byThreshold = (speed > 4.0f) ? "run" : (speed >= 0.5f ? "walk" : "idle");
            wanted = gCustomBodySpec.anims.find(byThreshold) != gCustomBodySpec.anims.end() ? byThreshold
                                                                                            : gCustomBodySpec.defaultAnim;
        }
    }

    if (wanted != gCustomBodyCurrentAnim) {
        // O ponteiro vem do membro persistente da spec, NÃO de uma temporária:
        // o engine guarda este endereço e o usa nos frames seguintes.
        const auto animPath = gCustomBodySpec.animsPrefixed.find(wanted);
        if (animPath != gCustomBodySpec.animsPrefixed.end()) {
            const bool reverse = gCustomBodySpec.reverseAnims.find(wanted) != gCustomBodySpec.reverseAnims.end();
            auto* loaded = reverse ? ResourceMgr_LoadAnimByName(animPath->second.c_str()) : nullptr;
            if (reverse && loaded == nullptr) {
                SPDLOG_WARN("ShipLua body animation: nao carreguei animacao reversa '{}'", wanted);
                return;
            }
            gCustomBodyCurrentAnim = wanted;
            const f32 lastFrame = reverse ? Animation_GetLastFrame(reinterpret_cast<LinkAnimationHeader*>(loaded)) : 0.0f;
            LinkAnimation_Change(play, &gCustomBodySkelAnime, (LinkAnimationHeader*)animPath->second.c_str(),
                                 reverse ? -1.0f : 1.0f, reverse ? lastFrame : 0.0f, 0.0f, ANIMMODE_LOOP, 0.0f);
            if (wanted == "punch_a" || wanted == "punch_b" || wanted == "punch_c") {
                gCustomBodySkelAnime.movementFlags |= ANIM_FLAG_NOMOVE;
            }
        }
    }
    const bool animationFinished = LinkAnimation_Update(play, &gCustomBodySkelAnime) != 0;
    // O Player de MM armazena B em actionVar2 enquanto punch A/B ainda roda.
    // Ignoramos o frame 0, que é o mesmo B que iniciou A, para não transformar
    // um único toque em combo automático.
    if ((gCustomBodyCurrentAnim == "punch_a" || gCustomBodyCurrentAnim == "punch_b") &&
        gCustomBodySkelAnime.curFrame >= 1.0f && CHECK_BTN_ALL(play->state.input[0].press.button, BTN_B)) {
        gCustomBodyPunchQueued = true;
    }
    CustomBodyApplyPunchRootMotion(actor, player);
    CustomBodyUpdatePunchHitbox(player, play);
    CustomBodyUpdateRollHitbox(player, play, rolling);
    if (animationFinished && gCustomBodyOverrideOnce) {
        const std::string completedAnimation = gCustomBodyCurrentAnim;
        if (CustomBodyGetPunchInfo() != nullptr && CustomBodyStartQueuedPunch(play, completedAnimation.c_str())) {
            return;
        }
        if (CustomBodyGetPunchInfo() != nullptr &&
            CustomBodyStartPunchRecovery(play, completedAnimation.c_str(), player)) {
            return;
        }
        if (gCustomBodyRollPhase == CustomBodyRollPhase::Enter) {
            gCustomBodyRollPhase = nativeRolling ? CustomBodyRollPhase::Rolling : CustomBodyRollPhase::None;
        } else if (gCustomBodyRollPhase == CustomBodyRollPhase::Exit) {
            gCustomBodyRollPhase = CustomBodyRollPhase::None;
        }
        gCustomBodyOverrideAnim.reset();
        gCustomBodyOverrideOnce = false;
    }
}

// A bola, a malha de espinhos e as duas camadas de energia são display lists
// distintas no MM. As energias usam o segmento 0x08 para TwoTexScroll, por
// isso precisam de um passe Xlu separado e nunca podem ser tratadas como uma
// textura comum do skeleton.
extern "C" void CustomBodyDrawRoll(PlayState* play, const char* path, const char* spikesPath,
                                   const char* energy1Path, const char* energy2Path, int charge) {
    Gfx* dList = reinterpret_cast<Gfx*>(const_cast<char*>(path));
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    Gfx* twoTexScroll = Gfx_TwoTexScroll(play->state.gfxCtx, 0, 0, 0, 0x40, 0x40, 1,
                                          play->gameplayFrames * 2, play->gameplayFrames * 2, 0x40, 0x40);
    gSPSegment(POLY_OPA_DISP++, 0x08, reinterpret_cast<uintptr_t>(twoTexScroll));
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(dList)) == 1) {
        gsSPPushCD(POLY_OPA_DISP++, dList);
    }
    gSPDisplayList(POLY_OPA_DISP++, dList);
    if (spikesPath != nullptr) {
        Gfx* spikes = reinterpret_cast<Gfx*>(const_cast<char*>(spikesPath));
        if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(spikes)) == 1) {
            gsSPPushCD(POLY_OPA_DISP++, spikes);
        }
        gSPDisplayList(POLY_OPA_DISP++, spikes);
    }

    // `grt_01_model` e `grt_02_model`: começam a aparecer no nível 5 de
    // carga, como no Player_Draw de MM. Após os espinhos, a energia fica
    // compacta e opaca em vez de continuar crescendo indefinidamente.
    if (charge >= 5 && energy1Path != nullptr && energy2Path != nullptr) {
        const bool spikesActive = gCustomBodyRollSpikesActive;
        const f32 energyScale = spikesActive ? 0.65f : std::min((charge - 4) * 0.02f, 1.0f);
        const u8 alpha = spikesActive ? 255 : static_cast<u8>(std::min(200.0f, energyScale * 200.0f));
        Gfx* energy1 = reinterpret_cast<Gfx*>(const_cast<char*>(energy1Path));
        Gfx* energy2 = reinterpret_cast<Gfx*>(const_cast<char*>(energy2Path));
        Gfx_SetupDL_25Xlu(play->state.gfxCtx);
        gSPSegment(POLY_XLU_DISP++, 0x08, reinterpret_cast<uintptr_t>(twoTexScroll));
        Matrix_Push();
        Matrix_Scale(1.0f, energyScale, energyScale, MTXMODE_APPLY);
        gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
        gDPSetEnvColor(POLY_XLU_DISP++, 155, 0, 0, alpha);
        if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(energy1)) == 1) {
            gsSPPushCD(POLY_XLU_DISP++, energy1);
        }
        gSPDisplayList(POLY_XLU_DISP++, energy1);
        gDPSetEnvColor(POLY_XLU_DISP++, (play->gameplayFrames & 1) == 0 ? 100 : 200, 0, 0, 255);
        if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(energy2)) == 1) {
            gsSPPushCD(POLY_XLU_DISP++, energy2);
        }
        gSPDisplayList(POLY_XLU_DISP++, energy2);
        Matrix_Pop();
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

bool CustomBodyIsGakkiAnimation() {
    return gCustomBodyCurrentAnim == "gakki_start" || gCustomBodyCurrentAnim == "gakki_wait" ||
           gCustomBodyCurrentAnim == "gakki_play" || gCustomBodyCurrentAnim == "gakki_play_a" ||
           gCustomBodyCurrentAnim == "gakki_play_l" || gCustomBodyCurrentAnim == "gakki_play_d" ||
           gCustomBodyCurrentAnim == "gakki_play_u" || gCustomBodyCurrentAnim == "gakki_play_r";
}

// Mesma tabela D_801C0428 usada pelo Player Goron nativo de MM. Manter os
// tres eixos separados preserva o pequeno "pop" assimetrico da abertura e
// tambem faz a saida reverse_once recolher os tambores no ritmo correto.
Vec3f CustomBodyGakkiScale() {
    if (gCustomBodyCurrentAnim != "gakki_start") {
        return { 1.0f, 1.0f, 1.0f };
    }

    struct GakkiScaleKey {
        f32 frame;
        Vec3f scale;
    };
    static const std::array<GakkiScaleKey, 7> kScaleKeys = {
        GakkiScaleKey{ 0.0f, { 0.0f, 0.0f, 0.0f } }, GakkiScaleKey{ 6.0f, { 0.0f, 0.0f, 0.0f } },
        GakkiScaleKey{ 7.0f, { 0.6f, 0.6f, 0.5f } }, GakkiScaleKey{ 8.0f, { 1.2f, 1.3f, 1.0f } },
        GakkiScaleKey{ 9.0f, { 1.0f, 1.2f, 0.8f } }, GakkiScaleKey{ 11.0f, { 1.0f, 1.0f, 1.0f } },
        GakkiScaleKey{ 13.0f, { 1.0f, 1.0f, 1.0f } },
    };

    const f32 frame = gCustomBodySkelAnime.curFrame;
    for (size_t i = 1; i < kScaleKeys.size(); ++i) {
        const auto& next = kScaleKeys[i];
        if (frame <= next.frame) {
            const auto& previous = kScaleKeys[i - 1];
            const f32 t = std::clamp((frame - previous.frame) / (next.frame - previous.frame), 0.0f, 1.0f);
            return {
                previous.scale.x + (next.scale.x - previous.scale.x) * t,
                previous.scale.y + (next.scale.y - previous.scale.y) * t,
                previous.scale.z + (next.scale.z - previous.scale.z) * t,
            };
        }
    }
    return kScaleKeys.back().scale;
}

// O efeito de soco é uma DL translúcida própria de MM. A mesma tabela de
// impacto já usada pelo collider decide quando desenhá-la; A usa mão esquerda,
// B mão direita e C a cintura, como o Player_PostLimbDrawGameplay original.
extern "C" void CustomBodyDrawPunchEffect(PlayState* play, s32 limbIndex) {
    if (!gCustomBodyPunchHitActive) {
        return;
    }
    const bool atTargetLimb = (gCustomBodyCurrentAnim == "punch_a" && limbIndex == PLAYER_LIMB_L_HAND) ||
                              (gCustomBodyCurrentAnim == "punch_b" && limbIndex == PLAYER_LIMB_R_HAND) ||
                              (gCustomBodyCurrentAnim == "punch_c" && limbIndex == PLAYER_LIMB_WAIST);
    const auto effect = gCustomBodySpec.modelsPrefixed.find("punch_effect");
    if (!atTargetLimb || effect == gCustomBodySpec.modelsPrefixed.end()) {
        return;
    }

    Gfx* dList = reinterpret_cast<Gfx*>(const_cast<char*>(effect->second.c_str()));
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_25Xlu(play->state.gfxCtx);
    gDPSetEnvColor(POLY_XLU_DISP++, 255, 0, 0, 180);
    gSPMatrix(POLY_XLU_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
    if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(dList)) == 1) {
        gsSPPushCD(POLY_XLU_DISP++, dList);
    }
    gSPDisplayList(POLY_XLU_DISP++, dList);
    CLOSE_DISPS(play->state.gfxCtx);
}

// O Player de MM anexa o conjunto de tambores no torso do Goron durante as
// animacoes gakki. O custom body usa o mesmo ponto de esqueleto (limb 0x15)
// e paths declarados pela spec; outros corpos nao ativam este callback.
extern "C" void CustomBodyPostLimbDraw(PlayState* play, s32 limbIndex, Gfx** dList, Vec3s* rot, void* arg) {
    constexpr s32 kGoronTorsoLimb = 0x15;
    CustomBodyDrawPunchEffect(play, limbIndex);
    if (limbIndex != kGoronTorsoLimb || !CustomBodyIsGakkiAnimation()) {
        return;
    }

    static constexpr std::array<const char*, 6> kGakkiModels = {
        "gakki_container", "gakki_piece_1", "gakki_piece_2", "gakki_piece_3", "gakki_piece_4", "gakki_piece_5",
    };
    const Vec3f scale = CustomBodyGakkiScale();
    if (scale.x <= 0.0f && scale.y <= 0.0f && scale.z <= 0.0f) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);
    for (const char* name : kGakkiModels) {
        const auto model = gCustomBodySpec.modelsPrefixed.find(name);
        if (model == gCustomBodySpec.modelsPrefixed.end()) {
            continue;
        }
        Gfx* dList = reinterpret_cast<Gfx*>(const_cast<char*>(model->second.c_str()));
        Matrix_Push();
        Matrix_Scale(scale.x, scale.y, scale.z, MTXMODE_APPLY);
        gSPMatrix(POLY_OPA_DISP++, MATRIX_NEWMTX(play->state.gfxCtx), G_MTX_MODELVIEW | G_MTX_LOAD);
        if (ResourceMgr_OTRSigCheck(reinterpret_cast<char*>(dList)) == 1) {
            gsSPPushCD(POLY_OPA_DISP++, dList);
        }
        gSPDisplayList(POLY_OPA_DISP++, dList);
        Matrix_Pop();
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

extern "C" void CustomBodyActorDraw(Actor* actor, PlayState* play) {
    if (!gCustomBodyActive) {
        return;
    }
    // A display list enrolada do Goron é geometria direta, não um skeleton.
    // Ela precisa do seu próprio pipeline Xlu, cor ambiente e ModelView.
    Player* player = GET_PLAYER(play);
    const auto rollModel = gCustomBodySpec.modelsPrefixed.find("roll");
    const bool waterBall = gCustomBodyWaterVoidPhase == CustomBodyWaterVoidPhase::Ball;
    const bool nativeRollOrGroundPound =
        gCustomBodyRollPhase == CustomBodyRollPhase::Rolling &&
        (player != nullptr && (player->actionFunc == Player_Action_Roll ||
                               gCustomBodyGroundPoundPhase != CustomBodyGroundPoundPhase::None));
    if (player != nullptr && (nativeRollOrGroundPound || waterBall) &&
        rollModel != gCustomBodySpec.modelsPrefixed.end()) {
        const auto spikes = gCustomBodySpec.modelsPrefixed.find("roll_spikes");
        const auto energy1 = gCustomBodySpec.modelsPrefixed.find("roll_energy_1");
        const auto energy2 = gCustomBodySpec.modelsPrefixed.find("roll_energy_2");
        const char* spikesPath = (!waterBall && gCustomBodyRollSpikesActive && spikes != gCustomBodySpec.modelsPrefixed.end())
                                     ? spikes->second.c_str()
                                     : nullptr;
        const char* energy1Path = !waterBall && energy1 != gCustomBodySpec.modelsPrefixed.end()
                                      ? energy1->second.c_str()
                                      : nullptr;
        const char* energy2Path = !waterBall && energy2 != gCustomBodySpec.modelsPrefixed.end()
                                      ? energy2->second.c_str()
                                      : nullptr;
        CustomBodyDrawRoll(play, rollModel->second.c_str(), spikesPath, energy1Path, energy2Path,
                           waterBall ? 0 : gCustomBodyRollCharge);
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);
    // Setup de render opaco padrão (SETUPDL_25: combiner/render mode/geometry
    // mode). SEM isto, SkelAnime_DrawFlexOpa emite a geometria com o estado de
    // pipeline herdado do ator desenhado logo antes — resultado em jogo:
    // corpo totalmente INVISÍVEL (não é bind pose, não é tamanho errado — não
    // desenha nada). Todo ator vanilla que desenha esqueleto chama isto após
    // OPEN_DISPS (ver z_en_go2.c:2032 e func_80093C80 usado pelo puppet). Foi
    // a divergência exata deste draw em relação aos exemplos que funcionam.
    Gfx_SetupDL_25Opa(play->state.gfxCtx);

    // Configure os segmentos antes de decidir entre skeleton e modelo de
    // estado: uma display list alternativa também pode referenciar texturas
    // segmentadas do objeto de Player.
    for (const auto& [segment, path] : gCustomBodySpec.segmentsPrefixed) {
        char* data = ResourceMgr_GetResourceDataByNameHandlingMQ(path.c_str());
        if (data != nullptr) {
            gSPSegment(POLY_OPA_DISP++, segment, reinterpret_cast<uintptr_t>(data));
        }
    }
    if (gCustomBodyShieldActive && gCustomBodyShieldSkelAnime.skeleton != nullptr) {
        SkelAnime_DrawFlexOpa(play, gCustomBodyShieldSkelAnime.skeleton, gCustomBodyShieldSkelAnime.jointTable,
                              gCustomBodyShieldSkelAnime.dListCount, nullptr, nullptr, actor);
    } else {
        SkelAnime_DrawFlexOpa(play, gCustomBodySkelAnime.skeleton, gCustomBodySkelAnime.jointTable,
                              gCustomBodySkelAnime.dListCount, nullptr, CustomBodyPostLimbDraw, actor);
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

bool CustomBodyActivateFromSpec(PlayState* play) {
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr) {
        return false;
    }
    // Todos os ponteiros abaixo saem de membros persistentes da spec global —
    // ver o comentário em CustomBodySpec: o engine guarda estes endereços.
    const auto defaultAnimPath = gCustomBodySpec.animsPrefixed.find(gCustomBodySpec.defaultAnim);
    if (gCustomBodySpec.skeletonPathPrefixed.empty() || defaultAnimPath == gCustomBodySpec.animsPrefixed.end()) {
        SPDLOG_WARN("ShipLua set_body: spec incompleta");
        return false;
    }
    if (!ResourceMgr_FileExists(gCustomBodySpec.skeletonPathPrefixed.c_str()) ||
        !ResourceMgr_FileExists(defaultAnimPath->second.c_str())) {
        SPDLOG_WARN("ShipLua set_body: assets indispon\xC3\xADveis para a spec atual");
        return false;
    }
    // Uma spec de corpo e atomica: nao basta o idle existir. Precarregamos e
    // validamos todos os headers declarados para evitar que uma habilidade
    // (por exemplo roll, tambor ou porta) revele uma animacao ausente somente
    // depois de a transformacao ja ter ocultado o Player vanilla.
    for (const auto& [name, path] : gCustomBodySpec.animsPrefixed) {
        if (!ResourceMgr_FileExists(path.c_str()) || ResourceMgr_LoadAnimByName(path.c_str()) == nullptr) {
            SPDLOG_WARN("ShipLua set_body: animacao '{}' indisponivel ('{}')", name, path);
            return false;
        }
    }
    const bool hasShieldSkeleton = !gCustomBodySpec.shieldSkeletonPathPrefixed.empty();
    const bool hasShieldAnimation = !gCustomBodySpec.shieldAnimationPathPrefixed.empty();
    if (hasShieldSkeleton != hasShieldAnimation) {
        SPDLOG_WARN("ShipLua set_body: 'shield' precisa declarar skeleton e animation juntos");
        return false;
    }
    if (hasShieldSkeleton &&
        (!ResourceMgr_FileExists(gCustomBodySpec.shieldSkeletonPathPrefixed.c_str()) ||
         !ResourceMgr_FileExists(gCustomBodySpec.shieldAnimationPathPrefixed.c_str()))) {
        SPDLOG_WARN("ShipLua set_body: assets de 'shield' indispon\xC3\xADveis");
        return false;
    }
    for (const auto& [segment, path] : gCustomBodySpec.segmentsPrefixed) {
        if (!ResourceMgr_FileExists(path.c_str())) {
            SPDLOG_WARN("ShipLua set_body: segmento 0x{:02X} indispon\xC3\xADvel ('{}')", segment, path);
            return false;
        }
    }
    for (const auto& [state, path] : gCustomBodySpec.modelsPrefixed) {
        if (!ResourceMgr_FileExists(path.c_str())) {
            SPDLOG_WARN("ShipLua set_body: modelo do estado '{}' indispon\xC3\xADvel ('{}')", state, path);
            return false;
        }
    }

    CustomBodyFreeSkelBuffers();
    // flags=9: mesmo modo usado pelo próprio Player e pelo puppet do Kafei —
    // buffers do chamador quando não-NULL, mas aqui passamos NULL para os
    // dois: aloca do tamanho real do header do skeleton fornecido pelo mod,
    // não de PLAYER_LIMB_MAX.
    SkelAnime_InitLink(play, &gCustomBodySkelAnime,
                       (FlexSkeletonHeader*)gCustomBodySpec.skeletonPathPrefixed.c_str(),
                       (LinkAnimationHeader*)defaultAnimPath->second.c_str(), 9, nullptr, nullptr, 0);
    gCustomBodyCurrentAnim = gCustomBodySpec.defaultAnim;

    if (hasShieldSkeleton) {
        // O escudo/curl do Goron é uma animação normal de quatro membros,
        // não LinkAnimation. Carregamos o header antes do InitFlex porque
        // Animation_PlayLoop opera sobre os dados, e não sobre o caminho OTR.
        auto* shieldAnimation = ResourceMgr_LoadAnimByName(gCustomBodySpec.shieldAnimationPathPrefixed.c_str());
        if (shieldAnimation == nullptr) {
            SPDLOG_WARN("ShipLua set_body: n\xC3\xA3o carreguei a anima\xC3\xA7\xC3\xA3o de 'shield'");
            CustomBodyFreeSkelBuffers();
            return false;
        }
        SkelAnime_InitFlex(play, &gCustomBodyShieldSkelAnime,
                           (FlexSkeletonHeader*)gCustomBodySpec.shieldSkeletonPathPrefixed.c_str(),
                           reinterpret_cast<AnimationHeader*>(shieldAnimation), nullptr, nullptr, 0);
        if (gCustomBodyShieldSkelAnime.jointTable == nullptr || gCustomBodyShieldSkelAnime.morphTable == nullptr) {
            SPDLOG_WARN("ShipLua set_body: sem mem\xC3\xB3ria para a postura de 'shield'");
            CustomBodyFreeSkelBuffers();
            return false;
        }
    }

    Actor* actor = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_ITEM00, player->actor.world.pos.x,
                               player->actor.world.pos.y, player->actor.world.pos.z, 0, 0, 0, 0);
    if (actor == nullptr) {
        SPDLOG_WARN("ShipLua set_body: n\xC3\xA3o consegui spawnar o ator hospedeiro");
        CustomBodyFreeSkelBuffers();
        return false;
    }
    actor->update = CustomBodyActorUpdate;
    actor->draw = CustomBodyActorDraw;
    // O ator hospedeiro é um En_Item00: o Init dele (params 0 = rupia verde)
    // já aplicou Actor_SetScale(0.015). Reafirma a escala do Player real
    // (z_player.c:12254, scale = 0.01) — o esqueleto do Goron é da família
    // Player e é desenhado nessa mesma escala de mundo. Sem isto, o corpo
    // sairia em tamanho de rupia.
    Actor_SetScale(actor, 0.01f);
    gCustomBodyActor = actor;
    gCustomBodyActive = true;
    player->stateFlags2 |= PLAYER_STATE2_DISABLE_DRAW;
    MaskTransitionBeginReveal(player);
    return true;
}

// ship.oot.player.set_body(spec): spec = { skeleton = "caminho/sem/prefixo",
// anims = { nome = "caminho", ... }, reverse_anims = { nome = true, ... },
// segments = { [8] = "textura", ... },
// models = { roll = "display/list", ... }, shield = { skeleton = "...",
// animation = "..." } (opcional), default_anim = "nome" (opcional, usa
// "idle" ou a 1a chave se omitido), water_void = true (opcional) }. spec =
// nil desliga e esquece a forma.
int LuaSetPlayerBody(lua_State* state) {
    PlayState* play = gPlayState;
    if (lua_isnil(state, 1)) {
        Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
        CustomBodyDeactivate(play, true);
        // A saida mask_off chega aqui no pico do flash; o desenho nativo de
        // Link volta antes do fade, portanto a troca tambem fica invisivel.
        MaskTransitionBeginReveal(player);
        SPDLOG_INFO("ShipLua set_body: corpo customizado desligado");
        lua_pushboolean(state, 1);
        return 1;
    }
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr) {
        SPDLOG_WARN("ShipLua set_body: fora de gameplay");
        lua_pushboolean(state, 0);
        return 1;
    }
    luaL_checktype(state, 1, LUA_TTABLE);

    lua_getfield(state, 1, "skeleton");
    const char* skeleton = luaL_optstring(state, -1, nullptr);
    if (skeleton == nullptr || *skeleton == '\0') {
        SPDLOG_WARN("ShipLua set_body: 'skeleton' \xC3\xA9 obrigat\xC3\xB3rio");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    CustomBodySpec spec;
    spec.skeletonPath = skeleton;
    lua_pop(state, 1);

    lua_getfield(state, 1, "anims");
    if (!lua_istable(state, -1)) {
        SPDLOG_WARN("ShipLua set_body: 'anims' \xC3\xA9 obrigat\xC3\xB3ria (tabela nome->caminho)");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    lua_pushnil(state);
    while (lua_next(state, -2) != 0) {
        if (lua_type(state, -2) == LUA_TSTRING && lua_type(state, -1) == LUA_TSTRING) {
            spec.anims[lua_tostring(state, -2)] = lua_tostring(state, -1);
        }
        lua_pop(state, 1);
    }
    lua_pop(state, 1); // anims

    if (spec.anims.empty()) {
        SPDLOG_WARN("ShipLua set_body: 'anims' n\xC3\xA3o tem nenhuma entrada v\xC3\xA1lida");
        lua_pushboolean(state, 0);
        return 1;
    }

    lua_getfield(state, 1, "reverse_anims");
    if (!lua_isnil(state, -1) && !lua_istable(state, -1)) {
        SPDLOG_WARN("ShipLua set_body: 'reverse_anims' precisa ser tabela nome->boolean");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    if (lua_istable(state, -1)) {
        lua_pushnil(state);
        while (lua_next(state, -2) != 0) {
            const bool valid = lua_type(state, -2) == LUA_TSTRING && lua_isboolean(state, -1);
            const char* name = valid ? lua_tostring(state, -2) : nullptr;
            const bool enabled = valid && lua_toboolean(state, -1) != 0;
            if (!valid || (enabled && spec.anims.find(name) == spec.anims.end())) {
                SPDLOG_WARN("ShipLua set_body: 'reverse_anims' aceita apenas chaves de 'anims' com valor boolean");
                lua_pop(state, 2);
                lua_pushboolean(state, 0);
                return 1;
            }
            if (enabled) {
                spec.reverseAnims.insert(name);
            }
            lua_pop(state, 1);
        }
    }
    lua_pop(state, 1); // reverse_anims

    lua_getfield(state, 1, "segments");
    if (!lua_isnil(state, -1) && !lua_istable(state, -1)) {
        SPDLOG_WARN("ShipLua set_body: 'segments' precisa ser tabela índice->caminho");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    if (lua_istable(state, -1)) {
        lua_pushnil(state);
        while (lua_next(state, -2) != 0) {
            const bool valid = lua_isinteger(state, -2) && lua_type(state, -1) == LUA_TSTRING;
            const int segment = valid ? static_cast<int>(lua_tointeger(state, -2)) : 0;
            if (!valid || segment < 1 || segment > 15) {
                SPDLOG_WARN("ShipLua set_body: 'segments' aceita apenas índices 1..15 e caminhos string");
                lua_pop(state, 2);
                lua_pushboolean(state, 0);
                return 1;
            }
            spec.segments[segment] = lua_tostring(state, -1);
            lua_pop(state, 1);
        }
    }
    lua_pop(state, 1); // segments

    lua_getfield(state, 1, "models");
    if (!lua_isnil(state, -1) && !lua_istable(state, -1)) {
        SPDLOG_WARN("ShipLua set_body: 'models' precisa ser tabela estado->caminho");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    if (lua_istable(state, -1)) {
        lua_pushnil(state);
        while (lua_next(state, -2) != 0) {
            if (lua_type(state, -2) != LUA_TSTRING || lua_type(state, -1) != LUA_TSTRING) {
                SPDLOG_WARN("ShipLua set_body: 'models' aceita apenas estado string -> caminho string");
                lua_pop(state, 2);
                lua_pushboolean(state, 0);
                return 1;
            }
            spec.models[lua_tostring(state, -2)] = lua_tostring(state, -1);
            lua_pop(state, 1);
        }
    }
    lua_pop(state, 1); // models

    lua_getfield(state, 1, "shield");
    if (!lua_isnil(state, -1) && !lua_istable(state, -1)) {
        SPDLOG_WARN("ShipLua set_body: 'shield' precisa ser tabela { skeleton, animation }");
        lua_pop(state, 1);
        lua_pushboolean(state, 0);
        return 1;
    }
    if (lua_istable(state, -1)) {
        lua_getfield(state, -1, "skeleton");
        const char* shieldSkeleton = luaL_optstring(state, -1, nullptr);
        if (shieldSkeleton == nullptr || *shieldSkeleton == '\0') {
            SPDLOG_WARN("ShipLua set_body: 'shield.skeleton' \xC3\xA9 obrigat\xC3\xB3rio");
            lua_pop(state, 2);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.shieldSkeletonPath = shieldSkeleton;
        lua_pop(state, 1);

        lua_getfield(state, -1, "animation");
        const char* shieldAnimation = luaL_optstring(state, -1, nullptr);
        if (shieldAnimation == nullptr || *shieldAnimation == '\0') {
            SPDLOG_WARN("ShipLua set_body: 'shield.animation' \xC3\xA9 obrigat\xC3\xB3rio");
            lua_pop(state, 2);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.shieldAnimationPath = shieldAnimation;
        lua_pop(state, 1);
    }
    lua_pop(state, 1); // shield

    lua_getfield(state, 1, "ground_offset");
    if (!lua_isnil(state, -1)) {
        if (!lua_isnumber(state, -1)) {
            SPDLOG_WARN("ShipLua set_body: 'ground_offset' precisa ser n\xC3\xBAmero");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.groundOffset = static_cast<f32>(lua_tonumber(state, -1));
        if (!std::isfinite(spec.groundOffset) || fabsf(spec.groundOffset) > 10000.0f) {
            SPDLOG_WARN("ShipLua set_body: 'ground_offset' fora da faixa segura");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
    }
    lua_pop(state, 1);

    lua_getfield(state, 1, "roll_offset");
    if (!lua_isnil(state, -1)) {
        if (!lua_isnumber(state, -1)) {
            SPDLOG_WARN("ShipLua set_body: 'roll_offset' precisa ser n\xC3\xBAmero");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.rollOffset = static_cast<f32>(lua_tonumber(state, -1));
        if (!std::isfinite(spec.rollOffset) || fabsf(spec.rollOffset) > 10000.0f) {
            SPDLOG_WARN("ShipLua set_body: 'roll_offset' fora da faixa segura");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
    }
    lua_pop(state, 1);

    lua_getfield(state, 1, "water_void");
    if (!lua_isnil(state, -1)) {
        if (!lua_isboolean(state, -1)) {
            SPDLOG_WARN("ShipLua set_body: 'water_void' precisa ser booleano");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.waterVoid = lua_toboolean(state, -1) != 0;
    }
    lua_pop(state, 1);

    lua_getfield(state, 1, "block_ledge_grab");
    if (!lua_isnil(state, -1)) {
        if (!lua_isboolean(state, -1)) {
            SPDLOG_WARN("ShipLua set_body: 'block_ledge_grab' precisa ser booleano");
            lua_pop(state, 1);
            lua_pushboolean(state, 0);
            return 1;
        }
        spec.blockLedgeGrab = lua_toboolean(state, -1) != 0;
    }
    lua_pop(state, 1);

    lua_getfield(state, 1, "default_anim");
    const char* defaultAnim = luaL_optstring(state, -1, nullptr);
    if (defaultAnim != nullptr && spec.anims.find(defaultAnim) != spec.anims.end()) {
        spec.defaultAnim = defaultAnim;
    } else if (spec.anims.find("idle") != spec.anims.end()) {
        spec.defaultAnim = "idle";
    } else {
        spec.defaultAnim = spec.anims.begin()->first;
    }
    lua_pop(state, 1);

    // Monta os espelhos com "__OTR__" UMA vez, aqui: são estes que fornecem os
    // ponteiros ao engine, e precisam viver enquanto o corpo estiver ativo.
    spec.skeletonPathPrefixed = std::string("__OTR__") + spec.skeletonPath;
    for (const auto& [animName, animPath] : spec.anims) {
        spec.animsPrefixed[animName] = std::string("__OTR__") + animPath;
    }
    for (const auto& [segment, path] : spec.segments) {
        spec.segmentsPrefixed[segment] = std::string("__OTR__") + path;
    }
    for (const auto& [stateName, path] : spec.models) {
        spec.modelsPrefixed[stateName] = std::string("__OTR__") + path;
    }
    if (!spec.shieldSkeletonPath.empty()) {
        spec.shieldSkeletonPathPrefixed = std::string("__OTR__") + spec.shieldSkeletonPath;
        spec.shieldAnimationPathPrefixed = std::string("__OTR__") + spec.shieldAnimationPath;
    }

    CustomBodyDeactivate(play, false); // desliga o corpo antigo, mas a spec só troca depois
    gCustomBodySpec = spec;
    if (!CustomBodyActivateFromSpec(play)) {
        gCustomBodySpec = CustomBodySpec{};
        lua_pushboolean(state, 0);
        return 1;
    }
    SPDLOG_INFO("ShipLua set_body: corpo customizado ligado ('{}')", spec.skeletonPath);
    lua_pushboolean(state, 1);
    return 1;
}

// ship.oot.player.get_body(): retorna a spec atual como tabela, ou nil se
// nenhum corpo customizado está ativo. Permite a um mod checar/coexistir com
// outro que já tenha trocado o corpo.
int LuaGetPlayerBody(lua_State* state) {
    if (!gCustomBodyActive) {
        lua_pushnil(state);
        return 1;
    }
    lua_newtable(state);
    lua_pushstring(state, gCustomBodySpec.skeletonPath.c_str());
    lua_setfield(state, -2, "skeleton");
    lua_pushstring(state, gCustomBodySpec.defaultAnim.c_str());
    lua_setfield(state, -2, "default_anim");
    lua_pushstring(state, gCustomBodyCurrentAnim.c_str());
    lua_setfield(state, -2, "current_anim");
    lua_newtable(state);
    for (const auto& [segment, path] : gCustomBodySpec.segments) {
        lua_pushstring(state, path.c_str());
        lua_rawseti(state, -2, segment);
    }
    lua_setfield(state, -2, "segments");
    lua_newtable(state);
    for (const auto& [name, path] : gCustomBodySpec.models) {
        lua_pushstring(state, path.c_str());
        lua_setfield(state, -2, name.c_str());
    }
    lua_setfield(state, -2, "models");
    lua_newtable(state);
    for (const auto& name : gCustomBodySpec.reverseAnims) {
        lua_pushboolean(state, 1);
        lua_setfield(state, -2, name.c_str());
    }
    lua_setfield(state, -2, "reverse_anims");
    lua_pushnumber(state, gCustomBodySpec.groundOffset);
    lua_setfield(state, -2, "ground_offset");
    lua_pushnumber(state, gCustomBodySpec.rollOffset);
    lua_setfield(state, -2, "roll_offset");
    lua_pushboolean(state, gCustomBodySpec.waterVoid);
    lua_setfield(state, -2, "water_void");
    lua_pushboolean(state, gCustomBodySpec.blockLedgeGrab);
    lua_setfield(state, -2, "block_ledge_grab");
    lua_pushboolean(state, gCustomBodyWaterVoidPhase != CustomBodyWaterVoidPhase::None);
    lua_setfield(state, -2, "water_void_active");
    lua_pushstring(state, CustomBodyWaterVoidPhaseName());
    lua_setfield(state, -2, "water_void_phase");
    lua_pushinteger(state, gCustomBodyWaterVoidTimer);
    lua_setfield(state, -2, "water_void_frames");
    lua_pushinteger(state, gCustomBodyRollCharge);
    lua_setfield(state, -2, "roll_charge");
    lua_pushstring(state, CustomBodyCurrentRollPhaseName());
    lua_setfield(state, -2, "roll_phase");
    lua_pushboolean(state, gCustomBodyPunchHitActive);
    lua_setfield(state, -2, "punch_hit_active");
    lua_pushboolean(state, gCustomBodyPunchQueued);
    lua_setfield(state, -2, "punch_combo_queued");
    lua_pushboolean(state, gCustomBodyRollColliderArmed);
    lua_setfield(state, -2, "roll_attack_active");
    lua_pushboolean(state, gCustomBodyRollPhase == CustomBodyRollPhase::Rolling && gCustomBodyRollCharge >= 5);
    lua_setfield(state, -2, "roll_energy_active");
    lua_pushboolean(state, gCustomBodyRollSpikesActive);
    lua_setfield(state, -2, "roll_spikes_active");
    lua_pushboolean(state, gCustomBodyRollWallBounceTimer > 0);
    lua_setfield(state, -2, "roll_bounce_active");
    lua_pushinteger(state, gCustomBodyRollWallBounceTimer);
    lua_setfield(state, -2, "roll_bounce_frames");
    lua_pushboolean(state, gCustomBodyGroundPoundPhase != CustomBodyGroundPoundPhase::None);
    lua_setfield(state, -2, "ground_pound_active");
    lua_pushboolean(state, gCustomBodyShieldActive);
    lua_setfield(state, -2, "shield_active");
    if (!gCustomBodySpec.shieldSkeletonPath.empty()) {
        lua_newtable(state);
        lua_pushstring(state, gCustomBodySpec.shieldSkeletonPath.c_str());
        lua_setfield(state, -2, "skeleton");
        lua_pushstring(state, gCustomBodySpec.shieldAnimationPath.c_str());
        lua_setfield(state, -2, "animation");
        lua_setfield(state, -2, "shield");
    }
    lua_newtable(state);
    for (const auto& [name, path] : gCustomBodySpec.anims) {
        lua_pushstring(state, path.c_str());
        lua_setfield(state, -2, name.c_str());
    }
    lua_setfield(state, -2, "anims");
    return 1;
}

// Toca uma animação nomeada declarada na spec atual. Em modo "once", a
// seleção automática volta a assumir no frame seguinte ao término.
int LuaPlayPlayerBodyAnimation(lua_State* state) {
    PlayState* play = gPlayState;
    if (!gCustomBodyActive || play == nullptr) {
        lua_pushboolean(state, 0);
        return 1;
    }
    const char* name = luaL_checkstring(state, 1);
    const auto path = gCustomBodySpec.animsPrefixed.find(name);
    if (path == gCustomBodySpec.animsPrefixed.end()) {
        SPDLOG_WARN("ShipLua play_body_animation: anima\xC3\xA7\xC3\xA3o desconhecida '{}'", name);
        lua_pushboolean(state, 0);
        return 1;
    }
    const char* mode = luaL_optstring(state, 2, "once");
    const bool once = strcmp(mode, "once") == 0;
    const bool loop = strcmp(mode, "loop") == 0;
    const bool reverseOnce = strcmp(mode, "reverse_once") == 0;
    if (!once && !loop && !reverseOnce) {
        SPDLOG_WARN("ShipLua play_body_animation: modo precisa ser 'once', 'loop' ou 'reverse_once'");
        lua_pushboolean(state, 0);
        return 1;
    }
    const double requestedSpeed = luaL_optnumber(state, 3, 1.0);
    if (!std::isfinite(requestedSpeed) || requestedSpeed <= 0.0 || requestedSpeed > 4.0) {
        SPDLOG_WARN("ShipLua play_body_animation: velocidade fora da faixa segura");
        lua_pushboolean(state, 0);
        return 1;
    }
    auto* loaded = ResourceMgr_LoadAnimByName(path->second.c_str());
    if (loaded == nullptr) {
        SPDLOG_WARN("ShipLua play_body_animation: n\xC3\xA3o carreguei '{}'", name);
        lua_pushboolean(state, 0);
        return 1;
    }
    const f32 lastFrame = Animation_GetLastFrame(reinterpret_cast<LinkAnimationHeader*>(loaded));
    LinkAnimation_Change(play, &gCustomBodySkelAnime,
                          reinterpret_cast<LinkAnimationHeader*>(const_cast<char*>(path->second.c_str())),
                          reverseOnce ? -static_cast<f32>(requestedSpeed) : static_cast<f32>(requestedSpeed),
                          reverseOnce ? lastFrame : 0.0f, reverseOnce ? 0.0f : lastFrame,
                          loop ? ANIMMODE_LOOP : ANIMMODE_ONCE, 0.0f);
    if (once && std::strcmp(name, "mask_off") == 0) {
        Player* player = GET_PLAYER(play);
        if (player != nullptr) {
            MaskTransitionStart(player, static_cast<int>(ceilf(lastFrame)) + 1);
        }
    }
    if (std::strcmp(name, "punch_a") == 0 || std::strcmp(name, "punch_b") == 0 || std::strcmp(name, "punch_c") == 0) {
        gCustomBodySkelAnime.movementFlags |= ANIM_FLAG_NOMOVE;
    }
    gCustomBodyCurrentAnim = name;
    gCustomBodyOverrideAnim = name;
    gCustomBodyOverrideOnce = once || reverseOnce;
    lua_pushboolean(state, 1);
    return 1;
}

// Troca, em tempo de execução, um segmento que a display list lê (por exemplo
// 0x08 dos olhos do Goron). O caminho fica armazenado na spec persistente.
int LuaSetPlayerBodySegment(lua_State* state) {
    if (!gCustomBodyActive) {
        lua_pushboolean(state, 0);
        return 1;
    }
    const int segment = static_cast<int>(luaL_checkinteger(state, 1));
    const char* path = luaL_checkstring(state, 2);
    if (segment < 1 || segment > 15 || *path == '\0') {
        SPDLOG_WARN("ShipLua set_body_segment: segmento precisa estar entre 1 e 15");
        lua_pushboolean(state, 0);
        return 1;
    }
    const std::string prefixed = std::string("__OTR__") + path;
    if (!ResourceMgr_FileExists(prefixed.c_str())) {
        SPDLOG_WARN("ShipLua set_body_segment: resource indispon\xC3\xADvel '{}'", path);
        lua_pushboolean(state, 0);
        return 1;
    }
    gCustomBodySpec.segments[segment] = path;
    gCustomBodySpec.segmentsPrefixed[segment] = prefixed;
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
        // Durante a entrada, remover a máscara é um cancelamento explícito.
        // Durante a saída, set_body(nil) já promoveu Conceal -> Reveal e o
        // mod limpa a máscara no mesmo callback; preservar o fade evita que
        // Link apareça antes do último frame branco.
        if (gMaskTransitionPhase == MaskTransitionPhase::Conceal) {
            MaskTransitionReset(player);
        }
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

// A entrada de uma transformação de MM começa no Player humano com
// gPlayerAnim_cl_setmask. Ao contrário do corte completo de MM, esta
// primitive não toma câmera, cutscene ou física: ela só toca a animação
// segura, em repouso, e devolve a duração para o mod sincronizar a troca de
// corpo. O caminho é estático porque LinkAnimation_Change o consulta em
// frames posteriores.
constexpr char kMaskOnAnimationPath[] = "__OTR__mm/objects/gameplay_keep/gPlayerAnim_cl_setmask";

int LuaPlayMaskOnAnimation(lua_State* state) {
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || gCustomBodyActive ||
        (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_INPUT_DISABLED)) ||
        (player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) == 0 || fabsf(player->linearVelocity) >= 0.5f) {
        SPDLOG_WARN("ShipLua play_mask_on_animation: exige Player parado no ch\xC3\xA3o e sem corpo externo");
        lua_pushinteger(state, 0);
        return 1;
    }
    auto* loaded = ResourceMgr_LoadAnimByName(kMaskOnAnimationPath);
    if (loaded == nullptr) {
        SPDLOG_WARN("ShipLua play_mask_on_animation: anima\xC3\xA7\xC3\xA3o MM indispon\xC3\xADvel");
        lua_pushinteger(state, 0);
        return 1;
    }
    const f32 lastFrame = Animation_GetLastFrame(reinterpret_cast<LinkAnimationHeader*>(loaded));
    LinkAnimation_Change(play, &player->skelAnime,
                         reinterpret_cast<LinkAnimationHeader*>(const_cast<char*>(kMaskOnAnimationPath)), 1.0f, 0.0f,
                         lastFrame, ANIMMODE_ONCE, 0.0f);
    const int transitionFrames = static_cast<int>(ceilf(lastFrame)) + 1;
    MaskTransitionStart(player, transitionFrames);
    lua_pushinteger(state, static_cast<lua_Integer>(transitionFrames));
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
HOOK_ID gRollStartHook = 0;
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
HOOK_ID gHeldItemDrawHook = 0;
HOOK_ID gMaskTransitionUpdateHook = 0;
HOOK_ID gHookFirstPersonHook = 0;
HOOK_ID gHookArrowTypeSelectHook = 0;
HOOK_ID gHookEnemyDefeatHook = 0;

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

// ship.oot.player.set_held_item_model(slot, path): desenha um DL arbitrário
// ancorado no osso da mão do player — o mecanismo que praticamente todo item
// customizado com modelo na mão precisa (achado minerando dois forks de
// sistemas de item independentes: Ball and Chain, Gust Jar, Whip, Dominion
// Rod, Cane of Somaria, os cajados... 10+ itens no relatório de um deles
// dependiam exatamente disto). bodyPartsPos já vem resolvido pelo próprio
// desenho do esqueleto do player no mesmo frame — só ler e desenhar.
// ---------------------------------------------------------------------------
// HUD: primitiva genérica de desenho em overlay.
//
// Um mod que precise mostrar qualquer medidor próprio (fome/sede/stamina de um
// sistema de sobrevivência, barra de XP de um RPG, cronômetro) não tinha como
// desenhar nada — a única saída era sequestrar os medidores do jogo. Aqui o
// host só oferece "desenhe um retângulo colorido" e "escreva um texto"; o que
// isso significa é decisão inteira do mod.
//
// Ancorado no evento hook.oot.hud.draw, disparado de OnPlayDrawEnd. Emitir em
// OVERLAY_DISP é o que garante a ordem correta: os buckets de display list
// (POLY_OPA/POLY_XLU/OVERLAY) são concatenados numa ordem fixa no fim do frame,
// então o overlay sai por cima do HUD nativo mesmo sendo emitido antes dele.
//
// gMagicMeterFillTex é a textura de preenchimento sólido que o próprio medidor
// de magia usa (z_parameter.c, Interface_DrawMagicBar) — reaproveitá-la evita
// trocar o cycle type para G_CYC_FILL, que exigiria restaurar estado e poderia
// corromper o desenho seguinte.
// OPEN_DISPS/CLOSE_DISPS expandem para estas funções. frame_interpolation.h as
// declara FORA do seu bloco extern "C" (que está vazio), então em C++ elas
// ganhariam linkage C++ — mas a definição, em frame_interpolation.cpp, está
// dentro de extern "C". O resto deste arquivo só usa as macros dentro de
// funções extern "C" e por isso nunca esbarrou nisso; as funções de HUD abaixo
// são C++ normais, então precisam da declaração com o linkage certo.
extern "C" {
void FrameInterpolation_RecordOpenChild(const void* a, int b);
void FrameInterpolation_RecordCloseChild(void);
}

bool gHudDrawActive = false; // true só durante o dispatch do evento

// Orçamento de desenho por frame. Cada retângulo custa comandos na display
// list, e o pool gráfico do jogo é fixo: um mod que desenhe demais estoura o
// buffer e derruba o host inteiro (Fault "região dinâmica destruída" em
// graph.c). O host não pode depender do bom senso do mod — corta no teto e
// avisa uma vez, em vez de crashar.
constexpr int kHudMaxRectsPerFrame = 400;
int gHudRectsThisFrame = 0;
bool gHudBudgetWarned = false;
// A textura de preenchimento é carregada UMA vez por frame, não por retângulo:
// gDPLoadMultiBlock_4b sozinho expande para vários comandos GBI, e recarregá-la
// a cada chamada foi o que estourou o pool. draw_text usa o caminho nativo de
// texto (POLY_OPA) e mexe no tile, então marca para recarregar.
bool gHudTileDirty = true;

// Carrega a textura de preenchimento no tile, se necessário.
void HudEnsureFillTexture(PlayState* play) {
    if (!gHudTileDirty) {
        return;
    }
    OPEN_DISPS(play->state.gfxCtx);
    gDPLoadMultiBlock_4b(OVERLAY_DISP++, gMagicMeterFillTex, 0, G_TX_RENDERTILE, G_IM_FMT_I, 16, 16, 0,
                         G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMIRROR | G_TX_WRAP, G_TX_NOMASK, G_TX_NOMASK, G_TX_NOLOD,
                         G_TX_NOLOD);
    CLOSE_DISPS(play->state.gfxCtx);
    gHudTileDirty = false;
}

// Reserva espaço no orçamento; false quando o teto já foi atingido.
bool HudTakeBudget(int count) {
    if (gHudRectsThisFrame + count > kHudMaxRectsPerFrame) {
        if (!gHudBudgetWarned) {
            gHudBudgetWarned = true;
            SPDLOG_WARN("ShipLua hud: teto de {} retângulos por frame atingido — o excedente deste frame não "
                        "será desenhado (evita estourar a display list)",
                        kHudMaxRectsPerFrame);
        }
        return false;
    }
    gHudRectsThisFrame += count;
    return true;
}

// Modelos anexados às mãos (ship.oot.player.set_held_item_model). Declarados
// aqui porque DrawHeldItemModels, logo abaixo, os consome.
std::string gHeldItemModelLeft;
std::string gHeldItemModelRight;

void HudBeginOverlay(PlayState* play) {
    OPEN_DISPS(play->state.gfxCtx);
    // Setup de overlay padrão — sem isto a geometria sai com o estado de render
    // herdado do que foi desenhado antes (mesma classe de bug que deixou o
    // corpo do Goron invisível).
    Gfx_SetupDL_39Overlay(play->state.gfxCtx);
    gDPSetCombineLERP(OVERLAY_DISP++, PRIMITIVE, ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE, PRIMITIVE,
                      ENVIRONMENT, TEXEL0, ENVIRONMENT, 0, 0, 0, PRIMITIVE);
    gDPSetEnvColor(OVERLAY_DISP++, 0, 0, 0, 255);
    CLOSE_DISPS(play->state.gfxCtx);
}

// ship.hud.draw_rect(x, y, w, h, r, g, b, a)
int LuaHudDrawRect(lua_State* state) {
    PlayState* play = gPlayState;
    if (!gHudDrawActive || play == nullptr) {
        SPDLOG_WARN("ShipLua hud.draw_rect: s\xC3\xB3 pode ser chamado de hook.oot.hud.draw");
        lua_pushboolean(state, 0);
        return 1;
    }
    const int x = static_cast<int>(luaL_checkinteger(state, 1));
    const int y = static_cast<int>(luaL_checkinteger(state, 2));
    const int w = static_cast<int>(luaL_checkinteger(state, 3));
    const int h = static_cast<int>(luaL_checkinteger(state, 4));
    const int r = static_cast<int>(luaL_optinteger(state, 5, 255));
    const int g = static_cast<int>(luaL_optinteger(state, 6, 255));
    const int b = static_cast<int>(luaL_optinteger(state, 7, 255));
    const int a = static_cast<int>(luaL_optinteger(state, 8, 255));
    if (w <= 0 || h <= 0) {
        lua_pushboolean(state, 0);
        return 1;
    }
    if (!HudTakeBudget(1)) {
        lua_pushboolean(state, 0);
        return 1;
    }
    const auto clamp8 = [](int v) { return static_cast<u8>(std::clamp(v, 0, 255)); };

    HudEnsureFillTexture(play);
    OPEN_DISPS(play->state.gfxCtx);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, clamp8(r), clamp8(g), clamp8(b), clamp8(a));
    gSPWideTextureRectangle(OVERLAY_DISP++, x << 2, y << 2, (x + w) << 2, (y + h) << 2, G_TX_RENDERTILE, 0, 0, 1 << 10,
                            1 << 10);
    CLOSE_DISPS(play->state.gfxCtx);
    lua_pushboolean(state, 1);
    return 1;
}

// ship.hud.draw_ring(cx, cy, raio, espessura, fração, r, g, b, a)
//
// Desenha um arco circular preenchido de `fração` (0..1) a partir do topo, no
// sentido horário — a roda de stamina de BotW/Skyward Sword. Combinada com
// ship.player.get("screen_x"/"screen_y"), o medidor acompanha o personagem.
//
// Implementação: o arco é composto por quadrados pequenos ao longo da
// circunferência. gSPTextureRectangle só desenha retângulos alinhados aos
// eixos — não há como rotacionar um quad por aqui sem montar vértices e
// matriz à mão. Com raio típico (12-20px) e ~48 segmentos o resultado lê como
// um anel contínuo. É uma aproximação assumida, não um arco analítico.
int LuaHudDrawRing(lua_State* state) {
    PlayState* play = gPlayState;
    if (!gHudDrawActive || play == nullptr) {
        SPDLOG_WARN("ShipLua hud.draw_ring: s\xC3\xB3 pode ser chamado de hook.oot.hud.draw");
        lua_pushboolean(state, 0);
        return 1;
    }
    const double cx = luaL_checknumber(state, 1);
    const double cy = luaL_checknumber(state, 2);
    const double radius = luaL_checknumber(state, 3);
    const double thickness = luaL_optnumber(state, 4, 3.0);
    const double fraction = std::clamp(luaL_optnumber(state, 5, 1.0), 0.0, 1.0);
    const int r = static_cast<int>(luaL_optinteger(state, 6, 255));
    const int g = static_cast<int>(luaL_optinteger(state, 7, 255));
    const int b = static_cast<int>(luaL_optinteger(state, 8, 255));
    const int a = static_cast<int>(luaL_optinteger(state, 9, 255));
    if (radius <= 0.0 || fraction <= 0.0) {
        lua_pushboolean(state, 1); // nada a desenhar não é erro
        return 1;
    }
    const auto clamp8 = [](int v) { return static_cast<u8>(std::clamp(v, 0, 255)); };
    // Densidade de segmentos: 1.5x o raio dá um anel visualmente contínuo sem
    // torrar a display list (era 3x, e um anel grande sozinho consumia quase
    // 100 retângulos por frame).
    const int seg = static_cast<int>(std::clamp(radius * 1.5, 12.0, 48.0));
    const int drawn = static_cast<int>(seg * fraction + 0.5);
    const int dot = std::max(1, static_cast<int>(thickness + 0.5));

    if (drawn <= 0) {
        lua_pushboolean(state, 1);
        return 1;
    }
    if (!HudTakeBudget(drawn)) {
        lua_pushboolean(state, 0);
        return 1;
    }

    HudEnsureFillTexture(play);
    OPEN_DISPS(play->state.gfxCtx);
    gDPSetPrimColor(OVERLAY_DISP++, 0, 0, clamp8(r), clamp8(g), clamp8(b), clamp8(a));
    // Constante local: M_PI não é garantido no MSVC sem _USE_MATH_DEFINES
    // antes dos headers de math, e mexer na ordem de includes deste arquivo
    // por causa disso não vale a pena.
    constexpr double kPi = 3.14159265358979323846;
    for (int i = 0; i < drawn; ++i) {
        // Começa no topo (-PI/2) e avança no sentido horário.
        const double ang = -kPi / 2.0 + (2.0 * kPi * i) / seg;
        const int px = static_cast<int>(cx + std::cos(ang) * radius - dot / 2.0 + 0.5);
        const int py = static_cast<int>(cy + std::sin(ang) * radius - dot / 2.0 + 0.5);
        gSPWideTextureRectangle(OVERLAY_DISP++, px << 2, py << 2, (px + dot) << 2, (py + dot) << 2, G_TX_RENDERTILE, 0,
                                0, 1 << 10, 1 << 10);
    }
    CLOSE_DISPS(play->state.gfxCtx);
    lua_pushboolean(state, 1);
    return 1;
}

// ship.hud.draw_text(text, x, y, r, g, b, a, scale)
int LuaHudDrawText(lua_State* state) {
    PlayState* play = gPlayState;
    if (!gHudDrawActive || play == nullptr) {
        SPDLOG_WARN("ShipLua hud.draw_text: s\xC3\xB3 pode ser chamado de hook.oot.hud.draw");
        lua_pushboolean(state, 0);
        return 1;
    }
    const char* text = luaL_checkstring(state, 1);
    const int x = static_cast<int>(luaL_checkinteger(state, 2));
    const int y = static_cast<int>(luaL_checkinteger(state, 3));
    const int r = static_cast<int>(luaL_optinteger(state, 4, 255));
    const int g = static_cast<int>(luaL_optinteger(state, 5, 255));
    const int b = static_cast<int>(luaL_optinteger(state, 6, 255));
    const int a = static_cast<int>(luaL_optinteger(state, 7, 255));
    const double scale = luaL_optnumber(state, 8, 1.0);
    if (text == nullptr) {
        lua_pushboolean(state, 0);
        return 1;
    }
    // Limite defensivo: o helper nativo desenha caractere a caractere e uma
    // string enorme por frame encheria a display list.
    std::string line(text);
    if (line.size() > 128) {
        line.resize(128);
    }
    const auto clamp8 = [](int v) { return static_cast<uint16_t>(std::clamp(v, 0, 255)); };
    // O caminho nativo de texto carrega as próprias texturas de fonte e mexe
    // no tile, então o próximo retângulo precisa recarregar a de preenchimento.
    // Cada caractere também consome display list — conta no orçamento.
    if (!HudTakeBudget(static_cast<int>(line.size()))) {
        lua_pushboolean(state, 0);
        return 1;
    }
    gHudTileDirty = true;
    Interface_DrawTextLine(play->state.gfxCtx, line.data(), static_cast<int16_t>(x), static_cast<int16_t>(y),
                           clamp8(r), clamp8(g), clamp8(b), clamp8(a),
                           static_cast<float>(std::clamp(scale, 0.1, 4.0)), 1);
    lua_pushboolean(state, 1);
    return 1;
}

extern "C" void DrawHudOverlay(PlayState* play) {
    if (gModHost == nullptr) {
        return;
    }
    // Só durante jogo de verdade. Checar apenas "existe Player" NÃO basta: a
    // tela de título carrega uma cena real, com Link nela, então o HUD do mod
    // aparecia por cima do logo. gameMode é o discriminador que o próprio
    // engine usa (z_play.c) e cobre título, file select e créditos.
    if (play == nullptr || GET_PLAYER(play) == nullptr || gSaveContext.gameMode != GAMEMODE_NORMAL) {
        return;
    }
    // Orçamento e estado de tile são por frame.
    gHudRectsThisFrame = 0;
    gHudTileDirty = true;
    HudBeginOverlay(play);
    gHudDrawActive = true;
    DispatchHookEvent("hook.oot.hud.draw", ShipLua::EventPayload{});
    gHudDrawActive = false;
}

extern "C" void DrawHeldItemModels(PlayState* play) {
    if (gHeldItemModelLeft.empty() && gHeldItemModelRight.empty()) {
        return;
    }
    Player* player = GET_PLAYER(play);
    if (player == nullptr) {
        return;
    }
    OPEN_DISPS(play->state.gfxCtx);
    if (!gHeldItemModelLeft.empty()) {
        Matrix_Translate(player->bodyPartsPos[PLAYER_BODYPART_L_HAND].x,
                         player->bodyPartsPos[PLAYER_BODYPART_L_HAND].y,
                         player->bodyPartsPos[PLAYER_BODYPART_L_HAND].z, MTXMODE_NEW);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                 G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gHeldItemModelLeft.c_str());
    }
    if (!gHeldItemModelRight.empty()) {
        Matrix_Translate(player->bodyPartsPos[PLAYER_BODYPART_R_HAND].x,
                         player->bodyPartsPos[PLAYER_BODYPART_R_HAND].y,
                         player->bodyPartsPos[PLAYER_BODYPART_R_HAND].z, MTXMODE_NEW);
        gSPMatrix(POLY_OPA_DISP++, Matrix_NewMtx(play->state.gfxCtx, (char*)__FILE__, __LINE__),
                 G_MTX_MODELVIEW | G_MTX_LOAD);
        gSPDisplayList(POLY_OPA_DISP++, (Gfx*)gHeldItemModelRight.c_str());
    }
    CLOSE_DISPS(play->state.gfxCtx);
}

// Mesmo passe Xlu de tela inteira usado pela maquina de transformacao de
// OoTMM. Roda antes do HUD, para que os botoes/hearts nunca sejam apagados.
extern "C" void DrawMaskTransitionFlash(PlayState* play) {
    if (play == nullptr || gMaskTransitionFlashAlpha <= 0) {
        return;
    }
    OPEN_DISPS(play->state.gfxCtx);
    Gfx_SetupDL_44Xlu(play->state.gfxCtx);
    gDPPipeSync(POLY_XLU_DISP++);
    gDPSetCombineLERP(POLY_XLU_DISP++, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0, PRIMITIVE, 0, 0, 0,
                      PRIMITIVE);
    gDPSetPrimColor(POLY_XLU_DISP++, 0, 0, 220, 220, 220, static_cast<u8>(gMaskTransitionFlashAlpha));
    gDPFillRectangle(POLY_XLU_DISP++, 0, 0, SCREEN_WIDTH - 1, SCREEN_HEIGHT - 1);
    gDPPipeSync(POLY_XLU_DISP++);
    CLOSE_DISPS(play->state.gfxCtx);
}

int LuaSetHeldItemModel(lua_State* state) {
    const char* slot = luaL_checkstring(state, 1);
    const char* path = luaL_optstring(state, 2, nullptr);
    std::string* target = nullptr;
    if (std::strcmp(slot, "left_hand") == 0) {
        target = &gHeldItemModelLeft;
    } else if (std::strcmp(slot, "right_hand") == 0) {
        target = &gHeldItemModelRight;
    } else {
        SPDLOG_WARN("ShipLua set_held_item_model: slot '{}' n\xC3\xA3o suportado (use 'left_hand'/'right_hand')",
                    slot);
        lua_pushboolean(state, 0);
        return 1;
    }

    if (path == nullptr || *path == '\0') {
        target->clear();
        lua_pushboolean(state, 1);
        return 1;
    }
    if (!ResourceMgr_FileExists(path)) {
        SPDLOG_WARN("ShipLua set_held_item_model: asset '{}' n\xC3\xA3o encontrado", path);
        lua_pushboolean(state, 0);
        return 1;
    }
    *target = std::string("__OTR__") + path;
    SPDLOG_INFO("ShipLua set_held_item_model: '{}' anexado ao slot '{}'", path, slot);
    lua_pushboolean(state, 1);
    return 1;
}

// PASSO 3 — ship.player.get/set: acesso a campos do player por nome, com
// validação, em vez de uma função nativa dedicada por ideia.
enum class FieldKind {
    Health, HealthCapacity, Magic, Rupees, PosX, PosY, PosZ, RotY, Speed, VelX, VelY, VelZ, OnGround,
    // Estados de ação, somente leitura. Semânticos de propósito: expor
    // stateFlags cru amarraria os mods a bits internos do engine.
    Rolling, Climbing, Swimming,
    // Posição do jogador projetada na tela — permite ancorar um medidor ao
    // personagem (roda de stamina ao estilo BotW) em vez de fixá-lo num canto.
    ScreenX, ScreenY
};

struct PlayerField {
    const char* name;
    FieldKind kind;
    double min;
    double max;
    bool writable;
};

// Array C cru de propósito: com std::array<N> era preciso manter o N à mão, e
// esquecer disso ao acrescentar um campo dá um erro de "muitos inicializadores"
// que não aponta para a linha do campo novo. std::begin/std::end abaixo tiram
// o tamanho do próprio literal.
constexpr PlayerField kPlayerFields[] = {
    { "health", FieldKind::Health, 0, 20 * 16, true },
    { "health_capacity", FieldKind::HealthCapacity, 16, 20 * 16, true },
    { "magic", FieldKind::Magic, 0, 96, true },
    { "rupees", FieldKind::Rupees, 0, 999, true },
    { "pos_x", FieldKind::PosX, -100000, 100000, true },
    { "pos_y", FieldKind::PosY, -100000, 100000, true },
    { "pos_z", FieldKind::PosZ, -100000, 100000, true },
    { "rot_y", FieldKind::RotY, -32768, 32767, true },
    { "speed", FieldKind::Speed, -50, 50, true },
    // vel_x/y/z + on_ground: física real do actor (Vec3f velocity, bgCheckFlags
    // bit 0). Faltava a componente vertical — "speed"/linearVelocity é só o
    // escalar planar. Com isto + game.frame (já existente), um mod implementa
    // pulo duplo/planador/air-dash inteiro em Lua, sem hook nativo dedicado.
    { "vel_x", FieldKind::VelX, -100, 100, true },
    { "vel_y", FieldKind::VelY, -100, 100, true },
    { "vel_z", FieldKind::VelZ, -100, 100, true },
    { "on_ground", FieldKind::OnGround, 0, 1, false },
    // Estados de ação (0/1, somente leitura). Permitem a um mod cobrar custo
    // por esforço — rolar, escalar, nadar — sem inferir da velocidade.
    { "rolling", FieldKind::Rolling, 0, 1, false },
    // climbing é gravável: escrever 0 solta o jogador da escada e o faz cair
    // (um mod de stamina precisa disso para "ficou sem força no meio da subida").
    { "climbing", FieldKind::Climbing, 0, 1, true },
    { "swimming", FieldKind::Swimming, 0, 1, false },
    // Coordenadas de tela do jogador (só leitura). Podem sair da tela quando a
    // câmera não o enquadra — o mod decide se desenha mesmo assim.
    { "screen_x", FieldKind::ScreenX, -10000, 10000, false },
    { "screen_y", FieldKind::ScreenY, -10000, 10000, false },
};

const PlayerField* FindPlayerField(const char* name) {
    const auto found = std::find_if(std::begin(kPlayerFields), std::end(kPlayerFields),
                                    [name](const PlayerField& f) { return std::strcmp(f.name, name) == 0; });
    return found == std::end(kPlayerFields) ? nullptr : &*found;
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
        case FieldKind::VelX:
            return player->actor.velocity.x;
        case FieldKind::VelY:
            return player->actor.velocity.y;
        case FieldKind::VelZ:
            return player->actor.velocity.z;
        case FieldKind::OnGround:
            return (player->actor.bgCheckFlags & 1) ? 1.0 : 0.0;
        case FieldKind::Rolling:
            // Comparar a actionFunc é o teste confiável — não há flag de estado
            // para rolamento. Mesmo critério que o corpo customizado usa.
            return player->actionFunc == Player_Action_Roll ? 1.0 : 0.0;
        case FieldKind::Climbing:
            return (player->stateFlags1 & (PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_CLIMBING_LEDGE)) ? 1.0 : 0.0;
        case FieldKind::Swimming:
            return (player->stateFlags1 & PLAYER_STATE1_IN_WATER) ? 1.0 : 0.0;
        case FieldKind::ScreenX:
        case FieldKind::ScreenY: {
            if (gPlayState == nullptr) {
                return 0.0;
            }
            s16 sx = 0;
            s16 sy = 0;
            // Projeta focus.pos (a cabeça), não world.pos — é o ponto que o
            // próprio engine usa para ancorar UI ao ator.
            Actor_GetScreenPos(gPlayState, &player->actor, &sx, &sy);
            return field.kind == FieldKind::ScreenX ? static_cast<double>(sx) : static_cast<double>(sy);
        }
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
        case FieldKind::VelX:
            player->actor.velocity.x = static_cast<float>(value);
            break;
        case FieldKind::VelY:
            player->actor.velocity.y = static_cast<float>(value);
            break;
        case FieldKind::VelZ:
            player->actor.velocity.z = static_cast<float>(value);
            break;
        case FieldKind::Climbing:
            // Só faz sentido soltar (0); escrever 1 não gruda o jogador numa
            // escada do nada, então é ignorado.
            if (value < 0.5 && (player->stateFlags1 & PLAYER_STATE1_CLIMBING_LADDER) && gPlayState != nullptr) {
                func_8083FB7C(player, gPlayState);
            }
            break;
        case FieldKind::OnGround:
        case FieldKind::Rolling:
        case FieldKind::Swimming:
        case FieldKind::ScreenX:
        case FieldKind::ScreenY:
            break; // somente leitura
    }
}

// Slot genérico para qualquer estado que um item customizado precise (timer,
// contador, flag) sem exigir um campo novo na struct nativa do Player a cada
// mod novo — achado como necessidade real cruzando dois forks independentes
// de sistemas de item customizado (cada um "colou" ~10 campos ad-hoc na
// struct Player nativa; aqui isso vira uma chave string qualquer). Não é
// salvo — vive só durante a sessão, mesma ressalva do storage mock.
std::map<std::string, double> gPlayerScratch;

int LuaPlayerGet(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    const PlayerField* field = FindPlayerField(name);
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (field != nullptr) {
        if (player == nullptr) {
            lua_pushnil(state);
            return 1;
        }
        lua_pushnumber(state, ReadPlayerField(*field, player));
        return 1;
    }
    const auto scratch = gPlayerScratch.find(name);
    if (scratch != gPlayerScratch.end()) {
        lua_pushnumber(state, scratch->second);
    } else {
        lua_pushnil(state);
    }
    return 1;
}

int LuaPlayerSet(lua_State* state) {
    const char* name = luaL_checkstring(state, 1);
    const double value = luaL_checknumber(state, 2);
    if (!std::isfinite(value)) {
        SPDLOG_WARN("ShipLua player.set: valor inv\xC3\xA1lido para '{}'", name);
        lua_pushboolean(state, 0);
        return 1;
    }
    const PlayerField* field = FindPlayerField(name);
    if (field != nullptr) {
        if (!field->writable || value < field->min || value > field->max) {
            SPDLOG_WARN("ShipLua player.set: valor fora da faixa para '{}'", name);
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
    // Chave desconhecida na tabela curada: cai no slot genérico — qualquer
    // string serve, nenhum campo nativo precisa existir para isso.
    gPlayerScratch[name] = value;
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
    lua_pushcfunction(state, LuaPlayMaskOnAnimation);
    lua_setfield(state, -2, "play_mask_on_animation");
    lua_pushcfunction(state, LuaSetPlayerBody);
    lua_setfield(state, -2, "set_body");
    lua_pushcfunction(state, LuaGetPlayerBody);
    lua_setfield(state, -2, "get_body");
    lua_pushcfunction(state, LuaPlayPlayerBodyAnimation);
    lua_setfield(state, -2, "play_body_animation");
    lua_pushcfunction(state, LuaSetPlayerBodySegment);
    lua_setfield(state, -2, "set_body_segment");
    lua_pushcfunction(state, LuaAttachModel);
    lua_setfield(state, -2, "attach_model");
    lua_pushcfunction(state, LuaSetHeldItemModel);
    lua_setfield(state, -2, "set_held_item_model");
    lua_pushcfunction(state, LuaSetDamageImmunity);
    lua_setfield(state, -2, "set_damage_immunity");
    lua_pushcfunction(state, LuaSetWeight);
    lua_setfield(state, -2, "set_weight");
    lua_pushcfunction(state, LuaSetRollMode);
    lua_setfield(state, -2, "set_roll_mode");
    lua_pushcfunction(state, LuaSetRollBlocked);
    lua_setfield(state, -2, "set_roll_blocked");
    lua_setfield(state, ootTable, "player");

    // ship.oot.env: estado do ambiente (hora do dia, cena). Separado de
    // ship.oot.player porque não é propriedade do jogador.
    lua_newtable(state);
    lua_pushcfunction(state, LuaEnvGet);
    lua_setfield(state, -2, "get");
    lua_setfield(state, ootTable, "env");

    // ship.oot.cutscene: assume a câmera por N frames. Genérica — serve para
    // transformação, item dramático, revelação de porta, o que o mod quiser.
    lua_newtable(state);
    lua_pushcfunction(state, LuaCutsceneStart);
    lua_setfield(state, -2, "start");
    lua_pushcfunction(state, LuaCutsceneStop);
    lua_setfield(state, -2, "stop");
    lua_pushcfunction(state, LuaCutsceneActive);
    lua_setfield(state, -2, "is_active");
    lua_setfield(state, ootTable, "cutscene");

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

    // ship.hud: desenho genérico em overlay, válido só dentro de
    // hook.oot.hud.draw. Comum aos dois jogos por design (o MM ganha o mesmo).
    lua_newtable(state);
    lua_pushcfunction(state, LuaHudDrawRect);
    lua_setfield(state, -2, "draw_rect");
    lua_pushcfunction(state, LuaHudDrawText);
    lua_setfield(state, -2, "draw_text");
    lua_pushcfunction(state, LuaHudDrawRing);
    lua_setfield(state, -2, "draw_ring");
    lua_setfield(state, shipTable, "hud");

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
    gTimers = std::make_shared<ShipLua::FrameTimerScheduler>();
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
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnGameFrameUpdate>([]() {
            TickWorldImport();
            // Gatilho temporário da Fase 1 do port de áudio (OOT-AUDIO-001):
            // toca uma amostra crua do mm.o2r para provar o caminho até o
            // alto-falante. Sai daqui quando as primitivas Lua de áudio
            // existirem (Fase 4).
            //
            // Dispara sozinho ~2s depois de entrar em gameplay, e repete a cada
            // D-pad esquerda. O automático existe porque um gatilho só por botão
            // não distingue "áudio falhou" de "o botão não chegou aqui".
            // Sem filtro de gameMode: áudio toca no título também, e amarrar o
            // teste a "estar em gameplay" só acrescenta uma variável a errar.
            if (gPlayState != nullptr) {
                static int sFrames = 0;
                static bool sAutoFired = false;
                sFrames++;

                // Batimento limitado: se o disparo não acontecer, estas linhas
                // dizem por quê, em vez de deixar o log mudo.
                if (sFrames % 60 == 0 && sFrames <= 300) {
                    SPDLOG_INFO("ShipLua/mmaudio: aguardando disparo — frame {} gameMode={} disparado={}", sFrames,
                                static_cast<int>(gSaveContext.gameMode), sAutoFired);
                }

                const bool autoNow = !sAutoFired && sFrames > 120;
                const bool pressed = CHECK_BTN_ALL(gPlayState->state.input[0].press.button, BTN_DLEFT);

                if (autoNow || pressed) {
                    sAutoFired = true;
                    SPDLOG_INFO("ShipLua/mmaudio: disparo de teste ({}) no frame {} gameMode={}",
                                autoNow ? "autom\xC3\xA1tico" : "D-pad esquerda", sFrames,
                                static_cast<int>(gSaveContext.gameMode));
                    ShipLua::MmAudio_PlaySampleOneShot("mm/audio/samples/AdultLinkAttack1_META");
                }
            }
            // Avança a cutscene AQUI, não no update do Player: com atores
            // congelados (freeze_player) o hook do Player não dispara, e a
            // câmera ficaria parada e sem nunca ser devolvida.
            if (gPlayState != nullptr) {
                CutsceneUpdate(gPlayState);
            }
            // Avança os timers de mod uma vez por frame. Sem isto, ship.timer
            // nunca dispara e qualquer mod que sequencie ações (animação e
            // depois efeito) trava no primeiro passo.
            if (gTimers != nullptr) {
                const auto ticked = gTimers->Tick();
                if (ticked.isOk()) {
                    for (const auto& failure : ticked.value->failures) {
                        SPDLOG_WARN("ShipLua [{}] timer falhou: {}", failure.modId, failure.message);
                    }
                }
            }
            // Grava o storage no máximo a cada ~5s, e só se algo mudou. Gravar
            // a cada ship.storage.set serializava o store inteiro e fazia um
            // write+rename síncronos na thread do jogo — um mod que atualize
            // contadores por segundo travava visivelmente, ainda mais com um
            // antivírus segurando o arquivo (o rename tenta de novo por dezenas
            // de ms). Flush() é no-op barato quando nada mudou.
            static int sFlushCountdown = 0;
            if (--sFlushCountdown <= 0) {
                sFlushCountdown = 300; // ~5s a 60fps
                if (gStorage != nullptr && gStorage->IsDirty()) {
                    const auto flushed = gStorage->Flush();
                    if (!flushed.isOk()) {
                        SPDLOG_WARN("ShipLua: falha ao gravar o storage: {}", flushed.message);
                    }
                }
                if (gSharedStorage != nullptr && gSharedStorage->IsDirty()) {
                    const auto flushed = gSharedStorage->Flush();
                    if (!flushed.isOk()) {
                        SPDLOG_WARN("ShipLua: falha ao gravar o storage compartilhado: {}", flushed.message);
                    }
                }
            }
        });
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
    // - A enquanto corre inicia explicitamente o primeiro rolamento;
    // - o rolamento se re-arma sozinho enquanto houver direção no analógico
    //   (no MM não se aperta nada para continuar rolando);
    // - a direção acompanha o analógico de verdade;
    // - bater na parede continua caindo no "bonk" do próprio OoT, que roda
    //   antes deste ponto — é o "até bater em algo".
    // O primeiro hook usa a ação vanilla Player_Action_Roll; os demais fazem
    // o chain chamar Player_SetupRoll e o steer girar o player ele mesmo.
    gRollStartHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!gChainRoll || gPlayState == nullptr) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        Input* input = &gPlayState->state.input[0];
        if (player == nullptr || input == nullptr || player->actionFunc == Player_Action_Roll) {
            return;
        }
        // Mantém a semântica do OoT: A só vira rolamento enquanto o jogador
        // está se deslocando no chão. Assim interações paradas continuam sendo
        // tratadas pelo jogo em vez de serem sequestradas pelo mod.
        if (!(input->press.button & BTN_A) || !(player->actor.bgCheckFlags & BGCHECKFLAG_GROUND) ||
            fabsf(player->linearVelocity) < 0.5f || (player->stateFlags1 & (PLAYER_STATE1_DEAD | PLAYER_STATE1_IN_CUTSCENE))) {
            return;
        }
        Player_SetupRoll(player, gPlayState);
    });
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
    gHookBonkHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerBonk>([]() {
        if (gPlayState != nullptr) {
            CustomBodyHandleRollWallBounce(GET_PLAYER(gPlayState), gPlayState);
        }
        DispatchHookEvent("hook.oot.player.bonk", ShipLua::EventPayload{});
    });
    // hook.oot.player.first_person_control — OnPlayerFirstPersonControl(Player*)
    // já existia (usado por Mouse.cpp), roda todo frame com mira em primeira
    // pessoa ativa. Achado minerando forks de item customizado: praticamente
    // todo item com overlay/comportamento de mira (Ball and Chain, Whip,
    // Gust Jar...) precisa saber "estou mirando agora" por frame.
    gHookFirstPersonHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerFirstPersonControl>([](Player* player) {
            if (player == nullptr) {
                return;
            }
            DispatchHookEvent("hook.oot.player.first_person_control",
                              ShipLua::EventPayload{
                                  {"held_item_action", static_cast<std::int64_t>(player->heldItemAction)},
                              });
        });
    // hook.oot.player.arrow_type_select — VB_PLAYER_ARROW_MAGIC_CONSUMPTION
    // (Player*, s32 magicArrowType, s32* arrowType), já mutava arrowType por
    // referência antes de nós (é o próprio mecanismo vanilla de escolher
    // flecha de fogo/gelo/luz). Reaproveitado como ponto de escolha de
    // variante de projétil para mods, sem novo call site nativo.
    gHookArrowTypeSelectHook = REGISTER_VB_SHOULD(VB_PLAYER_ARROW_MAGIC_CONSUMPTION, {
        va_arg(args, Player*);
        int32_t magicArrowType = va_arg(args, int32_t);
        int32_t* arrowType = va_arg(args, int32_t*);
        if (arrowType == nullptr) {
            return;
        }
        const auto result = DispatchHookTransform(
            "hook.oot.player.arrow_type_select",
            ShipLua::EventPayload{
                {"magic_arrow_type", static_cast<std::int64_t>(magicArrowType)},
                {"arrow_type", static_cast<std::int64_t>(*arrowType)},
            });
        if (result.has_value() && std::holds_alternative<std::int64_t>(result->value)) {
            *arrowType = static_cast<int32_t>(std::get<std::int64_t>(result->value));
        }
    });
    // hook.oot.enemy.defeat — OnEnemyDefeat(Actor*), disparado por dezenas de
    // inimigos ao morrer (não é todo Actor_Kill; só derrota real). Fonte de XP
    // para um mod de RPG: o Lua consulta a tabela de recompensa dele pelo
    // actor_id e persiste o total via ship.storage.
    gHookEnemyDefeatHook =
        GameInteractor::Instance->RegisterGameHook<GameInteractor::OnEnemyDefeat>([](void* actorPtr) {
            Actor* actor = static_cast<Actor*>(actorPtr);
            if (actor == nullptr) {
                return;
            }
            DispatchHookEvent("hook.oot.enemy.defeat",
                              ShipLua::EventPayload{
                                  {"actor_id", static_cast<std::int64_t>(actor->id)},
                                  {"category", static_cast<std::int64_t>(actor->category)},
                                  {"pos_x", static_cast<double>(actor->world.pos.x)},
                                  {"pos_y", static_cast<double>(actor->world.pos.y)},
                                  {"pos_z", static_cast<double>(actor->world.pos.z)},
                              });
        });
    // OnPlayDrawEnd roda após o desenho do mundo (Player incluído) e antes do
    // HUD — bodyPartsPos já está resolvido para o frame atual nesse ponto.
    gHeldItemDrawHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayDrawEnd>(
        []() {
            if (gPlayState != nullptr) {
                DrawHeldItemModels(gPlayState);
                DrawMaskTransitionFlash(gPlayState);
                // Por último: o overlay do mod fica por cima de tudo.
                DrawHudOverlay(gPlayState);
            }
        });

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
        CustomBodyResetForSceneChange();
        if (gActorProvider == nullptr) {
            return;
        }
        const auto cleaned = gActorProvider->OnSceneChange();
        if (!cleaned.isOk()) {
            SPDLOG_ERROR("ShipLua failed to clean OoT actors during scene teardown: {}", cleaned.message);
        }
    });
    // Religa o corpo customizado sozinho na cena nova, se ele estava ativo na
    // cena anterior — resolve a limitação conhecida da v1 (corpo sumia em
    // toda troca de cena e exigia religar manualmente). OnPlayerUpdate (não
    // OnSceneInit) de propósito: scene init roda antes dos atores da sala
    // (Player incluído) serem spawnados — GET_PLAYER ainda seria inválido
    // ali. OnPlayerUpdate só dispara quando o Player já existe de verdade,
    // mesmo padrão já usado pelo hook de imunidade a fogo acima.
    gCustomBodyRestoreHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (!gCustomBodyPendingRestore || gPlayState == nullptr) {
            return;
        }
        if (CustomBodyActivateFromSpec(gPlayState)) {
            gCustomBodyPendingRestore = false;
        }
    });
    gMaskTransitionUpdateHook = GameInteractor::Instance->RegisterGameHook<GameInteractor::OnPlayerUpdate>([]() {
        if (gPlayState == nullptr || gMaskTransitionPhase == MaskTransitionPhase::None) {
            return;
        }
        Player* player = GET_PLAYER(gPlayState);
        if (player != nullptr) {
            MaskTransitionUpdate(player);
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
    // Diagnóstico da Fase 1 do port de áudio do MM (handoff OOT-AUDIO-001).
    // Só escreve no log; sai daqui assim que a Fase 1 fechar.
    ShipLua::ProbeMmSoundFonts();
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
    if (gCustomBodyRestoreHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(gCustomBodyRestoreHook);
        gCustomBodyRestoreHook = 0;
    }
    if (gMaskTransitionUpdateHook != 0) {
        GameInteractor::Instance->UnregisterGameHook<GameInteractor::OnPlayerUpdate>(gMaskTransitionUpdateHook);
        gMaskTransitionUpdateHook = 0;
    }
    // Última chance de gravar: a persistência é adiada, então sem isto os
    // segundos finais de jogo se perderiam ao fechar.
    for (const auto& store : { gStorage, gSharedStorage }) {
        if (store != nullptr && store->IsDirty()) {
            const auto flushed = store->Flush();
            if (!flushed.isOk()) {
                SPDLOG_WARN("ShipLua: falha ao gravar o storage no shutdown: {}", flushed.message);
            }
        }
    }
    gStorage.reset();
    gSharedStorage.reset();
    gActorProvider.reset();
    gCapabilityRegistry.reset();
    gWorldAdapter.reset();
    gHotkeys.reset();
    gTimers.reset();
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

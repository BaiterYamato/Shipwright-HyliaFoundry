#include "ShipLuaBootstrap.h"
#include "OotActorProvider.h"
#include "OotHotkeyRegistry.h"
#include "OotWorldAdapter.h"
#include "ShipLuaPuppet.h"

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

#include <ship/Context.h>
#include <ship/resource/ResourceManager.h>
#include <ship/resource/archive/ArchiveManager.h>
#include <ship/resource/archive/O2rArchive.h>
#include <shiplua/generated/ApiBindings.h>
#include <shiplua/host/ModHost.h>
#include <shiplua/runtime/LuaRuntime.h>
#include <shiplua/storage/AtomicFile.h>
#include <shiplua/world/WorldHandoff.h>

#include "soh/Enhancements/game-interactor/GameInteractor.h"

extern "C" {
#include "functions.h"
#include "lauxlib.h"
#include "lua.h"
#include "macros.h"
#include "variables.h"
#include "z64.h"
extern PlayState* gPlayState;
}

namespace ShipLuaHost {
namespace {

std::unique_ptr<ShipLua::ModHost> gModHost;
std::shared_ptr<OotActorProvider> gActorProvider;
std::shared_ptr<ShipLua::CapabilityRegistry> gCapabilityRegistry;
std::shared_ptr<OotHotkeyRegistry> gHotkeys;
std::shared_ptr<OotWorldAdapter> gWorldAdapter;
HOOK_ID gLoadGameHook = 0;
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
    const auto prepared = gWorldAdapter->PrepareImport(handoff.value->player, handoff.value->destination);
    if (!prepared.isOk()) {
        SPDLOG_ERROR("Link-Span não preparou a importação OoT: {}", prepared.message);
        return;
    }
    const auto committed = gWorldAdapter->CommitImport();
    if (!committed.isOk()) {
        gWorldAdapter->AbortImport();
        SPDLOG_ERROR("Link-Span não confirmou a importação OoT: {}", committed.message);
        return;
    }
    std::error_code error;
    std::filesystem::remove(config->handoffPath, error);
    SPDLOG_INFO("Link-Span importou o estado compartilhado em OoT ({})", handoff.value->destination.id);
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
    context.capabilities = { "oot.player.jump", "oot.spawn_dog" };
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
    lua_setfield(state, ootTable, "player");
    lua_setfield(state, shipTable, "oot");
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
    MmCrossWorldArchive(const std::string& path, Ship::ArchiveManager* manager)
        : Ship::Archive(path), mInner(std::make_shared<Ship::O2rArchive>(path)), mManager(manager) {
    }

    bool Open() override {
        mInner->Load();
        if (!mInner->IsLoaded()) {
            return false;
        }
        std::size_t aliased = 0;
        std::size_t blocked = 0;
        const auto files = mInner->ListFiles();
        for (const auto& [hash, filePath] : *files) {
            IndexFile(kMmNamespace + filePath);
            if (mManager != nullptr && !mManager->HasFile(filePath)) {
                IndexFile(filePath);
                ++aliased;
            } else {
                ++blocked;
            }
        }
        mOwnIndex = ListFiles();
        SPDLOG_INFO("ShipLua exp\xC3\xB4s {} assets do MM sob 'mm/' ({} com alias direto, {} bloqueados por "
                    "colis\xC3\xA3o de caminho)",
                    files->size(), aliased, blocked);
        return !files->empty();
    }

    bool Close() override {
        mOwnIndex.reset();
        mInner->Unload();
        return true;
    }

    std::shared_ptr<Ship::File> LoadFile(const std::string& filePath) override {
        if (!HasFile(filePath)) {
            return nullptr;
        }
        if (filePath.rfind(kMmNamespace, 0) == 0) {
            return mInner->LoadFile(filePath.substr(std::char_traits<char>::length(kMmNamespace)));
        }
        return mInner->LoadFile(filePath);
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

    std::filesystem::path siblingRoot;
    if (const char* configured = std::getenv("SHIPLUA_MM_ROOT"); configured != nullptr && *configured != '\0') {
        siblingRoot = configured;
    } else {
        std::error_code ec;
        siblingRoot = std::filesystem::absolute(Ship::Context::GetAppDirectoryPath(""), ec);
        siblingRoot = siblingRoot.parent_path() / "MM";
    }

    const std::filesystem::path mmArchive = siblingRoot / "mm.o2r";
    std::error_code ec;
    if (!std::filesystem::is_regular_file(mmArchive, ec)) {
        SPDLOG_INFO("ShipLua: mm.o2r n\xC3\xA3o encontrado em '{}' — assets do MM indispon\xC3\xADveis no OOT",
                    siblingRoot.string());
        return;
    }

    if (archiveManager->AddArchive(std::make_shared<MmCrossWorldArchive>(mmArchive.string(), archiveManager.get())) !=
        nullptr) {
        SPDLOG_INFO("ShipLua montou o mm.o2r do MM em modo cross-world: {}", mmArchive.string());
    } else {
        SPDLOG_WARN("ShipLua n\xC3\xA3o conseguiu montar '{}'", mmArchive.string());
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

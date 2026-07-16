#include "ShipLuaBootstrap.h"
#include "OotHotkeyRegistry.h"
#include "OotWorldAdapter.h"

#include <filesystem>
#include <cstdlib>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include <ship/Context.h>
#include <shiplua/generated/ApiBindings.h>
#include <shiplua/host/ModHost.h>
#include <shiplua/runtime/LuaRuntime.h>

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
std::shared_ptr<OotHotkeyRegistry> gHotkeys;
std::shared_ptr<OotWorldAdapter> gWorldAdapter;

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

ShipLua::LuaApiHostContext CreateHostContext() {
    ShipLua::LuaApiHostContext context;
    context.gameId = "oot";
    context.hostVersion = GetHostVersion();
    context.capabilities = { "oot.player.jump", "oot.spawn_dog" };
    context.hotkeys = gHotkeys;
    if (const char* available = std::getenv("LINKSPAN_AVAILABLE_GAMES"); available != nullptr) {
        const std::string games(available);
        if (games.find("oot") != std::string::npos) {
            context.availableGames.push_back("oot");
        }
        if (games.find("mm") != std::string::npos) {
            context.availableGames.push_back("mm");
        }
    }
    return context;
}

int LuaPlayerJump(lua_State* state) {
    PlayState* play = gPlayState;
    Player* player = play != nullptr ? GET_PLAYER(play) : nullptr;
    if (player == nullptr || (player->stateFlags1 & PLAYER_STATE1_DEAD) != 0 ||
        (player->actor.bgCheckFlags & 1) == 0) {
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
    if (player == nullptr ||
        (play->sceneNum != SCENE_MARKET_DAY && play->sceneNum != SCENE_MARKET_NIGHT) ||
        Object_GetIndex(&play->objectCtx, OBJECT_DOG) < 0) {
        lua_pushboolean(state, 0);
        return 1;
    }
    Actor* dog = Actor_Spawn(&play->actorCtx, play, ACTOR_EN_DOG,
                             player->actor.world.pos.x, player->actor.world.pos.y,
                             player->actor.world.pos.z, 0, player->actor.shape.rot.y,
                             0, static_cast<s16>(0x8000), true);
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
    auto catalog = ShipLua::PortableItemCatalog::CreateDefault();
    if (!catalog.isOk()) {
        SPDLOG_ERROR("ShipLua não conseguiu criar o catálogo portátil OoT: {}", catalog.message);
        gHotkeys.reset();
        return;
    }
    gWorldAdapter = std::make_shared<OotWorldAdapter>(std::move(*catalog.value));
    ShipLua::LuaApiHostContext context = CreateHostContext();
    SPDLOG_INFO("ShipLua inicializando para {} {} (commit {})", context.gameId, context.hostVersion, gGitCommitHash);
    gModHost = std::make_unique<ShipLua::ModHost>(context, CreateLogger());
    LoadModsAndDispatchReady(context);
    SPDLOG_INFO("ShipLua inicializado");
}

void Shutdown() {
    if (gModHost == nullptr) {
        return;
    }

    gModHost.reset();
    gWorldAdapter.reset();
    gHotkeys.reset();
    SPDLOG_INFO("ShipLua finalizado");
}

ShipLua::ModHost* GetModHost() {
    return gModHost.get();
}

OotHotkeyRegistry* Hotkeys() {
    return gHotkeys.get();
}

OotWorldAdapter* WorldAdapter() {
    return gWorldAdapter.get();
}

} // namespace ShipLuaHost

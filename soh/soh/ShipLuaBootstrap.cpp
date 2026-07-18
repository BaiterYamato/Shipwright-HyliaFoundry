#include "ShipLuaBootstrap.h"
#include "OotHotkeyRegistry.h"
#include "OotWorldAdapter.h"

#include <filesystem>
#include <memory>
#include <string>

#include <spdlog/spdlog.h>

#include <ship/Context.h>
#include <shiplua/generated/ApiBindings.h>
#include <shiplua/host/ModHost.h>
#include <shiplua/runtime/LuaRuntime.h>

extern "C" {
#include <z64.h>
#include "variables.h"
#include "functions.h"
#include "macros.h"
extern PlayState* gPlayState;
#include "lauxlib.h"
#include "lua.h"
}

// OoT dog actor (En_Dog), defined in actor_table.h as 0x019B. The header is a
// macro-expansion template (DEFINE_ACTOR), so we reference the literal value
// rather than including it raw.
constexpr s16 kOotActorEnDog = 0x019B;

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
    return context;
}

// ship.oot.player.jump(): applies a host-controlled vertical impulse only when
// the player is alive and standing on the ground. Mirrors the MM binding at
// mm/2s2h/ShipLuaBootstrap.cpp. SoH does not define BGCHECKFLAG_GROUND, so we
// test the ground bit directly (idiom from z_player.c). MoonJump.cpp uses the
// same velocity literal (6.34375f), the canonical jump impulse for OoT/MM.
int LuaPlayerJump(lua_State* L) {
    PlayState* play = gPlayState;
    if (play == nullptr) {
        lua_pushboolean(L, 0);
        return 1;
    }

    Player* player = GET_PLAYER(play);
    if (player == nullptr || (player->stateFlags1 & PLAYER_STATE1_DEAD) != 0 ||
        (player->actor.bgCheckFlags & 1) == 0) {
        lua_pushboolean(L, 0);
        return 1;
    }

    player->actor.velocity.y = 6.34375f;
    lua_pushboolean(L, 1);
    return 1;
}

// ship.oot.spawn_dog(): spawns the OoT dog (En_Dog) at the player's position,
// returning true on success. En_Dog is the Hyrule Market / Dog Lady NPC; its
// init validates scene and params, so spawns outside Market scenes may not
// behave as expected. params 0x8000 sets the "second dog / follow" branch that
// avoids the Actor_Kill path taken by dogs without a valid scene path.
int LuaSpawnDog(lua_State* L) {
    PlayState* play = gPlayState;
    if (play == nullptr) {
        SPDLOG_WARN("ShipLua spawn_dog: gPlayState nulo (fora de gameplay)");
        lua_pushboolean(L, 0);
        return 1;
    }
    Player* player = GET_PLAYER(play);
    if (player == nullptr) {
        SPDLOG_WARN("ShipLua spawn_dog: player nulo");
        lua_pushboolean(L, 0);
        return 1;
    }

    SPDLOG_INFO("ShipLua spawn_dog: sceneNum={} em pos=({:.0f},{:.0f},{:.0f})", (int)play->sceneNum,
                player->actor.world.pos.x, player->actor.world.pos.y, player->actor.world.pos.z);

    // SoH's Actor_Spawn takes an extra trailing s16 canRandomize (absent in MM).
    Actor* dog = Actor_Spawn(&play->actorCtx, play, kOotActorEnDog, player->actor.world.pos.x,
                             player->actor.world.pos.y, player->actor.world.pos.z, 0,
                             player->actor.shape.rot.y, 0, (s16)0x8000, 0);
    SPDLOG_INFO("ShipLua spawn_dog: Actor_Spawn -> {}", dog != nullptr ? "ator criado" : "NULL");
    lua_pushboolean(L, dog != nullptr);
    return 1;
}

// Installs the OoT-specific ship.oot.* table onto a mod runtime. Uses
// require("ship") so it works regardless of how the core registers the module.
void InstallOotApi(lua_State* L) {
    if (L == nullptr) {
        return;
    }
    lua_getglobal(L, "require");
    lua_pushstring(L, "ship");
    if (lua_pcall(L, 1, 1, 0) != LUA_OK) {
        lua_pop(L, 1);
        return;
    }
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        return;
    }
    const int shipTable = lua_gettop(L);
    lua_getfield(L, shipTable, "oot");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }
    const int ootTable = lua_gettop(L);
    lua_pushcfunction(L, LuaSpawnDog);
    lua_setfield(L, -2, "spawn_dog");

    lua_getfield(L, ootTable, "player");
    if (!lua_istable(L, -1)) {
        lua_pop(L, 1);
        lua_newtable(L);
    }
    lua_pushcfunction(L, LuaPlayerJump);
    lua_setfield(L, -2, "jump");
    lua_setfield(L, ootTable, "player");

    lua_setfield(L, shipTable, "oot");
    lua_pop(L, 1);
}

void LoadModsAndDispatchReady(const ShipLua::LuaApiHostContext& context) {
    const std::shared_ptr<Ship::Context> shipContext = Ship::Context::GetInstance();
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

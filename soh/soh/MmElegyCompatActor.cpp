#include "ActorDB.h"
#include "OotActorProvider.h"
#include "ResourceManagerHelpers.h"

#include <cstdint>
#include <limits>
#include <optional>
#include <string>

extern "C" {
#include "functions.h"
#include "z64.h"
}

namespace ShipLuaHost {
namespace {

constexpr const char* kLogicalActorId = "compat.mm.elegy_shell.human";
constexpr const char* kNativeActorName = "En_MmElegyShellHuman";
constexpr const char* kElegyShellHumanResource = "__OTR__objects/gameplay_keep/gElegyShellHumanDL";

void MmElegyShellHuman_Init(Actor* actor, PlayState* play) {
    (void)actor;
    (void)play;
}

void MmElegyShellHuman_Destroy(Actor* actor, PlayState* play) {
    (void)actor;
    (void)play;
}

void MmElegyShellHuman_Update(Actor* actor, PlayState* play) {
    (void)actor;
    (void)play;
}

void MmElegyShellHuman_Draw(Actor* actor, PlayState* play) {
    (void)actor;
    Gfx* shell = ResourceMgr_LoadGfxByName(kElegyShellHumanResource);
    if (shell == nullptr) {
        return;
    }

    OPEN_DISPS(play->state.gfxCtx);

    Scene_SetRenderModeXlu(play, 0, 0x01);
    gDPSetEnvColor(POLY_OPA_DISP++, 255, 255, 255, 255);
    Gfx_DrawDListOpa(play, shell);

    CLOSE_DISPS(play->state.gfxCtx);
}

ShipLua::Result<void> ValidateElegyShellResources() {
    if (ResourceMgr_FileExists(kElegyShellHumanResource) == 0) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::InvalidState,
                                          "Majora's Mask Elegy shell asset is not mounted: " +
                                              std::string(kElegyShellHumanResource));
    }
    if (ResourceMgr_LoadGfxByName(kElegyShellHumanResource) == nullptr) {
        return ShipLua::Result<void>::err(ShipLua::ErrorCode::HostFailure,
                                          "Majora's Mask Elegy shell display list could not be loaded");
    }
    return ShipLua::Result<void>::ok();
}

std::optional<OotActorDefinition> CreateElegyShellDefinition() {
    if (ActorDB::Instance == nullptr) {
        return std::nullopt;
    }

    int actorId = ActorDB::Instance->RetrieveId(kNativeActorName);
    if (actorId < 0) {
        ActorDBInit init;
        init.name = kNativeActorName;
        init.desc = "Majora's Mask Elegy of Emptiness human shell compatibility actor";
        init.category = ACTORCAT_ITEMACTION;
        init.flags = ACTOR_FLAG_UPDATE_CULLING_DISABLED;
        init.objectId = OBJECT_GAMEPLAY_KEEP;
        init.instanceSize = sizeof(Actor);
        init.init = MmElegyShellHuman_Init;
        init.destroy = MmElegyShellHuman_Destroy;
        init.update = MmElegyShellHuman_Update;
        init.draw = MmElegyShellHuman_Draw;
        actorId = ActorDB::Instance->AddEntry(init).entry.id;
    }

    if (actorId < 0 || actorId > std::numeric_limits<std::int16_t>::max()) {
        return std::nullopt;
    }

    OotActorDefinition definition;
    definition.key = kLogicalActorId;
    definition.actorId = static_cast<std::int16_t>(actorId);
    definition.objectId = OBJECT_GAMEPLAY_KEEP;
    definition.params = 0;
    definition.preflight = ValidateElegyShellResources;
    return definition;
}

[[maybe_unused]] const bool kRegistered = []() {
    RegisterOotActorDefinitionFactory(CreateElegyShellDefinition);
    return true;
}();

} // namespace
} // namespace ShipLuaHost

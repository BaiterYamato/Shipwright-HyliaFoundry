#pragma once

#include <cstdint>
#include <functional>

#include "ExternalModTypes.h"

struct PlayState;

namespace SOH {

class ExternalModWorldPatchRuntime {
  public:
    using PatchActionRunner =
        std::function<void(const ExternalModWorldPatchsetDefinition&, const ExternalModWorldPatchOp&)>;

    // Returns true only when an enabled world.patchsets.v1 patchset for this exact scene contains a
    // suppressActor op matching the actor id (and params, when the op specifies them). Anything else
    // - including the player actor - keeps the permissive default (no suppression).
    bool ShouldSuppressActor(ExternalModPackage& package, int16_t actorId, int32_t sceneNum, int32_t params) const;

    // Applies spawnActor and actions ops of every patchset matching the freshly initialized scene
    // (and current room, when the patchset specifies one). spawnActor ops go through Actor_Spawn;
    // actions ops are forwarded to runActions so the manager executes them with its shared pipeline.
    // Never spawns ACTOR_PLAYER.
    void ApplyOnSceneInit(ExternalModPackage& package, ::PlayState* play, int32_t sceneNum,
                          const PatchActionRunner& runActions) const;
};

} // namespace SOH

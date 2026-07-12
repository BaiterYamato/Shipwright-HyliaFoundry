#include "ExternalModWorldPatchRuntime.h"

#include <spdlog/spdlog.h>

extern "C" {
#include <z64.h>
#include "functions.h"
}

namespace SOH {
namespace {

bool PatchsetMatchesScene(const ExternalModWorldPatchsetDefinition& patchset, int32_t sceneNum) {
    return patchset.sceneId == sceneNum;
}

bool PatchsetMatchesRoom(const ExternalModWorldPatchsetDefinition& patchset, ::PlayState* play) {
    if (!patchset.hasRoomId) {
        return true;
    }
    if (play == nullptr) {
        return false;
    }
    return patchset.roomId == static_cast<int32_t>(play->roomCtx.curRoom.num);
}

} // namespace

bool ExternalModWorldPatchRuntime::ShouldSuppressActor(ExternalModPackage& package, int16_t actorId, int32_t sceneNum,
                                                       int32_t params) const {
    if (actorId == ACTOR_PLAYER) {
        return false;
    }
    const auto& runtime = package.runtime;
    if (!runtime.enabled || runtime.worldPatchsets.empty()) {
        return false;
    }

    for (const auto& patchset : runtime.worldPatchsets) {
        if (!PatchsetMatchesScene(patchset, sceneNum)) {
            continue;
        }
        for (const auto& op : patchset.ops) {
            if (op.op != "suppressActor" || !op.hasActorId) {
                continue;
            }
            if (op.actorId != static_cast<int32_t>(actorId)) {
                continue;
            }
            if (op.hasParams && op.params != params) {
                continue;
            }
            return true;
        }
    }
    return false;
}

void ExternalModWorldPatchRuntime::ApplyOnSceneInit(ExternalModPackage& package, ::PlayState* play, int32_t sceneNum,
                                                    const PatchActionRunner& runActions) const {
    auto& runtime = package.runtime;
    if (!runtime.enabled || runtime.worldPatchsets.empty()) {
        return;
    }

    for (const auto& patchset : runtime.worldPatchsets) {
        if (!PatchsetMatchesScene(patchset, sceneNum) || !PatchsetMatchesRoom(patchset, play)) {
            continue;
        }
        for (const auto& op : patchset.ops) {
            if (op.op == "spawnActor") {
                if (!op.hasActorId || !op.hasPos || op.actorId == ACTOR_PLAYER || play == nullptr) {
                    continue;
                }
                Actor* spawned = Actor_Spawn(&play->actorCtx, play, static_cast<int16_t>(op.actorId), op.posX, op.posY,
                                             op.posZ, 0, op.hasRotY ? static_cast<int16_t>(op.rotY) : 0, 0,
                                             op.hasParams ? static_cast<int16_t>(op.params) : 0);
                if (spawned == nullptr) {
                    SPDLOG_WARN("[ExternalMods] world.patchsets.v1 patchset={} failed to spawn actorId={} in scene={}",
                                patchset.id, op.actorId, sceneNum);
                }
            } else if (op.op == "actions") {
                if (!op.actions.empty() && runActions) {
                    runActions(patchset, op);
                }
            }
            // suppressActor ops are consumed by ShouldSuppressActor via the ShouldActorInit hook.
        }
    }
}

} // namespace SOH

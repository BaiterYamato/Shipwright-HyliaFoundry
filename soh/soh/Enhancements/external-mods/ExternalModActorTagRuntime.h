#pragma once

#include <functional>
#include <vector>

#include "ExternalModTypes.h"

struct Actor;
struct PlayState;

namespace SOH {

class ExternalModActorTagRuntime {
  public:
    using TagActionRunner = std::function<void(const ExternalModActorTagDefinition&)>;

    // Matches the package's actors.tags.v1 definitions against a freshly initialized actor,
    // records the applied tags in package.runtime.taggedActorInstances, bumps the
    // __actor_tag.<tag> blackboard counters, and invokes runActions for each matched
    // definition with onTagged actions. Never tags the player actor.
    void ApplyOnActorInit(ExternalModPackage& package, ::Actor* actor, ::PlayState* play,
                          const TagActionRunner& runActions) const;

    // Drops per-actor tag records and __actor_tag.* blackboard counters for one package.
    void ClearForPackage(ExternalModPackage& package) const;

    // Drops per-actor tag state for every package (scene teardown / play destroy).
    void ClearAll(std::vector<ExternalModPackage>& packages) const;
};

} // namespace SOH

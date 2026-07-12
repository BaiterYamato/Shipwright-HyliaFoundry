#include "ExternalModActorTagRuntime.h"

#include <algorithm>
#include <cstdlib>
#include <string>

extern "C" {
#include <z64.h>
}

namespace SOH {
namespace {
constexpr const char* kActorTagBlackboardPrefix = "__actor_tag.";

void IncrementTagCounter(ExternalModRuntime& runtime, const std::string& tag) {
    const std::string key = std::string(kActorTagBlackboardPrefix) + tag;
    int32_t count = 0;
    const auto it = runtime.globalBlackboard.find(key);
    if (it != runtime.globalBlackboard.end()) {
        count = static_cast<int32_t>(std::strtol(it->second.c_str(), nullptr, 10));
    }
    runtime.globalBlackboard[key] = std::to_string(count + 1);
}
} // namespace

void ExternalModActorTagRuntime::ApplyOnActorInit(ExternalModPackage& package, ::Actor* actor, ::PlayState* play,
                                                  const TagActionRunner& runActions) const {
    if (actor == nullptr || actor->id == ACTOR_PLAYER) {
        return;
    }
    auto& runtime = package.runtime;
    if (runtime.actorTags.empty()) {
        return;
    }
    const int32_t sceneId = play != nullptr ? static_cast<int32_t>(play->sceneNum) : -1;

    for (const auto& definition : runtime.actorTags) {
        if (definition.actorId != static_cast<int32_t>(actor->id)) {
            continue;
        }
        if (definition.hasCategory && definition.category != static_cast<int32_t>(actor->category)) {
            continue;
        }
        if (definition.hasSceneId && definition.sceneId != sceneId) {
            continue;
        }
        if (definition.hasParams && definition.params != static_cast<int32_t>(actor->params)) {
            continue;
        }

        auto& appliedTags = runtime.taggedActorInstances[reinterpret_cast<uintptr_t>(actor)];
        for (const auto& tag : definition.tags) {
            if (std::find(appliedTags.begin(), appliedTags.end(), tag) != appliedTags.end()) {
                continue;
            }
            appliedTags.push_back(tag);
            IncrementTagCounter(runtime, tag);
        }

        if (!definition.onTagged.empty() && runActions) {
            runActions(definition);
        }
    }
}

void ExternalModActorTagRuntime::ClearForPackage(ExternalModPackage& package) const {
    package.runtime.taggedActorInstances.clear();
    std::erase_if(package.runtime.globalBlackboard, [](const auto& entry) {
        return entry.first.rfind(kActorTagBlackboardPrefix, 0) == 0;
    });
}

void ExternalModActorTagRuntime::ClearAll(std::vector<ExternalModPackage>& packages) const {
    for (auto& package : packages) {
        ClearForPackage(package);
    }
}

} // namespace SOH

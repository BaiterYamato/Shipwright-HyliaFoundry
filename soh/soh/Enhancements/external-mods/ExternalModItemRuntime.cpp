#include "ExternalModItemRuntime.h"

#include <algorithm>

extern "C" {
#include <z64.h>
}

namespace SOH {

bool ExternalModItemRuntime::IsHookshotItemId(int32_t itemId) {
    return itemId == ITEM_HOOKSHOT || itemId == ITEM_LONGSHOT;
}

void ExternalModItemRuntime::ApplySkyhookImpulse(void* playerPtr, float pullForce, float speed) {
    auto* player = static_cast<Player*>(playerPtr);
    if (player == nullptr) {
        return;
    }

    const float clampedPullForce = std::clamp(pullForce, 2.0f, 20.0f);
    const float clampedSpeed = std::clamp(speed, 2.0f, 35.0f);

    player->actor.velocity.y = std::max(player->actor.velocity.y, clampedPullForce);
    player->linearVelocity = std::max(player->linearVelocity, clampedSpeed);
}

void ExternalModItemRuntime::Reset(ExternalModPackage& package) const {
    package.runtime.itemStateMachineStates.clear();
    for (const auto& definition : package.runtime.itemStateMachineDefinitions) {
        package.runtime.itemStateMachineStates.emplace(
            definition.id, ExternalModItemStateMachineRuntimeState{ definition.initialState, false });
    }
}

bool ExternalModItemRuntime::DispatchEvent(ExternalModPackage& package,
                                           const ExternalModItemStateMachineDefinition& definition,
                                           ExternalModItemStateEvent event, const ActionDispatcher& dispatch) {
    auto& state = package.runtime.itemStateMachineStates[definition.id];
    if (state.currentState.empty()) {
        state.currentState = definition.initialState;
    }
    const auto transitionIt = std::find_if(
        definition.transitions.begin(), definition.transitions.end(), [&](const ExternalModItemStateTransition& transition) {
            return transition.event == event &&
                   (transition.fromState == "*" || transition.fromState == state.currentState);
        });
    if (transitionIt == definition.transitions.end()) {
        return false;
    }
    state.currentState = transitionIt->toState;
    if (dispatch) {
        dispatch(definition, *transitionIt);
    }
    return true;
}

void ExternalModItemRuntime::Tick(ExternalModPackage& package,
                                  const std::unordered_set<std::string>& selectedItemIds,
                                  const std::unordered_map<std::string, int32_t>& bindingMasks,
                                  int32_t currentButtons, int32_t previousButtons,
                                  const ActionDispatcher& dispatch) const {
    for (const auto& definition : package.runtime.itemStateMachineDefinitions) {
        auto& state = package.runtime.itemStateMachineStates[definition.id];
        if (state.currentState.empty()) {
            state.currentState = definition.initialState;
        }
        const bool selected = selectedItemIds.contains(definition.itemId);
        if (selected != state.selected) {
            DispatchEvent(package, definition,
                          selected ? ExternalModItemStateEvent::Select : ExternalModItemStateEvent::Deselect, dispatch);
            state.selected = selected;
        }
        if (!selected || definition.bindingId.empty()) {
            continue;
        }
        const auto bindingIt = bindingMasks.find(definition.bindingId);
        if (bindingIt == bindingMasks.end() || bindingIt->second == 0) {
            continue;
        }
        const int32_t mask = bindingIt->second;
        const bool current = (currentButtons & mask) == mask;
        const bool previous = (previousButtons & mask) == mask;
        if (current && !previous) {
            DispatchEvent(package, definition, ExternalModItemStateEvent::Press, dispatch);
        }
        if (current) {
            DispatchEvent(package, definition, ExternalModItemStateEvent::Hold, dispatch);
        }
        if (!current && previous) {
            DispatchEvent(package, definition, ExternalModItemStateEvent::Release, dispatch);
        }
    }
}

bool ExternalModItemRuntime::DispatchImpact(ExternalModPackage& package, const std::string& itemId,
                                            const ActionDispatcher& dispatch) const {
    bool handled = false;
    for (const auto& definition : package.runtime.itemStateMachineDefinitions) {
        if (definition.itemId == itemId) {
            handled = DispatchEvent(package, definition, ExternalModItemStateEvent::Impact, dispatch) || handled;
        }
    }
    return handled;
}

} // namespace SOH

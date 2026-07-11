#pragma once

#include <cstdint>
#include <functional>
#include <string>
#include <unordered_map>
#include <unordered_set>

#include "ExternalModTypes.h"

namespace SOH {

class ExternalModItemRuntime {
  public:
    using ActionDispatcher = std::function<void(const ExternalModItemStateMachineDefinition&,
                                                const ExternalModItemStateTransition&)>;

    static bool IsHookshotItemId(int32_t itemId);
    static void ApplySkyhookImpulse(void* playerPtr, float pullForce, float speed);

    void Reset(ExternalModPackage& package) const;
    void Tick(ExternalModPackage& package, const std::unordered_set<std::string>& selectedItemIds,
              const std::unordered_map<std::string, int32_t>& bindingMasks, int32_t currentButtons,
              int32_t previousButtons, const ActionDispatcher& dispatch) const;
    bool DispatchImpact(ExternalModPackage& package, const std::string& itemId, const ActionDispatcher& dispatch) const;

  private:
    static bool DispatchEvent(ExternalModPackage& package, const ExternalModItemStateMachineDefinition& definition,
                              ExternalModItemStateEvent event, const ActionDispatcher& dispatch);
};

} // namespace SOH

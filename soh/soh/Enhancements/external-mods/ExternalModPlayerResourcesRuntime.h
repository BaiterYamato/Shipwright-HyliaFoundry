#pragma once

#include <cstdint>
#include <functional>
#include <string>

#include "ExternalModInterop.h"
#include "ExternalModTypes.h"

struct PlayState;
struct Player;

namespace SOH {

class ExternalModPlayerResourcesRuntime {
  public:
    struct SettingsResolver {
        std::function<float(const ExternalModPackage&, const std::string&, const std::string&, float)> resolveFloat;
        std::function<bool(const ExternalModPackage&, const std::string&, const std::string&, bool)> resolveBool;
        std::function<void(const ExternalModPackage&, ExternalModHookType, const ExternalModHookEventContext&, const char*)>
            emitHook;
        std::function<float(const std::string&)> resolveLinkedRegenMultiplier;
    };

    void Reset(::PlayState* play);
    void OnLoadGame(ExternalModPackage& package);
    void OnSceneInit(ExternalModPackage& package);
    void OnPlayDestroy(ExternalModPackage& package);

    void TickPackage(ExternalModPackage& package, ::PlayState* play, ::Player* player, const SettingsResolver& resolver);

    bool TryConsumeActionStart(ExternalModPackage& package, ::PlayState* play, ::Player* player,
                               const std::string& actionTag, const SettingsResolver& resolver);
    bool TickAction(ExternalModPackage& package, ::PlayState* play, ::Player* player, const std::string& actionTag,
                    float deltaSeconds, const SettingsResolver& resolver);

    bool SetResourceValue(ExternalModPackage& package, const std::string& resourceId, float value,
                          const SettingsResolver& resolver, const char* triggerName);
    bool AddResourceValue(ExternalModPackage& package, const std::string& resourceId, float delta,
                          const SettingsResolver& resolver, const char* triggerName);
    bool ConsumeResource(ExternalModPackage& package, const std::string& resourceId, float amount,
                         const SettingsResolver& resolver, const char* triggerName);
    bool RefillResource(ExternalModPackage& package, const std::string& resourceId, float amount,
                        bool hasExplicitAmount, const SettingsResolver& resolver, const char* triggerName);
    bool SetResourceCapacity(ExternalModPackage& package, const std::string& resourceId, float value,
                             const SettingsResolver& resolver, const char* triggerName);

    bool IsActionInputActive(const ExternalModPackage& package, const std::string& actionTag, uint16_t curButtons,
                             uint16_t prevButtons, ExternalModInputTriggerType trigger) const;
    bool GetResourceConditionSnapshot(const ExternalModPackage& package, const std::string& resourceId, float& outCurrent,
                                      float& outCapacity) const;
    bool BuildRingView(const ExternalModPackage& package, const ExternalModResourceRingDefinition& definition,
                       const SettingsResolver& resolver, ExternalModsResourceRingView& outView) const;
    float GetMovementSpeedMultiplier(const ExternalModPackage& package, ::PlayState* play, ::Player* player) const;
    bool IsSprintBlocked(const ExternalModPackage& package, ::PlayState* play, ::Player* player) const;

  private:
    static const ExternalModPlayerResourceDefinition* FindDefinition(const ExternalModRuntime& runtime,
                                                                     const std::string& resourceId);
    static const ExternalModPlayerResourceActionRule* FindActionRule(const ExternalModRuntime& runtime,
                                                                     const std::string& actionTag,
                                                                     const ExternalModPlayerResourceDefinition** outDef);
    static ExternalModRuntime::PlayerResourceState* EnsureState(ExternalModPackage& package,
                                                                const ExternalModPlayerResourceDefinition& definition,
                                                                ::PlayState* play);
    static void PersistStateToDomains(ExternalModPackage& package, const ExternalModPlayerResourceDefinition& definition,
                                      const ExternalModRuntime::PlayerResourceState& state);
    static void ApplyDelta(ExternalModPackage& package, const ExternalModPlayerResourceDefinition& definition,
                           ExternalModRuntime::PlayerResourceState& state, float delta, const SettingsResolver& resolver,
                           const char* triggerName);
    static void UpdateVisibilityState(const ExternalModPlayerResourceDefinition& definition,
                                      const ExternalModResourceRingDefinition* ringDefinition,
                                      ExternalModRuntime::PlayerResourceState& state, int32_t visibleMs);
    static bool IsManualGameplayTickContext(::PlayState* play, ::Player* player);
    static float ResolveTickRuleMultiplier(const ExternalModPlayerResourceTickRule& rule, ::PlayState* play, ::Player* player);
};

} // namespace SOH

#include "ExternalModPlayerResourcesRuntime.h"
#include "ExternalModManager.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <limits>

#include "soh/OTRGlobals.h"
#include "libultraship/bridge/consolevariablebridge.h"

extern "C" {
#include <z64.h>
#include "functions.h"
#include "variables.h"
extern PlayState* gPlayState;
extern SaveContext gSaveContext;
}

namespace SOH {
namespace {
constexpr float kTickSeconds = 1.0f / 60.0f;
constexpr int32_t kTickMilliseconds = 16;

int32_t ResolveRingStyleKind(const std::string& style) {
    std::string normalized = style;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    if (normalized.rfind("magic_bar", 0) == 0 || normalized.rfind("mana_bar", 0) == 0 || normalized == "mana_yellow" ||
        normalized == "mana_blue" || normalized == "mana_orange") {
        return EXTERNAL_MODS_RESOURCE_RING_STYLE_MAGIC_BAR;
    }
    return EXTERNAL_MODS_RESOURCE_RING_STYLE_RING;
}

float ClampResourceValue(float value) {
    if (std::isnan(value) || !std::isfinite(value)) {
        return 0.0f;
    }
    return std::clamp(value, 0.0f, 999999.0f);
}

bool MatchButtons(uint16_t curButtons, uint16_t prevButtons, int32_t mask, ExternalModInputTriggerType trigger) {
    const uint16_t castMask = static_cast<uint16_t>(mask & 0xFFFF);
    if (castMask == 0) {
        return false;
    }

    switch (trigger) {
        case ExternalModInputTriggerType::Pressed:
            return (curButtons & castMask) != 0 && (prevButtons & castMask) == 0;
        case ExternalModInputTriggerType::Held:
            return (curButtons & castMask) == castMask;
        case ExternalModInputTriggerType::Released:
            return (curButtons & castMask) == 0 && (prevButtons & castMask) != 0;
        default:
            return false;
    }
}

ExternalModHookEventContext BuildResourceContext(const ExternalModPlayerResourceDefinition& definition,
                                                 const ExternalModRuntime::PlayerResourceState& state) {
    ExternalModHookEventContext context;
    context.scene = gPlayState != nullptr ? static_cast<int16_t>(gPlayState->sceneNum) : static_cast<int16_t>(-1);
    context.resourceId = definition.id;
    context.resourceCurrentValue = state.currentValue;
    context.resourceCapacityValue = state.capacityValue;
    context.resourcePercent = state.capacityValue > 0.0f ? (state.currentValue / state.capacityValue) : 0.0f;
    context.value = std::to_string(state.currentValue);
    return context;
}

float ResolveFloatSetting(const ExternalModPlayerResourcesRuntime::SettingsResolver& resolver, const ExternalModPackage& package,
                         const std::string& key, const std::string& domain, float fallback) {
    if (key.empty() || !resolver.resolveFloat) {
        return fallback;
    }
    return resolver.resolveFloat(package, key, domain, fallback);
}

bool ResolveBoolSetting(const ExternalModPlayerResourcesRuntime::SettingsResolver& resolver, const ExternalModPackage& package,
                        const std::string& key, const std::string& domain, bool fallback) {
    if (key.empty() || !resolver.resolveBool) {
        return fallback;
    }
    return resolver.resolveBool(package, key, domain, fallback);
}

bool IsResourceEnabled(const ExternalModPlayerResourcesRuntime::SettingsResolver& resolver, const ExternalModPackage& package,
                       const ExternalModPlayerResourceDefinition& definition) {
    if (definition.enabledSettingKey.empty()) {
        return true;
    }
    return ResolveBoolSetting(resolver, package, definition.enabledSettingKey, definition.settingsDomain, true);
}

float ResolveLowThresholdPercent(const ExternalModPlayerResourceDefinition& definition) {
    if (definition.lowThresholdPercent >= 0.0f) {
        return std::clamp(definition.lowThresholdPercent, 0.0f, 1.0f);
    }
    return std::clamp(definition.lowPercent, 0.0f, 1.0f);
}

int32_t ConvertHeartsToHealthUnits(float hearts) {
    return std::max(0, static_cast<int32_t>(std::round(hearts * 16.0f)));
}

} // namespace

void ExternalModPlayerResourcesRuntime::Reset(::PlayState* play) {
    (void)play;
}

void ExternalModPlayerResourcesRuntime::OnLoadGame(ExternalModPackage& package) {
    package.runtime.playerResourceStates.clear();
}

void ExternalModPlayerResourcesRuntime::OnSceneInit(ExternalModPackage& package) {
    for (auto& statePair : package.runtime.playerResourceStates) {
        statePair.second.initialized = false;
        statePair.second.recovering = false;
        statePair.second.consumedThisFrame = false;
        statePair.second.visibleMs = 0;
        statePair.second.regenDelayRemainingMs = 0;
        statePair.second.periodicEffectTimerMs = 0;
        statePair.second.lastSceneInitialized = -1;
        statePair.second.lastRoomInitialized = -1;
    }
}

void ExternalModPlayerResourcesRuntime::OnPlayDestroy(ExternalModPackage& package) {
    for (auto& statePair : package.runtime.playerResourceStates) {
        statePair.second.consumedThisFrame = false;
        statePair.second.recovering = false;
    }
}

const ExternalModPlayerResourceDefinition* ExternalModPlayerResourcesRuntime::FindDefinition(
    const ExternalModRuntime& runtime, const std::string& resourceId) {
    const auto it = std::find_if(runtime.playerResourceDefinitions.begin(), runtime.playerResourceDefinitions.end(),
                                 [&](const ExternalModPlayerResourceDefinition& definition) {
                                     return definition.id == resourceId;
                                 });
    return it == runtime.playerResourceDefinitions.end() ? nullptr : &(*it);
}

const ExternalModPlayerResourceActionRule* ExternalModPlayerResourcesRuntime::FindActionRule(
    const ExternalModRuntime& runtime, const std::string& actionTag, const ExternalModPlayerResourceDefinition** outDef) {
    if (outDef != nullptr) {
        *outDef = nullptr;
    }
    for (const auto& definition : runtime.playerResourceDefinitions) {
        const auto it = std::find_if(definition.actionRules.begin(), definition.actionRules.end(),
                                     [&](const ExternalModPlayerResourceActionRule& rule) { return rule.tag == actionTag; });
        if (it != definition.actionRules.end()) {
            if (outDef != nullptr) {
                *outDef = &definition;
            }
            return &(*it);
        }
    }
    return nullptr;
}

ExternalModRuntime::PlayerResourceState* ExternalModPlayerResourcesRuntime::EnsureState(
    ExternalModPackage& package, const ExternalModPlayerResourceDefinition& definition, ::PlayState* play) {
    auto& state = package.runtime.playerResourceStates[definition.id];
    if (!state.initialized) {
        state.resourceId = definition.id;
        state.capacityValue = definition.baseCapacity > 0.0f ? definition.baseCapacity : definition.wheelCapacity;
        state.currentValue = definition.initialValue >= 0.0f ? definition.initialValue : state.capacityValue;
        state.depleted = state.currentValue <= 0.0f;
        state.periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
        state.initialized = true;

        const std::string domainId = definition.storageDomainId.empty() ? definition.id : definition.storageDomainId;
        if (!domainId.empty()) {
            const auto domainIt = package.runtime.persistentDomains.find(domainId);
            if (domainIt != package.runtime.persistentDomains.end()) {
                const auto capIt = domainIt->second.find(definition.capacityStorageKey);
                if (capIt != domainIt->second.end()) {
                    try {
                        state.capacityValue = ClampResourceValue(std::stof(capIt->second));
                    } catch (...) {
                    }
                }
                if (definition.persistCurrentValue) {
                    const auto valueIt = domainIt->second.find(definition.currentStorageKey);
                    if (valueIt != domainIt->second.end()) {
                        try {
                            state.currentValue = ClampResourceValue(std::stof(valueIt->second));
                        } catch (...) {
                        }
                    }
                }
            }
        }

        if (definition.refillOnLoad) {
            state.currentValue = state.capacityValue;
            state.depleted = false;
            state.periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
        }
    }

    const int16_t sceneNum = play != nullptr ? static_cast<int16_t>(play->sceneNum) : static_cast<int16_t>(-1);
    const int16_t roomNum = play != nullptr ? static_cast<int16_t>(play->roomCtx.curRoom.num) : static_cast<int16_t>(-1);
    const bool sceneChanged = state.lastSceneInitialized != sceneNum;
    const bool roomChanged = state.lastRoomInitialized != roomNum;
    if (sceneChanged || roomChanged) {
        if (definition.refillOnSceneEnter) {
            state.currentValue = state.capacityValue;
            state.depleted = false;
            state.regenDelayRemainingMs = 0;
            state.visibleMs = 0;
            state.periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
        }
        state.lastSceneInitialized = sceneNum;
        state.lastRoomInitialized = roomNum;
    }

    state.capacityValue = std::max(definition.wheelCapacity > 0.0f ? definition.wheelCapacity : 1.0f, state.capacityValue);
    state.currentValue = std::clamp(state.currentValue, 0.0f, state.capacityValue);
    return &state;
}

void ExternalModPlayerResourcesRuntime::PersistStateToDomains(ExternalModPackage& package,
                                                              const ExternalModPlayerResourceDefinition& definition,
                                                              const ExternalModRuntime::PlayerResourceState& state) {
    const std::string domainId = definition.storageDomainId.empty() ? definition.id : definition.storageDomainId;
    if (domainId.empty()) {
        return;
    }
    auto& domain = package.runtime.persistentDomains[domainId];
    domain[definition.capacityStorageKey] = std::to_string(state.capacityValue);
    if (definition.persistCurrentValue) {
        domain[definition.currentStorageKey] = std::to_string(state.currentValue);
    } else {
        domain.erase(definition.currentStorageKey);
    }
    package.runtime.persistentStateDirty = true;
}

void ExternalModPlayerResourcesRuntime::UpdateVisibilityState(const ExternalModPlayerResourceDefinition& definition,
                                                              const ExternalModResourceRingDefinition* ringDefinition,
                                                              ExternalModRuntime::PlayerResourceState& state,
                                                              int32_t visibleMs) {
    (void)definition;
    state.visibleMs = std::max(state.visibleMs, visibleMs);
    if (ringDefinition != nullptr && ringDefinition->showWhenFull) {
        state.visibleMs = std::max(state.visibleMs, ringDefinition->hideDelayMs);
    }
}

bool ExternalModPlayerResourcesRuntime::IsManualGameplayTickContext(::PlayState* play, ::Player* player) {
    if (play == nullptr || player == nullptr) {
        return false;
    }
    if (player->csAction != 0) {
        return false;
    }
    if (player->stateFlags1 &
        (PLAYER_STATE1_IN_CUTSCENE | PLAYER_STATE1_ON_HORSE | PLAYER_STATE1_DEAD | PLAYER_STATE1_FIRST_PERSON)) {
        return false;
    }
    if (play->pauseCtx.state != 0) {
        return false;
    }
    return true;
}

float ExternalModPlayerResourcesRuntime::ResolveTickRuleMultiplier(const ExternalModPlayerResourceTickRule& rule, ::PlayState* play,
                                                                   ::Player* player) {
    (void)play;
    if (player == nullptr) {
        return 1.0f;
    }

    float multiplier = 1.0f;
    const bool moving = (std::fabs(player->actor.speedXZ) > 0.1f) || (std::fabs(player->linearVelocity) > 0.1f);
    const bool climbing = (player->stateFlags1 &
                           (PLAYER_STATE1_CLIMBING_LADDER | PLAYER_STATE1_HANGING_OFF_LEDGE | PLAYER_STATE1_CLIMBING_LEDGE)) != 0;
    const bool swimming = (player->stateFlags1 & PLAYER_STATE1_IN_WATER) != 0;
    const bool sprintHeld = moving && ExternalModManager::Instance().IsPlayerResourceActionInputActive(
                                         gPlayState, player, "sprint.hold", ExternalModInputTriggerType::Held);

    if (moving) {
        multiplier *= std::max(0.0f, rule.movingMultiplier);
    }
    if (climbing) {
        multiplier *= std::max(0.0f, rule.climbMultiplier);
    }
    if (swimming) {
        multiplier *= std::max(0.0f, rule.swimMultiplier);
    }
    if (sprintHeld) {
        multiplier *= std::max(0.0f, rule.sprintMultiplier);
    }
    return multiplier;
}

void ExternalModPlayerResourcesRuntime::ApplyDelta(ExternalModPackage& package,
                                                   const ExternalModPlayerResourceDefinition& definition,
                                                   ExternalModRuntime::PlayerResourceState& state, float delta,
                                                   const SettingsResolver& resolver, const char* triggerName) {
    const float beforeValue = state.currentValue;
    const float beforeCapacity = state.capacityValue;
    const bool wasDepleted = state.depleted;

    state.currentValue = ClampResourceValue(state.currentValue + delta);
    state.currentValue = std::clamp(state.currentValue, 0.0f, state.capacityValue);
    state.depleted = state.currentValue <= 0.0001f;
    state.recovering = delta > 0.0f && state.currentValue < state.capacityValue;
    if (!wasDepleted && state.depleted) {
        state.periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
    } else if (!state.depleted && definition.depletionEffects.periodicDamageIntervalMs > 0) {
        state.periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
    }
    if (delta < 0.0f) {
        state.consumedThisFrame = true;
        state.recovering = false;
        state.regenDelayRemainingMs = state.depleted ? definition.depletedRegenDelayMs : definition.regenDelayMs;
        state.visibleMs = std::max(state.visibleMs, 900);
    } else if (delta > 0.0f) {
        state.visibleMs = std::max(state.visibleMs, 900);
    }

    if (std::abs(beforeValue - state.currentValue) > 0.0001f && resolver.emitHook) {
        resolver.emitHook(package, ExternalModHookType::OnResourceChanged, BuildResourceContext(definition, state),
                          triggerName != nullptr ? triggerName : "resource.changed");
    }
    if (!wasDepleted && state.depleted && resolver.emitHook) {
        resolver.emitHook(package, ExternalModHookType::OnResourceDepleted, BuildResourceContext(definition, state),
                          triggerName != nullptr ? triggerName : "resource.depleted");
    }
    if (wasDepleted && !state.depleted && resolver.emitHook) {
        resolver.emitHook(package, ExternalModHookType::OnResourceRecovered, BuildResourceContext(definition, state),
                          triggerName != nullptr ? triggerName : "resource.recovered");
    }
    if (std::abs(beforeCapacity - state.capacityValue) > 0.0001f && resolver.emitHook) {
        resolver.emitHook(package, ExternalModHookType::OnResourceCapacityChanged,
                          BuildResourceContext(definition, state),
                          triggerName != nullptr ? triggerName : "resource.capacityChanged");
    }

    PersistStateToDomains(package, definition, state);
}

void ExternalModPlayerResourcesRuntime::TickPackage(ExternalModPackage& package, ::PlayState* play, ::Player* player,
                                                    const SettingsResolver& resolver) {
    for (const auto& definition : package.runtime.playerResourceDefinitions) {
        auto* state = EnsureState(package, definition, play);
        if (state == nullptr) {
            continue;
        }

        if (!IsResourceEnabled(resolver, package, definition)) {
            state->consumedThisFrame = false;
            state->recovering = false;
            state->visibleMs = 0;
            continue;
        }

        const int32_t hideDelayMs = 900;
        if (state->visibleMs > 0) {
            state->visibleMs = std::max(0, state->visibleMs - kTickMilliseconds);
        }

        if (!definition.tickRules.empty()) {
            float drainRate = 0.0f;
            for (const auto& rule : definition.tickRules) {
                if (rule.drainPerSecond <= 0.0f) {
                    continue;
                }
                if (rule.manualGameplayOnly && !IsManualGameplayTickContext(play, player)) {
                    continue;
                }
                drainRate += rule.drainPerSecond * ResolveTickRuleMultiplier(rule, play, player);
            }
            if (!definition.drainMultiplierSettingKey.empty()) {
                drainRate *= ResolveFloatSetting(resolver, package, definition.drainMultiplierSettingKey,
                                                 definition.settingsDomain, 1.0f);
            }
            if (drainRate > 0.0f) {
                ApplyDelta(package, definition, *state, -(drainRate * kTickSeconds), resolver, "playerResource.tick");
            }
        }

        if (state->depleted && definition.depletionEffects.periodicDamageHearts > 0.0f &&
            definition.depletionEffects.periodicDamageIntervalMs > 0 && play != nullptr) {
            state->periodicEffectTimerMs -= kTickMilliseconds;
            if (state->periodicEffectTimerMs <= 0) {
                const int32_t damageAmount = ConvertHeartsToHealthUnits(definition.depletionEffects.periodicDamageHearts);
                if (damageAmount > 0) {
                    Health_ChangeBy(play, static_cast<s16>(-damageAmount));
                }
                state->periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
            }
        } else {
            state->periodicEffectTimerMs = definition.depletionEffects.periodicDamageIntervalMs;
        }

        if (state->consumedThisFrame) {
            state->consumedThisFrame = false;
            state->recovering = false;
            continue;
        }

        if (state->regenDelayRemainingMs > 0) {
            state->regenDelayRemainingMs = std::max(0, state->regenDelayRemainingMs - kTickMilliseconds);
            state->recovering = false;
            continue;
        }

        if (state->currentValue >= state->capacityValue) {
            state->currentValue = state->capacityValue;
            state->recovering = false;
            continue;
        }

        float regenRate = definition.regenRatePerSecond;
        if (!definition.regenMultiplierSettingKey.empty()) {
            regenRate *= ResolveFloatSetting(resolver, package, definition.regenMultiplierSettingKey,
                                             definition.settingsDomain, 1.0f);
        }
        if (resolver.resolveLinkedRegenMultiplier) {
            regenRate *= resolver.resolveLinkedRegenMultiplier(definition.id);
        }
        if (regenRate <= 0.0f) {
            state->recovering = false;
            continue;
        }

        state->recovering = true;
        state->visibleMs = std::max(state->visibleMs, hideDelayMs);
        ApplyDelta(package, definition, *state, regenRate * kTickSeconds, resolver, "playerResource.regen");
        if (state->currentValue >= state->capacityValue - 0.0001f) {
            state->currentValue = state->capacityValue;
            state->recovering = false;
            PersistStateToDomains(package, definition, *state);
        }
    }
}

bool ExternalModPlayerResourcesRuntime::TryConsumeActionStart(ExternalModPackage& package, ::PlayState* play, ::Player* player,
                                                              const std::string& actionTag,
                                                              const SettingsResolver& resolver) {
    (void)player;
    const ExternalModPlayerResourceDefinition* definition = nullptr;
    const auto* rule = FindActionRule(package.runtime, actionTag, &definition);
    if (rule == nullptr || definition == nullptr) {
        return true;
    }
    auto* state = EnsureState(package, *definition, play);
    if (state == nullptr) {
        return true;
    }
    if (!IsResourceEnabled(resolver, package, *definition)) {
        return true;
    }
    float cost = rule->startCost;
    if (!definition->drainMultiplierSettingKey.empty()) {
        cost *= ResolveFloatSetting(resolver, package, definition->drainMultiplierSettingKey, definition->settingsDomain, 1.0f);
    }
    if (cost <= 0.0f) {
        state->visibleMs = std::max(state->visibleMs, 900);
        return true;
    }
    if (state->currentValue + 0.0001f < cost && rule->blockWhenInsufficient) {
        state->visibleMs = std::max(state->visibleMs, 900);
        return false;
    }
    ApplyDelta(package, *definition, *state, -cost, resolver, actionTag.c_str());
    return !state->depleted || !rule->cancelOnDepleted;
}

bool ExternalModPlayerResourcesRuntime::TickAction(ExternalModPackage& package, ::PlayState* play, ::Player* player,
                                                   const std::string& actionTag, float deltaSeconds,
                                                   const SettingsResolver& resolver) {
    (void)player;
    const ExternalModPlayerResourceDefinition* definition = nullptr;
    const auto* rule = FindActionRule(package.runtime, actionTag, &definition);
    if (rule == nullptr || definition == nullptr) {
        return true;
    }
    auto* state = EnsureState(package, *definition, play);
    if (state == nullptr) {
        return true;
    }
    if (!IsResourceEnabled(resolver, package, *definition)) {
        return true;
    }
    float cost = rule->costPerSecond * std::max(deltaSeconds, 0.0f);
    if (!definition->drainMultiplierSettingKey.empty()) {
        cost *= ResolveFloatSetting(resolver, package, definition->drainMultiplierSettingKey, definition->settingsDomain, 1.0f);
    }
    if (cost <= 0.0f) {
        state->visibleMs = std::max(state->visibleMs, 900);
        state->consumedThisFrame = true;
        return true;
    }
    if (state->currentValue + 0.0001f < cost && rule->blockWhenInsufficient) {
        state->visibleMs = std::max(state->visibleMs, 900);
        return false;
    }
    ApplyDelta(package, *definition, *state, -cost, resolver, actionTag.c_str());
    return !state->depleted || !rule->cancelOnDepleted;
}

bool ExternalModPlayerResourcesRuntime::SetResourceValue(ExternalModPackage& package, const std::string& resourceId, float value,
                                                         const SettingsResolver& resolver, const char* triggerName) {
    const auto* definition = FindDefinition(package.runtime, resourceId);
    if (definition == nullptr) {
        return false;
    }
    auto* state = EnsureState(package, *definition, gPlayState);
    if (state == nullptr) {
        return false;
    }
    const float delta = std::clamp(value, 0.0f, state->capacityValue) - state->currentValue;
    ApplyDelta(package, *definition, *state, delta, resolver, triggerName);
    return true;
}

bool ExternalModPlayerResourcesRuntime::AddResourceValue(ExternalModPackage& package, const std::string& resourceId, float delta,
                                                         const SettingsResolver& resolver, const char* triggerName) {
    const auto* definition = FindDefinition(package.runtime, resourceId);
    if (definition == nullptr) {
        return false;
    }
    auto* state = EnsureState(package, *definition, gPlayState);
    if (state == nullptr) {
        return false;
    }
    ApplyDelta(package, *definition, *state, delta, resolver, triggerName);
    return true;
}

bool ExternalModPlayerResourcesRuntime::ConsumeResource(ExternalModPackage& package, const std::string& resourceId, float amount,
                                                        const SettingsResolver& resolver, const char* triggerName) {
    return AddResourceValue(package, resourceId, -std::abs(amount), resolver, triggerName);
}

bool ExternalModPlayerResourcesRuntime::RefillResource(ExternalModPackage& package, const std::string& resourceId, float amount,
                                                       bool hasExplicitAmount, const SettingsResolver& resolver,
                                                       const char* triggerName) {
    const auto* definition = FindDefinition(package.runtime, resourceId);
    if (definition == nullptr) {
        return false;
    }
    auto* state = EnsureState(package, *definition, gPlayState);
    if (state == nullptr) {
        return false;
    }
    const float delta = hasExplicitAmount ? std::abs(amount) : (state->capacityValue - state->currentValue);
    ApplyDelta(package, *definition, *state, delta, resolver, triggerName);
    return true;
}

bool ExternalModPlayerResourcesRuntime::SetResourceCapacity(ExternalModPackage& package, const std::string& resourceId, float value,
                                                            const SettingsResolver& resolver, const char* triggerName) {
    const auto* definition = FindDefinition(package.runtime, resourceId);
    if (definition == nullptr) {
        return false;
    }
    auto* state = EnsureState(package, *definition, gPlayState);
    if (state == nullptr) {
        return false;
    }
    const float beforeCapacity = state->capacityValue;
    state->capacityValue = std::max(definition->wheelCapacity > 0.0f ? definition->wheelCapacity : 1.0f,
                                    ClampResourceValue(value));
    state->currentValue = std::clamp(state->currentValue, 0.0f, state->capacityValue);
    ApplyDelta(package, *definition, *state, 0.0f, resolver, triggerName);
    if (std::abs(beforeCapacity - state->capacityValue) > 0.0001f) {
        PersistStateToDomains(package, *definition, *state);
    }
    return true;
}

bool ExternalModPlayerResourcesRuntime::IsActionInputActive(const ExternalModPackage& package, const std::string& actionTag,
                                                            uint16_t curButtons, uint16_t prevButtons,
                                                            ExternalModInputTriggerType trigger) const {
    const ExternalModPlayerResourceDefinition* definition = nullptr;
    const auto* rule = FindActionRule(package.runtime, actionTag, &definition);
    (void)definition;
    if (rule == nullptr || rule->inputBindingId.empty()) {
        return false;
    }
    const auto bindingIt = std::find_if(package.runtime.inputBindings.begin(), package.runtime.inputBindings.end(),
                                        [&](const ExternalModInputBinding& binding) {
                                            return binding.id == rule->inputBindingId;
                                        });
    if (bindingIt == package.runtime.inputBindings.end()) {
        return false;
    }
    const auto cvarName = ExternalModManager::BuildBindingCVarName(package.manifest.id, bindingIt->id);
    int32_t effectiveMask = bindingIt->allowUserRemap ? CVarGetInteger(cvarName.c_str(), bindingIt->defaultMask)
                                                      : bindingIt->defaultMask;
    if (effectiveMask == 0) {
        effectiveMask = bindingIt->defaultMask;
    }
    return MatchButtons(curButtons, prevButtons, effectiveMask, trigger);
}

bool ExternalModPlayerResourcesRuntime::GetResourceConditionSnapshot(const ExternalModPackage& package,
                                                                     const std::string& resourceId, float& outCurrent,
                                                                     float& outCapacity) const {
    const auto it = package.runtime.playerResourceStates.find(resourceId);
    if (it == package.runtime.playerResourceStates.end() || !it->second.initialized) {
        return false;
    }
    outCurrent = it->second.currentValue;
    outCapacity = it->second.capacityValue;
    return true;
}

bool ExternalModPlayerResourcesRuntime::BuildRingView(const ExternalModPackage& package,
                                                      const ExternalModResourceRingDefinition& definition,
                                                      const SettingsResolver& resolver,
                                                      ExternalModsResourceRingView& outView) const {
    const auto stateIt = package.runtime.playerResourceStates.find(definition.resourceId);
    if (stateIt == package.runtime.playerResourceStates.end() || !stateIt->second.initialized) {
        return false;
    }
    const auto* resourceDefinition = FindDefinition(package.runtime, definition.resourceId);
    if (resourceDefinition == nullptr) {
        return false;
    }
    if (!IsResourceEnabled(resolver, package, *resourceDefinition)) {
        return false;
    }
    const auto& state = stateIt->second;
    const float lowThresholdPercent = ResolveLowThresholdPercent(*resourceDefinition);
    const bool isLow = state.capacityValue > 0.0f && (state.currentValue / state.capacityValue) <= lowThresholdPercent;
    const bool showContextualEnabled = ResolveBoolSetting(resolver, package, definition.contextualEnabledSettingKey,
                                                          definition.settingsDomain, true);
    const bool showFixedEnabled = ResolveBoolSetting(resolver, package, definition.fixedEnabledSettingKey,
                                                     definition.settingsDomain, false);
    const float scale = ResolveFloatSetting(resolver, package, definition.scaleSettingKey, definition.settingsDomain,
                                            definition.scale);
    const float opacity = ResolveFloatSetting(resolver, package, definition.opacitySettingKey, definition.settingsDomain,
                                              definition.opacity);
    const bool hasVisibilityReason = state.currentValue < state.capacityValue || state.visibleMs > 0 || state.depleted || state.recovering;

    memset(&outView, 0, sizeof(outView));
    outView.active = 1;
    outView.styleKind = ResolveRingStyleKind(definition.style);
    outView.currentValue = state.currentValue;
    outView.capacityValue = state.capacityValue;
    outView.wheelCapacity = resourceDefinition->wheelCapacity > 0.0f ? resourceDefinition->wheelCapacity : state.capacityValue;
    outView.segmentCount = resourceDefinition->segmentCount;
    outView.scale = scale;
    outView.opacity = opacity;
    outView.thickness = definition.thickness;
    outView.ringSpacing = definition.ringSpacing;
    outView.fixedStackSpacing = definition.fixedStackSpacing;
    outView.screenOffsetX = definition.screenOffsetX;
    outView.screenOffsetY = definition.screenOffsetY;
    outView.worldOffsetX = definition.worldOffsetX;
    outView.worldOffsetY = definition.worldOffsetY;
    outView.worldOffsetZ = definition.worldOffsetZ;
    outView.lowThresholdPercent = lowThresholdPercent;
    outView.lowPulse = definition.lowPulse ? 1 : 0;
    outView.exhausted = state.depleted ? 1 : 0;
    outView.showContextual = showContextualEnabled && (definition.anchorMode != ExternalModResourceRingAnchorMode::Fixed) &&
                             (definition.showWhenFull || hasVisibilityReason);
    outView.showFixed = showFixedEnabled && (definition.anchorMode != ExternalModResourceRingAnchorMode::Contextual) &&
                        (definition.showWhenFull || hasVisibilityReason);
    outView.fixedAnchor = static_cast<int32_t>(definition.fixedAnchor);
    outView.fixedStackOrder = definition.fixedStackOrder;
    outView.fixedStackMagicBarGroup =
        (!definition.fixedStackGroup.empty() && definition.fixedStackGroup == "magic_bar_stack") ? 1 : 0;
    outView.normalColor[0] = definition.normalColor.r;
    outView.normalColor[1] = definition.normalColor.g;
    outView.normalColor[2] = definition.normalColor.b;
    outView.normalColor[3] = definition.normalColor.a;
    outView.lowColor[0] = definition.lowColor.r;
    outView.lowColor[1] = definition.lowColor.g;
    outView.lowColor[2] = definition.lowColor.b;
    outView.lowColor[3] = definition.lowColor.a;
    outView.exhaustedColor[0] = definition.exhaustedColor.r;
    outView.exhaustedColor[1] = definition.exhaustedColor.g;
    outView.exhaustedColor[2] = definition.exhaustedColor.b;
    outView.exhaustedColor[3] = definition.exhaustedColor.a;
    outView.backgroundColor[0] = definition.backgroundColor.r;
    outView.backgroundColor[1] = definition.backgroundColor.g;
    outView.backgroundColor[2] = definition.backgroundColor.b;
    outView.backgroundColor[3] = definition.backgroundColor.a;
    outView.segmentColor[0] = definition.segmentColor.r;
    outView.segmentColor[1] = definition.segmentColor.g;
    outView.segmentColor[2] = definition.segmentColor.b;
    outView.segmentColor[3] = definition.segmentColor.a;
    outView.companionIconRgba32 = definition.companionIconRgba32.empty() ? nullptr : definition.companionIconRgba32.data();
    outView.companionCounterValue = -1;
    if (!definition.companionCounterSource.empty()) {
        const auto stackIt = package.runtime.consumableStackCounts.find(definition.companionCounterSource);
        if (stackIt != package.runtime.consumableStackCounts.end()) {
            outView.companionCounterValue = stackIt->second;
        }
    }
    outView.isLow = isLow ? 1 : 0;
    return outView.showContextual || outView.showFixed;
}

float ExternalModPlayerResourcesRuntime::GetMovementSpeedMultiplier(const ExternalModPackage& package, ::PlayState* play,
                                                                   ::Player* player) const {
    if (!IsManualGameplayTickContext(play, player)) {
        return 1.0f;
    }

    float multiplier = 1.0f;
    for (const auto& definition : package.runtime.playerResourceDefinitions) {
        const auto stateIt = package.runtime.playerResourceStates.find(definition.id);
        if (stateIt == package.runtime.playerResourceStates.end() || !stateIt->second.initialized || !stateIt->second.depleted) {
            continue;
        }
        if (definition.depletionEffects.movementMultiplier > 0.0f) {
            multiplier = std::min(multiplier, definition.depletionEffects.movementMultiplier);
        }
    }
    return std::clamp(multiplier, 0.1f, 1.0f);
}

bool ExternalModPlayerResourcesRuntime::IsSprintBlocked(const ExternalModPackage& package, ::PlayState* play, ::Player* player) const {
    if (!IsManualGameplayTickContext(play, player)) {
        return false;
    }

    for (const auto& definition : package.runtime.playerResourceDefinitions) {
        const auto stateIt = package.runtime.playerResourceStates.find(definition.id);
        if (stateIt == package.runtime.playerResourceStates.end() || !stateIt->second.initialized || !stateIt->second.depleted) {
            continue;
        }
        if (definition.depletionEffects.disableSprint) {
            return true;
        }
    }
    return false;
}

} // namespace SOH

#include "ExternalModUi.h"

#include <algorithm>
#include <cctype>
#include <cmath>
#include <cstring>
#include <filesystem>
#include <iomanip>
#include <locale>
#include <sstream>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

#include <ship/Context.h>

#include "ExternalModManager.h"
#include "soh/cvar_prefixes.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/SohGui/UIWidgets.hpp"

namespace SOH {

const char* GetExternalModItemSlotName(ExternalModItemSlot slot) {
    switch (slot) {
        case ExternalModItemSlot::Stick:
            return "SLOT_STICK";
        case ExternalModItemSlot::Nut:
            return "SLOT_NUT";
        case ExternalModItemSlot::Bomb:
            return "SLOT_BOMB";
        case ExternalModItemSlot::Bow:
            return "SLOT_BOW";
        case ExternalModItemSlot::FireArrow:
            return "SLOT_ARROW_FIRE";
        case ExternalModItemSlot::DinsFire:
            return "SLOT_DINS_FIRE";
        case ExternalModItemSlot::Slingshot:
            return "SLOT_SLINGSHOT";
        case ExternalModItemSlot::Ocarina:
            return "SLOT_OCARINA";
        case ExternalModItemSlot::Bombchu:
            return "SLOT_BOMBCHU";
        case ExternalModItemSlot::Hookshot:
            return "SLOT_HOOKSHOT";
        case ExternalModItemSlot::IceArrow:
            return "SLOT_ARROW_ICE";
        case ExternalModItemSlot::FaroresWind:
            return "SLOT_FARORES_WIND";
        case ExternalModItemSlot::Boomerang:
            return "SLOT_BOOMERANG";
        case ExternalModItemSlot::Lens:
            return "SLOT_LENS";
        case ExternalModItemSlot::Bean:
            return "SLOT_BEAN";
        case ExternalModItemSlot::LightArrow:
            return "SLOT_ARROW_LIGHT";
        case ExternalModItemSlot::Hammer:
            return "SLOT_HAMMER";
        case ExternalModItemSlot::NayrusLove:
            return "SLOT_NAYRUS_LOVE";
        case ExternalModItemSlot::Bottle1:
            return "SLOT_BOTTLE_1";
        case ExternalModItemSlot::Bottle2:
            return "SLOT_BOTTLE_2";
        case ExternalModItemSlot::Bottle3:
            return "SLOT_BOTTLE_3";
        case ExternalModItemSlot::Bottle4:
            return "SLOT_BOTTLE_4";
        case ExternalModItemSlot::TradeAdult:
            return "SLOT_TRADE_ADULT";
        case ExternalModItemSlot::TradeChild:
            return "SLOT_TRADE_CHILD";
        default:
            return "UNKNOWN";
    }
}
const char* GetExternalModItemAgePolicyName(ExternalModItemAgePolicy policy) {
    switch (policy) {
        case ExternalModItemAgePolicy::RespectVanilla:
            return "respectVanilla";
        case ExternalModItemAgePolicy::AllowChild:
            return "allowChild";
        case ExternalModItemAgePolicy::AllowAdult:
            return "allowAdult";
        default:
            return "unknown";
    }
}

const char* GetExternalModItemUseModeName(ExternalModItemUseMode mode) {
    switch (mode) {
        case ExternalModItemUseMode::Vanilla:
            return "vanilla";
        case ExternalModItemUseMode::Override:
            return "override";
        case ExternalModItemUseMode::Augment:
            return "augment";
        default:
            return "unknown";
    }
}

const char* GetExternalModActorArchetypeName(ExternalModActorArchetype archetype) {
    switch (archetype) {
        case ExternalModActorArchetype::Npc:
            return "npc";
        case ExternalModActorArchetype::Prop:
            return "prop";
        case ExternalModActorArchetype::Trigger:
            return "trigger";
        default:
            return "unknown";
    }
}

const char* GetExternalModHookTypeName(ExternalModHookType hookType) {
    switch (hookType) {
        case ExternalModHookType::OnLoadGame:
            return "OnLoadGame";
        case ExternalModHookType::OnExitGame:
            return "OnExitGame";
        case ExternalModHookType::OnSceneInit:
            return "OnSceneInit";
        case ExternalModHookType::AfterSceneCommands:
            return "AfterSceneCommands";
        case ExternalModHookType::OnTransitionEnd:
            return "OnTransitionEnd";
        case ExternalModHookType::OnFlagSet:
            return "OnFlagSet";
        case ExternalModHookType::OnFlagUnset:
            return "OnFlagUnset";
        case ExternalModHookType::OnSceneFlagSet:
            return "OnSceneFlagSet";
        case ExternalModHookType::OnSceneFlagUnset:
            return "OnSceneFlagUnset";
        case ExternalModHookType::OnPlayerUpdate:
            return "OnPlayerUpdate";
        case ExternalModHookType::OnPlayerUseItem:
            return "OnPlayerUseItem";
        case ExternalModHookType::OnPlayerHealthChange:
            return "OnPlayerHealthChange";
        case ExternalModHookType::OnItemReceive:
            return "OnItemReceive";
        case ExternalModHookType::OnActorInit:
            return "OnActorInit";
        case ExternalModHookType::OnActorSpawn:
            return "OnActorSpawn";
        case ExternalModHookType::OnActorUpdate:
            return "OnActorUpdate";
        case ExternalModHookType::OnActorKill:
            return "OnActorKill";
        case ExternalModHookType::OnActorDestroy:
            return "OnActorDestroy";
        case ExternalModHookType::OnEnemyDefeat:
            return "OnEnemyDefeat";
        case ExternalModHookType::OnBossDefeat:
            return "OnBossDefeat";
        case ExternalModHookType::OnPlayDestroy:
            return "OnPlayDestroy";
        case ExternalModHookType::OnGameFrameUpdate:
            return "OnGameFrameUpdate";
        default:
            return "UnknownHook";
    }
}

const char* GetExternalModHookDispatchName(ExternalModHookDispatchType dispatchType) {
    switch (dispatchType) {
        case ExternalModHookDispatchType::Actions:
            return "actions";
        case ExternalModHookDispatchType::WasmExport:
            return "wasmExport";
        default:
            return "unknown";
    }
}

const char* GetExternalModRuntimeModuleFormatName(ExternalModRuntimeModuleFormat moduleFormat) {
    switch (moduleFormat) {
        case ExternalModRuntimeModuleFormat::WasmBinary:
            return "wasm";
        case ExternalModRuntimeModuleFormat::WatText:
            return "wat";
        case ExternalModRuntimeModuleFormat::NativeLibrary:
            return "dll";
        default:
            return "unknown";
    }
}

std::string ToLowerUi(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string SanitizeModIdForUiCVar(const std::string& modId) {
    std::string out;
    out.reserve(modId.size());
    for (const unsigned char c : modId) {
        if (std::isalnum(c) != 0) {
            out.push_back(static_cast<char>(std::tolower(c)));
        } else {
            out.push_back('_');
        }
    }
    if (out.empty()) {
        return "unknown_mod";
    }
    return out;
}

std::string BuildPermissionPromptSeenCVarName(const std::string& modId) {
    return "gExternalMods.PermissionPrompt." + SanitizeModIdForUiCVar(modId) + ".Seen";
}

enum class ExternalModUiPackageTab {
    Mods,
    CoreApi,
};

struct ExternalModUiPackageClassification {
    ExternalModUiPackageTab tab = ExternalModUiPackageTab::Mods;
    std::string badge;
    std::string reason;
};

ExternalModUiPackageClassification ClassifyExternalModPackage(const ExternalModPackage& package) {
    ExternalModUiPackageClassification classification{};
    if (ToLowerUi(package.manifest.uiCategory) == "core_api") {
        classification.tab = ExternalModUiPackageTab::CoreApi;
        classification.badge = "framework";
        classification.reason = "classified by manifest uiCategory=core_api";
        return classification;
    }

    classification.tab = ExternalModUiPackageTab::Mods;
    classification.badge = "content";
    classification.reason = package.manifest.uiCategory.empty()
                                ? "classified as mod (uiCategory missing fallback)"
                                : "classified by manifest uiCategory=mod";
    return classification;
}

const char* GetIssueSeverityLabel(ExternalModIssueSeverity severity) {
    switch (severity) {
        case ExternalModIssueSeverity::Info:
            return "INFO";
        case ExternalModIssueSeverity::Warn:
            return "WARN";
        case ExternalModIssueSeverity::Error:
            return "ERROR";
        case ExternalModIssueSeverity::Fatal:
            return "FATAL";
        default:
            return "INFO";
    }
}

ImVec4 GetIssueSeverityColor(ExternalModIssueSeverity severity) {
    switch (severity) {
        case ExternalModIssueSeverity::Info:
            return ImVec4(0.55f, 0.75f, 0.95f, 1.0f);
        case ExternalModIssueSeverity::Warn:
            return ImVec4(0.95f, 0.8f, 0.35f, 1.0f);
        case ExternalModIssueSeverity::Error:
            return ImVec4(0.95f, 0.55f, 0.25f, 1.0f);
        case ExternalModIssueSeverity::Fatal:
            return ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
        default:
            return ImVec4(0.8f, 0.8f, 0.8f, 1.0f);
    }
}

std::string GetPackageHealthLabel(const ExternalModPackage& package) {
    if (!package.valid || package.severitySummary.fatal > 0) {
        return "Disabled (Fatal)";
    }
    if (!package.runtime.enabled) {
        return "Disabled";
    }
    if (package.severitySummary.error > 0) {
        return "Enabled (Errors)";
    }
    if (package.severitySummary.warn > 0) {
        return "Enabled (Warnings)";
    }
    return "Enabled";
}

ImVec4 GetPackageHealthColor(const ExternalModPackage& package) {
    if (!package.valid || package.severitySummary.fatal > 0) {
        return ImVec4(0.95f, 0.35f, 0.35f, 1.0f);
    }
    if (!package.runtime.enabled) {
        return ImVec4(0.75f, 0.75f, 0.75f, 1.0f);
    }
    if (package.severitySummary.error > 0) {
        return ImVec4(0.95f, 0.55f, 0.25f, 1.0f);
    }
    if (package.severitySummary.warn > 0) {
        return ImVec4(0.95f, 0.8f, 0.35f, 1.0f);
    }
    return ImVec4(0.35f, 0.85f, 0.45f, 1.0f);
}

std::string BuildPackageDiagnosticsText(const ExternalModPackage& package) {
    std::ostringstream stream;
    stream << (package.manifest.id.empty() ? "<unknown-id>" : package.manifest.id) << "\n";
    stream << "Health: " << GetPackageHealthLabel(package) << "\n";
    stream << "Summary: info=" << package.severitySummary.info << ", warn=" << package.severitySummary.warn
           << ", error=" << package.severitySummary.error << ", fatal=" << package.severitySummary.fatal << "\n";
    if (!package.error.empty()) {
        stream << "Reason: " << package.error << "\n";
    }
    for (const auto& issue : package.issues) {
        stream << "- [" << GetIssueSeverityLabel(issue.severity) << "] " << issue.code << ": " << issue.message << "\n";
        if (!issue.sourcePath.empty()) {
            stream << "  source: " << issue.sourcePath << "\n";
        }
        if (!issue.registryId.empty()) {
            stream << "  registry: " << issue.registryId << "\n";
        }
        if (!issue.referencedId.empty()) {
            stream << "  referencedId: " << issue.referencedId << "\n";
        }
        if (!issue.suggestedFix.empty()) {
            stream << "  fix: " << issue.suggestedFix << "\n";
        }
    }
    return stream.str();
}

const char* GetSettingDomainLabel(ExternalModSettingDomain domain) {
    switch (domain) {
        case ExternalModSettingDomain::Save:
            return "save";
        case ExternalModSettingDomain::Session:
            return "session";
        case ExternalModSettingDomain::Global:
        default:
            return "global";
    }
}

const char* GetSettingApplyModeLabel(ExternalModSettingApplyMode applyMode) {
    switch (applyMode) {
        case ExternalModSettingApplyMode::SceneReload:
            return "scene_reload";
        case ExternalModSettingApplyMode::Restart:
            return "restart";
        case ExternalModSettingApplyMode::Realtime:
        default:
            return "realtime";
    }
}

bool PackageDeclaresCapability(const ExternalModPackage& package, const std::string& capability) {
    const auto capabilityToken = ToLowerUi(capability);
    for (const auto& declaredCapability : package.manifest.capabilities) {
        if (ToLowerUi(declaredCapability) == capabilityToken) {
            return true;
        }
    }
    return false;
}

bool PackageDeclaresRegistryHint(const ExternalModPackage& package, const std::string& registryId) {
    const auto registryToken = ToLowerUi(registryId);
    for (const auto& providedRegistry : package.manifest.provides) {
        if (ToLowerUi(providedRegistry) == registryToken) {
            return true;
        }
    }
    for (const auto& usedRegistry : package.manifest.uses) {
        if (ToLowerUi(usedRegistry) == registryToken) {
            return true;
        }
    }
    return false;
}

bool ShouldRenderSettingEntry(const ExternalModPackage& package, const ExternalModSettingEntryDefinition& entry) {
    if (!entry.requiresCapability.empty() && !PackageDeclaresCapability(package, entry.requiresCapability)) {
        return false;
    }
    if (!entry.requiresRegistry.empty() && !PackageDeclaresRegistryHint(package, entry.requiresRegistry)) {
        return false;
    }
    return true;
}

bool TryParseSettingBoolUiValue(const std::string& value, bool& outValue) {
    const auto normalized = ToLowerUi(value);
    if (normalized == "1" || normalized == "true" || normalized == "yes" || normalized == "on") {
        outValue = true;
        return true;
    }
    if (normalized == "0" || normalized == "false" || normalized == "no" || normalized == "off") {
        outValue = false;
        return true;
    }
    return false;
}

bool TryParseSettingIntUiValue(const std::string& value, int32_t& outValue) {
    try {
        size_t parsedChars = 0;
        const auto parsed = std::stoi(value, &parsedChars);
        if (parsedChars != value.size()) {
            return false;
        }
        outValue = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

bool TryParseSettingFloatUiValue(const std::string& value, float& outValue) {
    try {
        std::string normalizedValue = value;
        std::replace(normalizedValue.begin(), normalizedValue.end(), ',', '.');
        size_t parsedChars = 0;
        const auto parsed = std::stof(normalizedValue, &parsedChars);
        if (parsedChars != normalizedValue.size()) {
            return false;
        }
        outValue = parsed;
        return true;
    } catch (...) {
        return false;
    }
}

std::string ResolveCurrentSettingValue(const ExternalModPackage& package, const ExternalModSettingEntryDefinition& entry) {
    std::string value;
    if (ExternalModManager::Instance().GetModSettingValue(package.manifest.id, entry.key, GetSettingDomainLabel(entry.domain),
                                                          value)) {
        return value;
    }
    return entry.defaultValue;
}

void EmitSettingsApplyFailureNotification(const ExternalModPackage& package, const ExternalModSettingEntryDefinition& entry,
                                          const std::string& error) {
    Notification::Emit({
        .message = "[ExternalMods] Failed setting " + package.manifest.id + ":" + entry.key + " -> " + error,
        .remainingTime = 5.0f,
    });
}

void DrawSettingEntryEditor(const ExternalModPackage& package, const ExternalModSettingEntryDefinition& entry) {
    auto& manager = ExternalModManager::Instance();
    const std::string value = ResolveCurrentSettingValue(package, entry);
    const std::string domainLabel = GetSettingDomainLabel(entry.domain);
    const std::string applyModeLabel = GetSettingApplyModeLabel(entry.applyMode);
    const bool sceneReloadPending = package.runtime.pendingSceneReloadSettings.contains(entry.key);
    const bool restartPending = package.runtime.pendingRestartSettings.contains(entry.key);

    ImGui::PushID(entry.key.c_str());
    std::string error;
    bool applied = false;

    switch (entry.type) {
        case ExternalModSettingValueType::Bool: {
            bool boolValue = false;
            if (!TryParseSettingBoolUiValue(value, boolValue)) {
                boolValue = false;
            }
            if (ImGui::Checkbox(entry.label.c_str(), &boolValue)) {
                applied = manager.SetModSettingValue(package.manifest.id, entry.key, boolValue ? "true" : "false",
                                                     domainLabel, applyModeLabel, error);
            }
            break;
        }
        case ExternalModSettingValueType::Int: {
            int32_t intValue = 0;
            if (!TryParseSettingIntUiValue(value, intValue)) {
                intValue = 0;
            }
            const auto minValue = entry.hasMin ? static_cast<int32_t>(std::floor(entry.minValue)) : intValue - 100;
            const auto maxValue = entry.hasMax ? static_cast<int32_t>(std::ceil(entry.maxValue)) : intValue + 100;
            if (entry.hasMin || entry.hasMax) {
                if (ImGui::SliderInt(entry.label.c_str(), &intValue, minValue, maxValue)) {
                    applied = manager.SetModSettingValue(package.manifest.id, entry.key, std::to_string(intValue),
                                                         domainLabel, applyModeLabel, error);
                }
            } else if (ImGui::InputInt(entry.label.c_str(), &intValue)) {
                applied = manager.SetModSettingValue(package.manifest.id, entry.key, std::to_string(intValue),
                                                     domainLabel, applyModeLabel, error);
            }
            break;
        }
        case ExternalModSettingValueType::Float: {
            float floatValue = 0.0f;
            if (!TryParseSettingFloatUiValue(value, floatValue)) {
                floatValue = 0.0f;
            }
            const float minValue = entry.hasMin ? entry.minValue : 0.0f;
            const float maxValue = entry.hasMax ? entry.maxValue : std::max(floatValue + 1.0f, 1.0f);
            const float stepValue = entry.hasStep ? entry.stepValue : 0.05f;
            if (entry.hasMin || entry.hasMax) {
                if (ImGui::SliderFloat(entry.label.c_str(), &floatValue, minValue, maxValue, "%.3f")) {
                    std::ostringstream stream;
                    stream.imbue(std::locale::classic());
                    stream << std::fixed << std::setprecision(4) << floatValue;
                    applied = manager.SetModSettingValue(package.manifest.id, entry.key, stream.str(), domainLabel,
                                                         applyModeLabel, error);
                }
            } else if (ImGui::DragFloat(entry.label.c_str(), &floatValue, stepValue)) {
                std::ostringstream stream;
                stream.imbue(std::locale::classic());
                stream << std::fixed << std::setprecision(4) << floatValue;
                applied = manager.SetModSettingValue(package.manifest.id, entry.key, stream.str(), domainLabel,
                                                     applyModeLabel, error);
            }
            break;
        }
        case ExternalModSettingValueType::Enum: {
            const char* previewValue = value.empty() ? "<unset>" : value.c_str();
            if (ImGui::BeginCombo(entry.label.c_str(), previewValue)) {
                for (const auto& enumValue : entry.enumValues) {
                    const bool selected = enumValue == value;
                    if (ImGui::Selectable(enumValue.c_str(), selected)) {
                        applied = manager.SetModSettingValue(package.manifest.id, entry.key, enumValue, domainLabel,
                                                             applyModeLabel, error);
                    }
                    if (selected) {
                        ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndCombo();
            }
            break;
        }
        case ExternalModSettingValueType::String:
        case ExternalModSettingValueType::Color:
        case ExternalModSettingValueType::Keybind:
        default: {
            char editableValue[512] = {};
            strncpy_s(editableValue, sizeof(editableValue), value.c_str(), _TRUNCATE);
            ImGui::InputText(entry.label.c_str(), editableValue, IM_ARRAYSIZE(editableValue));
            if (ImGui::IsItemDeactivatedAfterEdit()) {
                applied = manager.SetModSettingValue(package.manifest.id, entry.key, std::string(editableValue), domainLabel,
                                                     applyModeLabel, error);
            }
            break;
        }
    }

    if (!error.empty()) {
        EmitSettingsApplyFailureNotification(package, entry, error);
    }

    if (!entry.help.empty() && ImGui::IsItemHovered(ImGuiHoveredFlags_DelayShort)) {
        ImGui::SetTooltip("%s", entry.help.c_str());
    }

    ImGui::SameLine();
    if (ImGui::SmallButton("Reset")) {
        std::string resetError;
        if (!manager.ResetModSettingValue(package.manifest.id, entry.key, domainLabel, false, resetError)) {
            EmitSettingsApplyFailureNotification(package, entry, resetError);
        }
    }

    ImGui::TextDisabled("domain=%s apply=%s%s%s", domainLabel.c_str(), applyModeLabel.c_str(),
                        sceneReloadPending ? " pending-scene-reload" : "", restartPending ? " pending-restart" : "");
    if (entry.experimental) {
        ImGui::TextColored(ImVec4(0.95f, 0.8f, 0.35f, 1.0f), "experimental");
    }

    if (applied && entry.applyMode != ExternalModSettingApplyMode::Realtime) {
        Notification::Emit({
            .message = "[ExternalMods] Updated " + package.manifest.id + ":" + entry.key + " (" + applyModeLabel + ")",
            .remainingTime = 3.5f,
        });
    }

    ImGui::PopID();
}

void DrawPackageSettingsSection(const ExternalModPackage& package) {
    if (!package.manifest.settings.enabled || !package.runtime.settingsSchema.valid ||
        package.runtime.settingsSchema.groups.empty()) {
        return;
    }

    ImGui::Separator();
    ImGui::Text("Settings:");
    ImGui::TextDisabled("Schema v%d • domain default=%s", package.runtime.settingsSchema.version,
                        package.manifest.settings.defaultDomain.c_str());

    const std::string resetAllLabel = "Reset all settings##" + package.manifest.id;
    if (ImGui::SmallButton(resetAllLabel.c_str())) {
        std::string resetError;
        if (!ExternalModManager::Instance().ResetModSettingValue(package.manifest.id, std::string{}, std::string{}, true,
                                                                 resetError)) {
            Notification::Emit({
                .message = "[ExternalMods] Failed resetting settings for " + package.manifest.id + ": " + resetError,
                .remainingTime = 5.0f,
            });
        }
    }

    std::vector<const ExternalModSettingGroupDefinition*> visibleGroups;
    visibleGroups.reserve(package.runtime.settingsSchema.groups.size());
    for (const auto& group : package.runtime.settingsSchema.groups) {
        bool hasVisibleEntries = false;
        for (const auto& entry : group.entries) {
            if (ShouldRenderSettingEntry(package, entry)) {
                hasVisibleEntries = true;
                break;
            }
        }
        if (hasVisibleEntries) {
            visibleGroups.push_back(&group);
        }
    }

    std::stable_sort(visibleGroups.begin(), visibleGroups.end(),
                     [](const ExternalModSettingGroupDefinition* lhs, const ExternalModSettingGroupDefinition* rhs) {
                         if (lhs->order != rhs->order) {
                             return lhs->order < rhs->order;
                         }
                         return lhs->label < rhs->label;
                     });

    for (const auto* group : visibleGroups) {
        const std::string header = (group->label.empty() ? group->id : group->label) + "##" + package.manifest.id +
                                   ".settings." + group->id;
        if (ImGui::TreeNode(header.c_str())) {
            for (const auto& entry : group->entries) {
                if (!ShouldRenderSettingEntry(package, entry)) {
                    continue;
                }
                DrawSettingEntryEditor(package, entry);
            }
            ImGui::TreePop();
        }
    }
}

const char* GetRuntimePermissionTierLabel(const std::string& permission) {
    const std::string normalized = ToLowerUi(permission);
    if (normalized == "filesystem") {
        return "tier1";
    }
    if (normalized == "network" || normalized == "process" || normalized == "nativeinterop" ||
        normalized == "editor.livelink") {
        return "tier2";
    }
    if (normalized == "nativeinterop.raw" || normalized == "engine.memory") {
        return "tier3";
    }
    return "tier0";
}

int GetRuntimePermissionTierValue(const std::string& permission) {
    const std::string normalized = ToLowerUi(permission);
    if (normalized == "filesystem") {
        return 1;
    }
    if (normalized == "network" || normalized == "process" || normalized == "nativeinterop" ||
        normalized == "editor.livelink") {
        return 2;
    }
    if (normalized == "nativeinterop.raw" || normalized == "engine.memory") {
        return 3;
    }
    return 0;
}

const char* GetRuntimePermissionTierTooltip(const std::string& permission) {
    const std::string normalized = ToLowerUi(permission);
    if (normalized == "filesystem") {
        return "Can read/write external mod persistence files.";
    }
    if (normalized == "network") {
        return "Can communicate over the network.";
    }
    if (normalized == "process") {
        return "Can execute dev/process-level host actions.";
    }
    if (normalized == "nativeinterop") {
        return "Can call native interop host bridges.";
    }
    if (normalized == "nativeinterop.raw") {
        return "Can load raw native plugins with direct engine access.";
    }
    if (normalized == "engine.memory") {
        return "Can inspect or manipulate raw engine memory surfaces.";
    }
    if (normalized == "editor.livelink") {
        return "Can open local editor live-link channels.";
    }
    return "Low-impact runtime permission.";
}

std::vector<std::string> CollectDeniedRiskyPermissions(const ExternalModPackage& package) {
    std::vector<std::string> denied;
    denied.reserve(package.manifest.permissions.size());
    for (const auto& permission : package.manifest.permissions) {
        if (!ExternalModManager::IsRuntimePermissionRisky(permission)) {
            continue;
        }
        bool granted = false;
        if (!ExternalModManager::Instance().GetRuntimePermissionGrant(package.manifest.id, permission, granted)) {
            granted = false;
        }
        if (!granted) {
            denied.push_back(permission);
        }
    }
    return denied;
}

struct PendingPermissionPromptState {
    bool active = false;
    std::string modId;
    std::string modName;
    std::vector<std::string> deniedRiskyPermissions;
    int32_t remindLaterFrames = 0;
};

PendingPermissionPromptState& GetPendingPermissionPromptState() {
    static PendingPermissionPromptState state;
    return state;
}

void SaveUiCVarChangesSoon() {
    if (auto context = Ship::Context::GetRawInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                     context->GetWindow()->GetGui() != nullptr) {
        context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
}

void QueuePermissionPromptIfNeeded(const std::vector<ExternalModPackage>& packages) {
    auto& state = GetPendingPermissionPromptState();
    if (state.active) {
        return;
    }
    if (state.remindLaterFrames > 0) {
        --state.remindLaterFrames;
        return;
    }

    for (const auto& package : packages) {
        if (package.manifest.id.empty()) {
            continue;
        }
        if (!package.runtime.enabled) {
            const std::string errorLower = ToLowerUi(package.error);
            if (errorLower.find("permission") == std::string::npos &&
                errorLower.find("native runtime requires granted") == std::string::npos) {
                continue;
            }
        }
        const auto deniedRisky = CollectDeniedRiskyPermissions(package);
        if (deniedRisky.empty()) {
            continue;
        }

        const auto seenCVar = BuildPermissionPromptSeenCVarName(package.manifest.id);
        if (CVarGetInteger(seenCVar.c_str(), 0) != 0) {
            continue;
        }

        state.active = true;
        state.modId = package.manifest.id;
        state.modName = package.manifest.name.empty() ? package.manifest.id : package.manifest.name;
        state.deniedRiskyPermissions = deniedRisky;
        ImGui::OpenPopup("External Mods Permission Prompt");
        return;
    }
}

void DrawPermissionPromptModal(std::vector<ExternalModPackage>& packages) {
    auto& state = GetPendingPermissionPromptState();
    if (!state.active) {
        return;
    }

    if (!ImGui::BeginPopupModal("External Mods Permission Prompt", nullptr, ImGuiWindowFlags_AlwaysAutoResize)) {
        return;
    }

    ImGui::TextWrapped("Mod '%s' has risky permissions denied.", state.modName.c_str());
    ImGui::TextWrapped("Some features may not work until you grant them:");
    for (const auto& permission : state.deniedRiskyPermissions) {
        ImGui::BulletText("%s [%s]", permission.c_str(), GetRuntimePermissionTierLabel(permission));
    }

    const ExternalModPackage* packageRef = nullptr;
    for (const auto& package : packages) {
        if (package.manifest.id == state.modId) {
            packageRef = &package;
            break;
        }
    }
    if (packageRef != nullptr &&
        (packageRef->runtime.kind == ExternalModRuntimeKind::Native ||
         packageRef->runtime.kind == ExternalModRuntimeKind::Hybrid)) {
        ImGui::Spacing();
        ImGui::TextWrapped("This package includes native code. Review the requested permissions before granting access.");
    }
    if (packageRef != nullptr) {
        for (const auto& permission : state.deniedRiskyPermissions) {
            if (const auto it = packageRef->manifest.permissionRationales.find(permission);
                it != packageRef->manifest.permissionRationales.end() && !it->second.empty()) {
                ImGui::Indent();
                ImGui::TextWrapped("%s: %s", permission.c_str(), it->second.c_str());
                ImGui::Unindent();
            }
        }
    }

    if (ImGui::Button("Grant Risky Permissions")) {
        bool hasError = false;
        std::string firstError;
        for (const auto& permission : state.deniedRiskyPermissions) {
            std::string permissionError;
            if (!ExternalModManager::Instance().SetRuntimePermissionGrant(state.modId, permission, true, permissionError)) {
                hasError = true;
                if (firstError.empty()) {
                    firstError = permissionError;
                }
            }
        }
        if (hasError) {
            Notification::Emit({
                .message = "[ExternalMods] Failed granting risky permissions for " + state.modId + ": " + firstError,
                .remainingTime = 5.0f,
            });
        } else {
            CVarSetInteger(BuildPermissionPromptSeenCVarName(state.modId).c_str(), 1);
            SaveUiCVarChangesSoon();
            Notification::Emit({
                .message = "[ExternalMods] Granted risky permissions for " + state.modName + ".",
                .remainingTime = 4.0f,
            });
            state.active = false;
            state.modId.clear();
            state.modName.clear();
            state.deniedRiskyPermissions.clear();
            ImGui::CloseCurrentPopup();
        }
    }

    ImGui::SameLine();
    if (ImGui::Button("Keep Denied")) {
        CVarSetInteger(BuildPermissionPromptSeenCVarName(state.modId).c_str(), 1);
        SaveUiCVarChangesSoon();
        state.active = false;
        state.modId.clear();
        state.modName.clear();
        state.deniedRiskyPermissions.clear();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("Remind Later")) {
        state.active = false;
        state.remindLaterFrames = 600;
        state.modId.clear();
        state.modName.clear();
        state.deniedRiskyPermissions.clear();
        ImGui::CloseCurrentPopup();
    }

    ImGui::EndPopup();
}

std::vector<std::string> SplitEnabledPackList(const std::string& value) {
    std::vector<std::string> out;
    size_t start = 0;
    while (start < value.size()) {
        const size_t end = value.find('|', start);
        const std::string token = value.substr(start, end == std::string::npos ? std::string::npos : end - start);
        if (!token.empty()) {
            out.push_back(token);
        }
        if (end == std::string::npos) {
            break;
        }
        start = end + 1;
    }
    return out;
}

std::string JoinEnabledPackList(const std::vector<std::string>& values) {
    std::string out;
    for (const auto& value : values) {
        if (value.empty()) {
            continue;
        }
        if (!out.empty()) {
            out += "|";
        }
        out += value;
    }
    return out;
}

void PersistEnabledPackOrder(const std::vector<std::string>& enabledOrder) {
    const std::string value = JoinEnabledPackList(enabledOrder);
    CVarSetString(CVAR_SETTING("EnabledMods"), value.c_str());
    if (auto context = Ship::Context::GetRawInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                     context->GetWindow()->GetGui() != nullptr) {
        context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
    }
}

struct ResourcePackUiEntry {
    std::string key;
    std::string relativePath;
};

std::vector<ResourcePackUiEntry> DiscoverResourcePackEntries() {
    std::vector<ResourcePackUiEntry> entries;
    std::unordered_map<std::string, std::string> keyToRelativePath;

    const std::string modsPathString = Ship::Context::LocateFileAcrossAppDirs("mods", appShortName);
    if (modsPathString.empty() || !std::filesystem::exists(modsPathString)) {
        return entries;
    }

    const std::filesystem::path modsPath = std::filesystem::path(modsPathString);
    std::error_code ec;
    for (const auto& entry : std::filesystem::recursive_directory_iterator(
             modsPath, std::filesystem::directory_options::follow_directory_symlink, ec)) {
        if (ec || entry.is_directory()) {
            continue;
        }

        const std::string extension = ToLowerUi(entry.path().extension().string());
        if (extension != ".otr" && extension != ".o2r") {
            continue;
        }
        const std::string normalizedPath = ToLowerUi(entry.path().generic_string());
        if (normalizedPath.find("__external_mods_generated__") != std::string::npos) {
            continue;
        }

        const std::string key = entry.path().stem().generic_string();
        if (key.empty() || keyToRelativePath.contains(key)) {
            continue;
        }

        std::string relativePath = entry.path().filename().generic_string();
        std::error_code relativeError;
        const auto rel = std::filesystem::relative(entry.path(), modsPath, relativeError);
        if (!relativeError && !rel.empty()) {
            relativePath = rel.generic_string();
        }
        keyToRelativePath[key] = relativePath;
    }

    entries.reserve(keyToRelativePath.size());
    for (const auto& [key, relativePath] : keyToRelativePath) {
        entries.push_back({ key, relativePath });
    }

    std::sort(entries.begin(), entries.end(), [](const ResourcePackUiEntry& lhs, const ResourcePackUiEntry& rhs) {
        return lhs.key < rhs.key;
    });
    return entries;
}

void DrawResourcePacksTab() {
    auto entries = DiscoverResourcePackEntries();
    if (entries.empty()) {
        ImGui::TextDisabled("No standalone .otr/.o2r resourcepacks found in mods/.");
        return;
    }

    std::vector<std::string> enabledOrder = SplitEnabledPackList(CVarGetString(CVAR_SETTING("EnabledMods"), ""));
    auto findEnabledIndex = [&](const std::string& key) -> int32_t {
        const auto it = std::find(enabledOrder.begin(), enabledOrder.end(), key);
        if (it == enabledOrder.end()) {
            return -1;
        }
        return static_cast<int32_t>(std::distance(enabledOrder.begin(), it));
    };

    bool changed = false;
    for (const auto& entry : entries) {
        ImGui::PushID(entry.key.c_str());
        const int32_t enabledIndex = findEnabledIndex(entry.key);
        bool enabled = enabledIndex >= 0;

        if (ImGui::Checkbox("Enable", &enabled)) {
            changed = true;
            if (enabled) {
                if (enabledOrder.end() == std::find(enabledOrder.begin(), enabledOrder.end(), entry.key)) {
                    enabledOrder.push_back(entry.key);
                }
            } else {
                enabledOrder.erase(std::remove(enabledOrder.begin(), enabledOrder.end(), entry.key), enabledOrder.end());
            }
        }

        if (enabled) {
            const int32_t currentIndex = findEnabledIndex(entry.key);
            ImGui::SameLine();
            if (currentIndex <= 0) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Up")) {
                if (currentIndex > 0) {
                    std::iter_swap(enabledOrder.begin() + currentIndex, enabledOrder.begin() + (currentIndex - 1));
                    changed = true;
                }
            }
            if (currentIndex <= 0) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            if (currentIndex < 0 || currentIndex >= static_cast<int32_t>(enabledOrder.size()) - 1) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Button("Down")) {
                if (currentIndex >= 0 && currentIndex < static_cast<int32_t>(enabledOrder.size()) - 1) {
                    std::iter_swap(enabledOrder.begin() + currentIndex, enabledOrder.begin() + (currentIndex + 1));
                    changed = true;
                }
            }
            if (currentIndex < 0 || currentIndex >= static_cast<int32_t>(enabledOrder.size()) - 1) {
                ImGui::EndDisabled();
            }
        }

        ImGui::SameLine();
        ImGui::Text("%s", entry.key.c_str());
        ImGui::TextDisabled("Path: %s", entry.relativePath.c_str());
        ImGui::Separator();
        ImGui::PopID();
    }

    if (changed) {
        PersistEnabledPackOrder(enabledOrder);
        Notification::Emit({
            .message = "[ExternalMods] Resourcepack list updated. Restart may be required to apply asset pack order.",
            .remainingTime = 6.0f,
        });
    }
}

void DrawExternalModPackageRow(ExternalModPackage& package, const ExternalModUiPackageClassification& classification) {
    ImGui::PushID(package.manifest.id.c_str());
    ImGui::Separator();

    const bool runtimeEnabled = package.runtime.enabled;
    const bool manifestValid = package.valid;
    const std::string healthLabel = GetPackageHealthLabel(package);
    const ImVec4 statusColor = GetPackageHealthColor(package);

    ImGui::TextColored(statusColor, "%s", healthLabel.c_str());
    ImGui::SameLine();
    ImGui::Text("%s (%s)", package.manifest.name.empty() ? "<unnamed>" : package.manifest.name.c_str(),
                package.manifest.id.empty() ? "<unknown-id>" : package.manifest.id.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("[%s]", classification.badge.c_str());
    if (ImGui::IsItemHovered() && !classification.reason.empty()) {
        ImGui::SetTooltip("%s", classification.reason.c_str());
    }
    if (package.runtime.kind == ExternalModRuntimeKind::Native || package.runtime.kind == ExternalModRuntimeKind::Hybrid) {
        ImGui::SameLine();
        const char* nativeModeLabel =
            package.runtime.nativeMode == ExternalModNativeMode::Raw ? "native/raw" : "native/sdk";
        ImGui::TextDisabled("[%s]", nativeModeLabel);
        if (package.runtime.nativeUnsafeRaw && ImGui::IsItemHovered()) {
            ImGui::SetTooltip("Raw native plugins are build-coupled and require explicit risky grants.");
        }
    }

    if (!package.manifest.version.empty()) {
        ImGui::TextDisabled("Version: %s", package.manifest.version.c_str());
    }

    if (!package.manifest.capabilities.empty()) {
        std::string capabilitiesLine;
        for (size_t i = 0; i < package.manifest.capabilities.size(); ++i) {
            if (!capabilitiesLine.empty()) {
                capabilitiesLine += ", ";
            }
            capabilitiesLine += package.manifest.capabilities[i];
        }
        ImGui::TextDisabled("Capabilities: %s", capabilitiesLine.c_str());
    } else {
        ImGui::TextDisabled("Capabilities: none");
    }

    if (!package.manifest.capabilityRationales.empty()) {
        const std::string rationaleHeader = "Capability rationales##" + package.manifest.id;
        if (ImGui::TreeNode(rationaleHeader.c_str())) {
            for (const auto& capability : package.manifest.capabilities) {
                const auto rationaleIt = package.manifest.capabilityRationales.find(capability);
                if (rationaleIt == package.manifest.capabilityRationales.end() || rationaleIt->second.empty()) {
                    ImGui::BulletText("%s", capability.c_str());
                    ImGui::SameLine();
                    ImGui::TextDisabled("- no rationale provided");
                    continue;
                }

                ImGui::BulletText("%s", capability.c_str());
                ImGui::TextWrapped("%s", rationaleIt->second.c_str());
            }
            ImGui::TreePop();
        }
    }

    if (!package.manifest.permissions.empty()) {
        ImGui::Text("Permissions:");
        ImGui::TextDisabled("Tier1/Tier2 grants can be changed here and are stored per mod/profile.");

        auto applyPermissionPreset = [&](const char* presetLabel, auto&& decideGrant) {
            bool hasError = false;
            std::string firstError;
            for (const auto& permission : package.manifest.permissions) {
                bool currentGrant = false;
                if (!ExternalModManager::Instance().GetRuntimePermissionGrant(package.manifest.id, permission, currentGrant)) {
                    currentGrant = !ExternalModManager::IsRuntimePermissionRisky(permission);
                }
                const bool requestedGrant = decideGrant(permission, currentGrant);
                if (currentGrant == requestedGrant) {
                    continue;
                }

                std::string permissionError;
                if (!ExternalModManager::Instance().SetRuntimePermissionGrant(package.manifest.id, permission, requestedGrant,
                                                                              permissionError)) {
                    hasError = true;
                    if (firstError.empty()) {
                        firstError = permissionError;
                    }
                }
            }

            if (hasError) {
                Notification::Emit({
                    .message = "[ExternalMods] Permission preset failed for " + package.manifest.id + ": " + firstError,
                    .remainingTime = 5.0f,
                });
                return;
            }

            Notification::Emit({
                .message = "[ExternalMods] Applied permission preset '" + std::string(presetLabel) + "' for " +
                           package.manifest.id,
                .remainingTime = 4.0f,
            });
        };

        if (!manifestValid) {
            ImGui::BeginDisabled();
        }
        const std::string allowSafeLabel = "Allow Safe##permPreset.allowSafe." + package.manifest.id;
        if (ImGui::Button(allowSafeLabel.c_str())) {
            applyPermissionPreset("allow_safe", [](const std::string& permission, bool currentGrant) {
                return GetRuntimePermissionTierValue(permission) <= 1 ? true : currentGrant;
            });
        }
        ImGui::SameLine();
        const std::string denyRiskyLabel = "Deny Risky##permPreset.denyRisky." + package.manifest.id;
        if (ImGui::Button(denyRiskyLabel.c_str())) {
            applyPermissionPreset("deny_risky", [](const std::string& permission, bool currentGrant) {
                return GetRuntimePermissionTierValue(permission) >= 2 ? false : currentGrant;
            });
        }
        ImGui::SameLine();
        const std::string allowAllLabel = "Allow All##permPreset.allowAll." + package.manifest.id;
        if (ImGui::Button(allowAllLabel.c_str())) {
            applyPermissionPreset("allow_all", [](const std::string&, bool) { return true; });
        }
        ImGui::SameLine();
        const std::string resetLabel = "Reset Defaults##permPreset.reset." + package.manifest.id;
        if (ImGui::Button(resetLabel.c_str())) {
            applyPermissionPreset("reset_defaults", [](const std::string& permission, bool) {
                return !ExternalModManager::IsRuntimePermissionRisky(permission);
            });
        }
        if (!manifestValid) {
            ImGui::EndDisabled();
        }

        for (const auto& permission : package.manifest.permissions) {
            bool granted = false;
            if (!ExternalModManager::Instance().GetRuntimePermissionGrant(package.manifest.id, permission, granted)) {
                granted = !ExternalModManager::IsRuntimePermissionRisky(permission);
            }

            bool requestedGrant = granted;
            const std::string label = "##perm." + package.manifest.id + "." + permission;
            if (!manifestValid) {
                ImGui::BeginDisabled();
            }
            if (ImGui::Checkbox(label.c_str(), &requestedGrant)) {
                std::string permissionError;
                if (!ExternalModManager::Instance().SetRuntimePermissionGrant(package.manifest.id, permission, requestedGrant,
                                                                              permissionError)) {
                    Notification::Emit({
                        .message = "[ExternalMods] Permission update failed for " + package.manifest.id + ": " +
                                   permissionError,
                        .remainingTime = 5.0f,
                    });
                } else {
                    granted = requestedGrant;
                }
            }
            if (!manifestValid) {
                ImGui::EndDisabled();
            }

            ImGui::SameLine();
            ImGui::Text("%s", permission.c_str());
            ImGui::SameLine();
            ImGui::TextDisabled("[%s]", GetRuntimePermissionTierLabel(permission));
            if (ImGui::IsItemHovered()) {
                ImGui::SetTooltip("%s", GetRuntimePermissionTierTooltip(permission));
            }
            if (!granted) {
                ImGui::SameLine();
                ImGui::TextColored(ImVec4(0.95f, 0.45f, 0.45f, 1.0f), "denied");
            }

            if (const auto rationaleIt = package.manifest.permissionRationales.find(permission);
                rationaleIt != package.manifest.permissionRationales.end() && !rationaleIt->second.empty()) {
                ImGui::Indent();
                ImGui::TextWrapped("Why: %s", rationaleIt->second.c_str());
                ImGui::Unindent();
            }
        }
    } else {
        ImGui::TextDisabled("Permissions: none");
    }

    if (!package.runtime.moduleSourcePath.empty()) {
        ImGui::TextDisabled("Runtime module: %s (%s)", package.runtime.moduleSourcePath.c_str(),
                            GetExternalModRuntimeModuleFormatName(package.runtime.moduleFormat));
        ImGui::TextDisabled("Compiled wasm size: %zu bytes", package.runtime.compiledModuleSizeBytes);
        if (package.runtime.moduleFormat == ExternalModRuntimeModuleFormat::WatText) {
            ImGui::TextDisabled("WAT compile time: %d ms", package.runtime.moduleCompileTimeMs);
        }
        if (!package.runtime.moduleCompileDiagnostics.empty()) {
            ImGui::TextWrapped("Runtime diagnostics: %s", package.runtime.moduleCompileDiagnostics.c_str());
        }
    }
    if (!package.runtime.nativeLibrarySourcePath.empty()) {
        ImGui::TextDisabled("Native library: %s", package.runtime.nativeLibrarySourcePath.c_str());
        if (!package.runtime.nativeLibraryLoadedPath.empty()) {
            ImGui::TextDisabled("Loaded path: %s", package.runtime.nativeLibraryLoadedPath.c_str());
        }
        ImGui::TextDisabled("Native ABI: v%d • mode=%s • buildIdPolicy=%s", package.runtime.nativeAbiVersion,
                            package.runtime.nativeMode == ExternalModNativeMode::Raw ? "raw" : "sdk",
                            package.runtime.nativeBuildIdPolicy.c_str());
        if (!package.runtime.nativePluginName.empty()) {
            ImGui::TextDisabled("Plugin: %s %s", package.runtime.nativePluginName.c_str(),
                                package.runtime.nativePluginVersion.empty() ? "" : package.runtime.nativePluginVersion.c_str());
        }
        if (!package.runtime.nativePluginAuthor.empty()) {
            ImGui::TextDisabled("Author: %s", package.runtime.nativePluginAuthor.c_str());
        }
        if (!package.runtime.nativePluginBuildId.empty()) {
            ImGui::TextDisabled("Plugin buildId: %s", package.runtime.nativePluginBuildId.c_str());
        }
    }

    const auto enabledCVar = ExternalModManager::BuildEnabledCVarName(package.manifest.id);
    bool modEnabled = CVarGetInteger(enabledCVar.c_str(), 1) != 0;

    if (!manifestValid) {
        ImGui::BeginDisabled();
    }
    if (ImGui::Checkbox((std::string("Enable##") + package.manifest.id).c_str(), &modEnabled)) {
        CVarSetInteger(enabledCVar.c_str(), modEnabled ? 1 : 0);
        package.runtime.enabled = manifestValid && modEnabled;
        if (auto context = Ship::Context::GetRawInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                         context->GetWindow()->GetGui() != nullptr) {
            context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
        }
    }
    if (!manifestValid) {
        ImGui::EndDisabled();
    }

    if (!package.error.empty()) {
        ImGui::TextWrapped("Reason: %s", package.error.c_str());
        const std::string errorLower = ToLowerUi(package.error);
        if (errorLower.find("quarantined") != std::string::npos && !package.manifest.id.empty()) {
            const std::string clearLabel = "Clear Quarantine##" + package.manifest.id;
            if (ImGui::Button(clearLabel.c_str())) {
                std::string clearError;
                if (!ExternalModManager::Instance().ClearRuntimeQuarantineForMod(package.manifest.id, clearError)) {
                    Notification::Emit({
                        .message = "[ExternalMods] Failed clearing quarantine for " + package.manifest.id + ": " + clearError,
                        .remainingTime = 6.0f,
                    });
                } else {
                    CVarSetInteger(BuildPermissionPromptSeenCVarName(package.manifest.id).c_str(), 0);
                    SaveUiCVarChangesSoon();
                    Notification::Emit({
                        .message = "[ExternalMods] Cleared quarantine for " + package.manifest.id +
                                   ". Click Reload External Mods to retry.",
                        .remainingTime = 6.0f,
                    });
                }
            }
        }
    }

    if (!package.issues.empty()) {
        ImGui::TextDisabled("Health summary: info=%d warn=%d error=%d fatal=%d", package.severitySummary.info,
                            package.severitySummary.warn, package.severitySummary.error, package.severitySummary.fatal);
        const std::string copyLabel = "Copy diagnostics##" + package.manifest.id;
        if (ImGui::Button(copyLabel.c_str())) {
            const std::string diagnosticsText = BuildPackageDiagnosticsText(package);
            ImGui::SetClipboardText(diagnosticsText.c_str());
            Notification::Emit({
                .message = "[ExternalMods] Copied diagnostics for " + package.manifest.id,
                .remainingTime = 3.0f,
            });
        }

        const std::string diagnosticsHeader = "Diagnostics##" + package.manifest.id;
        if (ImGui::TreeNode(diagnosticsHeader.c_str())) {
            for (const auto& issue : package.issues) {
                ImGui::PushID(static_cast<int>((&issue) - package.issues.data()));
                ImGui::TextColored(GetIssueSeverityColor(issue.severity), "[%s] %s", GetIssueSeverityLabel(issue.severity),
                                   issue.code.c_str());
                ImGui::SameLine();
                ImGui::TextWrapped("%s", issue.message.c_str());
                if (!issue.sourcePath.empty()) {
                    ImGui::TextDisabled("Source: %s", issue.sourcePath.c_str());
                }
                if (!issue.registryId.empty()) {
                    ImGui::TextDisabled("Registry: %s", issue.registryId.c_str());
                }
                if (!issue.referencedId.empty()) {
                    ImGui::TextDisabled("Referenced id: %s", issue.referencedId.c_str());
                }
                if (!issue.suggestedFix.empty()) {
                    ImGui::TextWrapped("Suggested fix: %s", issue.suggestedFix.c_str());
                }
                ImGui::Separator();
                ImGui::PopID();
            }
            ImGui::TreePop();
        }
    }

    if (!package.runtime.hookSubscriptions.empty()) {
        ImGui::Text("Hook subscriptions: %zu", package.runtime.hookSubscriptions.size());
        ImGui::TextDisabled("Hook budget/frame: %d used of %d", package.runtime.hookCallsThisFrame,
                            package.runtime.maxHookCallsPerFrame);
        ImGui::TextDisabled("WASM calls/frame: %d, budget drops/frame: %d", package.runtime.wasmCallsThisFrame,
                            package.runtime.wasmBudgetDropsThisFrame);
        for (const auto& subscription : package.runtime.hookSubscriptions) {
            ImGui::BulletText("%s hook=%s dispatch=%s cooldown=%d", subscription.id.c_str(),
                              GetExternalModHookTypeName(subscription.hook),
                              GetExternalModHookDispatchName(subscription.dispatch), subscription.cooldownFrames);
        }
    }

    if (!package.runtime.actorDefinitions.empty() || !package.runtime.actorInstances.empty()) {
        ImGui::Text("Actor VM: defs=%zu instances=%zu/%d", package.runtime.actorDefinitions.size(),
                    package.runtime.actorInstances.size(), package.runtime.maxActorInstances);
        for (const auto& actorDefinition : package.runtime.actorDefinitions) {
            ImGui::BulletText("%s archetype=%s scene=%d max=%d tick=%d", actorDefinition.id.c_str(),
                              GetExternalModActorArchetypeName(actorDefinition.archetype), actorDefinition.sceneId,
                              actorDefinition.maxInstances, actorDefinition.tickRate);
        }
    }

    if (!package.runtime.meshDefinitions.empty() || !package.runtime.prefabDefinitions.empty() ||
        !package.runtime.worldInstanceDefinitions.empty() || !package.runtime.editorPlacementDefinitions.empty() ||
        !package.runtime.worldAuthoringDefinitions.empty()) {
        ImGui::Text("Visual Authoring: meshes=%zu prefabs=%zu instances=%zu placements=%zu authored=%zu",
                    package.runtime.meshDefinitions.size(), package.runtime.prefabDefinitions.size(),
                    package.runtime.worldInstanceDefinitions.size(), package.runtime.editorPlacementDefinitions.size(),
                    package.runtime.worldAuthoringDefinitions.size());
    }

    if (!package.runtime.itemDefinitions.empty()) {
        ImGui::Text("Items:");
        for (const auto& item : package.runtime.itemDefinitions) {
            const char* slotName = GetExternalModItemSlotName(item.slot);
            ImGui::BulletText("%s (%s) slot=%s mode=%s age=%s", item.displayName.c_str(), item.id.c_str(), slotName,
                              GetExternalModItemUseModeName(item.useMode),
                              GetExternalModItemAgePolicyName(item.agePolicy));
            if (!item.description.empty()) {
                ImGui::TextDisabled("desc: %s", item.description.c_str());
            }
            if (!item.iconAsset.empty()) {
                ImGui::TextDisabled("icon: %s", item.iconAsset.c_str());
            }
            if (!item.modelAsset.empty()) {
                ImGui::TextDisabled("model: %s (triangles=%zu)", item.modelAsset.c_str(), item.customModelTriangles.size());
            }
            if (!item.modelDisplayList.empty()) {
                ImGui::TextDisabled("model dlist: %s", item.modelDisplayList.c_str());
            }
            if (!item.modelTextureAsset.empty()) {
                ImGui::TextDisabled("model texture: %s", item.modelTextureAsset.c_str());
            }
            if (!item.hookshotMetalTextureAsset.empty() || !item.hookshotHandleTextureAsset.empty() ||
                !item.hookshotDesignTextureAsset.empty() || !item.hookshotChainTextureAsset.empty() ||
                !item.hookshotReticleTextureAsset.empty()) {
                ImGui::TextDisabled("hookshot gameplay textures:");
                if (!item.hookshotMetalTextureAsset.empty()) {
                    ImGui::TextDisabled("  metal: %s", item.hookshotMetalTextureAsset.c_str());
                }
                if (!item.hookshotHandleTextureAsset.empty()) {
                    ImGui::TextDisabled("  handle: %s", item.hookshotHandleTextureAsset.c_str());
                }
                if (!item.hookshotDesignTextureAsset.empty()) {
                    ImGui::TextDisabled("  design: %s", item.hookshotDesignTextureAsset.c_str());
                }
                if (!item.hookshotChainTextureAsset.empty()) {
                    ImGui::TextDisabled("  chain: %s", item.hookshotChainTextureAsset.c_str());
                }
                if (!item.hookshotReticleTextureAsset.empty()) {
                    ImGui::TextDisabled("  reticle: %s", item.hookshotReticleTextureAsset.c_str());
                }
            }
            if (item.hasGrantItemId || item.hasGrantAmmo) {
                const std::string grantItemText = item.hasGrantItemId ? std::to_string(item.grantItemId) : "<slot-default>";
                const std::string grantAmmoText = item.hasGrantAmmo ? std::to_string(item.grantAmmo) : "<unchanged>";
                ImGui::TextDisabled("grant: itemId=%s ammo=%s", grantItemText.c_str(), grantAmmoText.c_str());
            }
            ImGui::TextDisabled("state: granted=%s cooldown=%d/%d", item.granted ? "yes" : "no",
                                item.cooldownRemaining, item.cooldownFrames);
        }
    }

    if (!package.runtime.renderInspectorDefinitions.empty()) {
        auto& manager = ExternalModManager::Instance();
        const bool debugGateActive = manager.IsRenderInspectorDebugGateActive();
        ImGui::Separator();
        ImGui::Text("Render Inspector:");
        ImGui::TextDisabled("%s",
                            debugGateActive
                                ? "Inspector active: notification summary + fixed overlay while volumetrics debug is enabled."
                                : "Inspector gated: enable volumetrics debug to activate notifications and fixed overlay.");

        for (const auto& inspector : package.runtime.renderInspectorDefinitions) {
            bool visible = false;
            manager.GetRenderInspectorOverlayVisible(package.manifest.id, inspector.id, visible);
            bool requestedVisible = visible;
            const std::string checkboxId = "Show##renderInspector." + package.manifest.id + "." + inspector.id;
            if (ImGui::Checkbox(checkboxId.c_str(), &requestedVisible)) {
                std::string visibilityError;
                if (!manager.SetRenderInspectorOverlayVisible(package.manifest.id, inspector.id, requestedVisible,
                                                              visibilityError)) {
                    Notification::Emit({
                        .message = "[ExternalMods] Failed toggling render inspector for " + package.manifest.id + ": " +
                                   visibilityError,
                        .remainingTime = 5.0f,
                    });
                } else {
                    SaveUiCVarChangesSoon();
                }
            }
            ImGui::SameLine();
            ImGui::Text("%s", inspector.id.c_str());
            ImGui::TextDisabled("features: material=%s lightBudget=%s postFx=%s",
                                inspector.showMaterialUnderCursor ? "on" : "off",
                                inspector.showLightBudget ? "on" : "off", inspector.showPostFxState ? "on" : "off");
        }

        if (debugGateActive) {
            const std::string summary = manager.GetRenderInspectorSummaryText();
            if (!summary.empty()) {
                const std::string copyLabel = "Copy render summary##" + package.manifest.id;
                if (ImGui::SmallButton(copyLabel.c_str())) {
                    ImGui::SetClipboardText(summary.c_str());
                    Notification::Emit({
                        .message = "[ExternalMods] Copied render inspector summary.",
                        .remainingTime = 3.0f,
                    });
                }
                const std::string summaryHeader = "Current render summary##" + package.manifest.id;
                if (ImGui::TreeNode(summaryHeader.c_str())) {
                    ImGui::PushTextWrapPos(0.0f);
                    ImGui::TextUnformatted(summary.c_str());
                    ImGui::PopTextWrapPos();
                    ImGui::TreePop();
                }
            }
        }
    }

    if (!package.runtime.inputBindings.empty()) {
        ImGui::Text("Bindings:");
        ImGui::TextDisabled("Tip: map to Mod Action buttons, then bind any keyboard/gamepad key in Settings > Controls > Modifier Buttons.");
        for (const auto& binding : package.runtime.inputBindings) {
            ImGui::PushID(binding.id.c_str());
            const auto cvarName = ExternalModManager::BuildBindingCVarName(package.manifest.id, binding.id);
            const auto label = std::string("Binding: ") + binding.id + "##" + package.manifest.id + "." + binding.id;
            UIWidgets::CVarBtnSelector(label.c_str(), cvarName.c_str(),
                                       UIWidgets::BtnSelectorOptions()
                                           .DefaultValue(binding.defaultMask)
                                           .Color(UIWidgets::Colors::LightBlue)
                                           .Tooltip("External mod action binding (supports combinations)"));
            ImGui::PopID();
        }
    } else {
        ImGui::TextDisabled("No input bindings for this mod.");
    }

    DrawPackageSettingsSection(package);

    ImGui::PopID();
}

void DrawExternalModControlsSection() {
    if (!ImGui::CollapsingHeader("External Mods (ZIP)", ImGuiTreeNodeFlags_DefaultOpen)) {
        return;
    }

    auto& packages = ExternalModManager::Instance().GetPackages();
    if (packages.empty()) {
        ImGui::TextDisabled("No external mods discovered in mods/.");
        return;
    }

    QueuePermissionPromptIfNeeded(packages);
    DrawPermissionPromptModal(packages);

    ImGui::TextDisabled("Toggle and bindings are saved. Use Reload External Mods after editing ZIP/pasta manifests or scripts.");

    if (ImGui::Button("Open Extra Inventory (I)")) {
        if (auto context = Ship::Context::GetRawInstance(); context != nullptr && context->GetWindow() != nullptr &&
                                                     context->GetWindow()->GetGui() != nullptr) {
            if (auto window = context->GetWindow()->GetGui()->GetGuiWindow("External Mod Inventory"); window != nullptr) {
                window->ToggleVisibility();
                context->GetWindow()->GetGui()->SaveConsoleVariablesNextFrame();
            }
        }
    }
    ImGui::SameLine();
    ImGui::TextDisabled("Grid livre para itens concedidos por mods e equip rapido nos slots comuns.");

    ImGui::SameLine();
    if (ImGui::Button("Reload External Mods")) {
        std::string reloadMessage;
        ExternalModManager::Instance().ReloadPackages(reloadMessage);
        Notification::Emit({
            .message = reloadMessage.empty() ? "[ExternalMods] Reload completed." : ("[ExternalMods] Reload completed with warnings: " + reloadMessage),
            .remainingTime = 8.0f,
        });
    }

    if (ImGui::BeginTabBar("ExternalContentTabs")) {
        if (ImGui::BeginTabItem("Resourcepacks")) {
            DrawResourcePacksTab();
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Mods")) {
            bool hasRows = false;
            for (auto& package : packages) {
                const auto classification = ClassifyExternalModPackage(package);
                if (classification.tab != ExternalModUiPackageTab::Mods) {
                    continue;
                }
                hasRows = true;
                DrawExternalModPackageRow(package, classification);
            }
            if (!hasRows) {
                ImGui::TextDisabled("No content mods discovered.");
            }
            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("Core + API")) {
            bool hasRows = false;
            for (auto& package : packages) {
                const auto classification = ClassifyExternalModPackage(package);
                if (classification.tab != ExternalModUiPackageTab::CoreApi) {
                    continue;
                }
                hasRows = true;
                DrawExternalModPackageRow(package, classification);
            }
            if (!hasRows) {
                ImGui::TextDisabled("No framework/core API packages discovered.");
            }
            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();
    }
}

} // namespace SOH

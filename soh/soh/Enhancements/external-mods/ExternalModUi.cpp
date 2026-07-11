#include "ExternalModUi.h"

#include <algorithm>
#include <cctype>
#include <filesystem>
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
        default:
            return "unknown";
    }
}

std::string ToLowerUi(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
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
    const ImVec4 statusColor = runtimeEnabled ? ImVec4(0.35f, 0.85f, 0.45f, 1.0f)
                                              : (manifestValid ? ImVec4(0.95f, 0.78f, 0.25f, 1.0f)
                                                               : ImVec4(0.95f, 0.35f, 0.35f, 1.0f));

    ImGui::TextColored(statusColor, "%s", runtimeEnabled ? "Enabled" : "Disabled");
    ImGui::SameLine();
    ImGui::Text("%s (%s)", package.manifest.name.empty() ? "<unnamed>" : package.manifest.name.c_str(),
                package.manifest.id.empty() ? "<unknown-id>" : package.manifest.id.c_str());
    ImGui::SameLine();
    ImGui::TextDisabled("[%s]", classification.badge.c_str());
    if (ImGui::IsItemHovered() && !classification.reason.empty()) {
        ImGui::SetTooltip("%s", classification.reason.c_str());
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

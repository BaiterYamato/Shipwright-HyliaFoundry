#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

struct GetItemEntry;
struct PlayState;
struct Player;
#include "ExternalModTypes.h"
#include "ExternalModInterop.h"
#include "ExternalModItemRuntime.h"
#include "ExternalModEffectRuntime.h"
#include "ExternalModActorTagRuntime.h"
#include "ExternalModWorldGraphicsRuntime.h"
#include "ExternalModPlayerResourcesRuntime.h"

namespace SOH {

struct ExternalModWasmGroundInfo;
struct ExternalModWasmRaycastHit;

class ExternalModManager {
  public:
    static ExternalModManager& Instance();

    void Initialize();
    void Shutdown();
    bool ReloadPackages(std::string& outError);
    // Returns true (and clears the flag) when a deferred runtime reload was requested, e.g. after a
    // permission grant changed or a save was loaded. Must only be consumed from a point outside
    // GameInteractor::ExecuteHooks (see ExternalModInventoryWindow::UpdateElement), because
    // ReloadPackages re-registers hooks and would invalidate the hook iteration otherwise.
    bool TakePendingRuntimeReload();
    void DiscoverPackages();
    std::vector<ExternalModPackage>& GetPackages();
    const std::vector<ExternalModPackage>& GetPackages() const;
    void ApplyGetItemVisualOverrides(::GetItemEntry& entry) const;
    std::vector<ExternalModInventoryCellView> GetExtraInventoryGrid() const;
    int32_t GetExtraInventoryPageCount() const;
    bool GetExtraInventoryPageCell(int32_t pageIndex, int32_t pageCellIndex, ExternalModInventoryCellView& outCell) const;
    bool MoveExtraInventoryCell(size_t fromIndex, size_t toIndex, std::string& outError);
    bool EquipExtraInventoryCellToButton(size_t cellIndex, int32_t buttonIndex, std::string& outError);
    void OnVanillaButtonEquipped(int32_t buttonIndex);
    bool TryDrawButtonOverrideIcon(::PlayState* play, int32_t buttonIndex, int32_t alpha) const;
    bool DrawSurfBoardIfActive(::PlayState* play, ::Player* player);
    bool OnHammerGroundImpact(::PlayState* play, ::Player* player, float impactX, float impactY, float impactZ);
    bool DrawAimReticleIfActive(::PlayState* play, ::Player* player, ExternalModAimCameraContext context) const;
    int16_t ResolveAimCameraMode(::PlayState* play, ::Player* player, int16_t defaultMode,
                                 ExternalModAimCameraContext context) const;
    bool HandleGlobalHotkeyScancode(int32_t scancode);
    bool HandleCameraHotkeyScancode(int32_t scancode);
    bool IsAimMouseFireHeld(::PlayState* play, ::Player* player, int32_t heldItemAction) const;
    bool GetRuntimePermissionGrant(const std::string& modId, const std::string& permission, bool& outGranted) const;
    bool SetRuntimePermissionGrant(const std::string& modId, const std::string& permission, bool granted,
                                   std::string& outError);
    static bool IsRuntimePermissionRisky(const std::string& permission);
    bool ClearRuntimeQuarantineForMod(const std::string& modId, std::string& outError);
    bool GetModSettingValue(const std::string& modId, const std::string& key, const std::string& domain,
                            std::string& outValue) const;
    bool SetModSettingValue(const std::string& modId, const std::string& key, const std::string& value,
                            const std::string& domain, const std::string& applyMode, std::string& outError);
    bool ResetModSettingValue(const std::string& modId, const std::string& key, const std::string& domain, bool resetAll,
                              std::string& outError);
    int32_t HandleAimSelectSlotPress(::PlayState* play, ::Player* player, int32_t buttonIndex, int32_t itemId);
    bool IsAimAttackButtonFireEnabled(::PlayState* play, ::Player* player) const;
    bool IsPlayerFreezeNoDamageActive(::Player* player) const;
    bool IsAimOverShoulderEnabled() const;
    bool ShouldUseAimOverShoulderBattleMovement(::PlayState* play, ::Player* player, int32_t heldItemAction) const;
    bool TryConsumePlayerResourceActionStart(::PlayState* play, ::Player* player, const std::string& actionTag);
    bool TickPlayerResourceAction(::PlayState* play, ::Player* player, const std::string& actionTag, float deltaSeconds);
    bool IsPlayerResourceActionInputActive(::PlayState* play, ::Player* player, const std::string& actionTag,
                                           ExternalModInputTriggerType trigger) const;
    float GetPlayerResourceMoveSpeedMultiplier(::PlayState* play, ::Player* player) const;
    bool IsPlayerResourceSprintBlocked(::PlayState* play, ::Player* player) const;
    int32_t GetPlayerResourceRingViewCount(::PlayState* play, ::Player* player) const;
    bool GetPlayerResourceRingView(::PlayState* play, ::Player* player, int32_t index,
                                   ExternalModsResourceRingView& outView) const;
    bool TryFillBottleFromWater(::PlayState* play, ::Player* player, int32_t& outPlaceholderItemId);
    bool IsHeldBottleCustomContentActive(::PlayState* play, ::Player* player) const;
    bool ConsumeHeldBottleCustomContent(::PlayState* play, ::Player* player);
    bool IsHeldInventoryConsumableActive(::PlayState* play, ::Player* player) const;
    bool ConsumeHeldInventoryConsumable(::PlayState* play, ::Player* player);
    bool TryHandleGrassDrop(::PlayState* play, float x, float y, float z, int32_t grassType, int32_t dropParams);
    bool HasCustomEquippedSlingshotModel() const;
    bool DrawCustomEquippedSlingshotModel(::PlayState* play) const;
    static std::string BuildEnabledCVarName(const std::string& modId);
    static std::string BuildBindingCVarName(const std::string& modId, const std::string& bindingId);
    static std::string BuildRenderInspectorOverlayCVarName(const std::string& modId, const std::string& overlayId);
    static std::string BuildCameraHotkeyScancodeCVarName(const std::string& modId, const std::string& hotkeyId);
    static std::string BuildHotkeyScancodeCVarName(const std::string& modId, const std::string& hotkeyId);
    static void ExecuteActionsPublic(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                                     const char* triggerName);
    void EmitExtendedHook(ExternalModHookType hookType, const ExternalModHookEventContext& context,
                          const char* triggerName);
    bool TryConsumePendingSceneLoadRequest(int16_t sceneId, ExternalModPendingSceneLoadRequest& outRequest);
    void HandlePendingSceneLoadSuccess(const ExternalModPendingSceneLoadRequest& request);
    void HandlePendingSceneLoadFailure(const ExternalModPendingSceneLoadRequest& request, const std::string& error);
    void LoadPersistentInventoryState();
    void SavePersistentInventoryState() const;
    bool TryResolveMaterialAlbedoOverridePath(const std::string& texturePath, int16_t sceneId, int16_t roomId,
                                              std::string& outOverridePath) const;
    uint32_t GetMaterialFallbackGenerationFlags(const std::string& texturePath, int16_t sceneId, int16_t roomId) const;
    float GetMaterialFallbackNormalScale(const std::string& texturePath, int16_t sceneId, int16_t roomId) const;
    void GetMaterialRuntimeStats(int16_t sceneId, int16_t roomId, size_t& outActiveOverrides,
                                 size_t& outBoundDefinitions, size_t& outBoundAlbedoDefinitions) const;
    bool GetRenderInspectorOverlayVisible(const std::string& modId, const std::string& overlayId, bool& outVisible) const;
    bool SetRenderInspectorOverlayVisible(const std::string& modId, const std::string& overlayId, bool visible,
                                          std::string& outError);
    bool IsRenderInspectorDebugGateActive() const;
    bool HasVisibleRenderInspectorOverlay() const;
    std::string GetRenderInspectorSummaryText() const;
    bool QueryPublicModJson(const std::string& requesterModId, const std::string& requestJson, std::string& outJson,
                            std::string& outError) const;
    bool CallPublicService(const std::string& requesterModId, const std::string& targetModId,
                           const std::string& serviceId, const std::string& requestJson, std::string& outJson,
                           std::string& outError) const;
    bool InvokeOwnActionJson(const std::string& modId, const std::string& actionJson, std::string& outJson,
                             std::string& outError);
    void DrawPrefabVisuals(::PlayState* play);

  private:
    struct ExtraInventoryCell {
        std::string modId;
        std::string itemId;
    };

    enum class ActionButtonSource {
        Vanilla,
        Mod,
    };

    struct ActionButtonAssignment {
        ActionButtonSource source = ActionButtonSource::Vanilla;
        std::string modId;
        std::string itemId;
    };

    enum class AimSelectSlotPressResult {
        None = 0,
        Activated = 1,
        DeactivatedConsumed = 2,
    };

    struct AimSelectState {
        bool active = false;
        std::string modId;
        std::string itemId;
        int32_t buttonIndex = -1;
        int32_t resolvedItemId = -1;
    };

    std::vector<ExternalModPackage> mPackages;
    ExternalModPendingSceneLoadRequest mPendingSceneLoadRequest;
    std::vector<ExtraInventoryCell> mExtraInventoryCells;
    std::array<ActionButtonAssignment, 8> mActionButtonAssignments{};
    int32_t mExtraInventoryPage = 0;
    int32_t mExtraInventoryCursor = 0;
    bool mPersistentInventoryDirty = false;
    bool mSaveSectionRegistered = false;
    bool mRuntimeReloadPending = false;
    bool mReloadingPackages = false;
    int32_t mPersistentInventorySectionId = -1;
    ExternalModAimCameraState mAimCameraState{};
    AimSelectState mAimSelectState{};
    uint32_t mOnLoadGameHook = 0;
    uint32_t mOnExitGameHook = 0;
    uint32_t mOnSceneInitHook = 0;
    uint32_t mAfterSceneCommandsHook = 0;
    uint32_t mOnTransitionEndHook = 0;
    uint32_t mOnFlagSetHook = 0;
    uint32_t mOnFlagUnsetHook = 0;
    uint32_t mOnSceneFlagSetHook = 0;
    uint32_t mOnSceneFlagUnsetHook = 0;
    uint32_t mOnPlayerUpdateHook = 0;
    uint32_t mOnGameFrameHook = 0;
    uint32_t mOnPlayDrawBeginHook = 0;
    uint32_t mOnPlayDrawEndHook = 0;
    uint32_t mOnPlayerUseItemHook = 0;
    uint32_t mOnPlayerHealthChangeHook = 0;
    uint32_t mOnItemReceiveHook = 0;
    uint32_t mOnActorInitHook = 0;
    uint32_t mOnActorSpawnHook = 0;
    uint32_t mOnActorUpdateHook = 0;
    uint32_t mOnActorKillHook = 0;
    uint32_t mOnActorDestroyHook = 0;
    uint32_t mOnEnemyDefeatHook = 0;
    uint32_t mOnBossDefeatHook = 0;
    uint32_t mOnPlayDestroyHook = 0;
    std::unique_ptr<ExternalModWorldGraphicsRuntime> mWorldGraphicsRuntime;
    std::unique_ptr<ExternalModPlayerResourcesRuntime> mPlayerResourcesRuntime;
    std::unique_ptr<ExternalModItemRuntime> mItemStateRuntime;
    std::unique_ptr<ExternalModEffectRuntime> mEffectRuntime;
    std::unique_ptr<ExternalModActorTagRuntime> mActorTagRuntime;

    static bool TryParseManifest(const std::string& content, ExternalModManifest& outManifest, std::string& outError);
    static bool TryParseEntryScript(const std::string& content, int32_t apiVersion, ExternalModRuntime& outRuntime,
                                    std::string& outError);
    static bool TryParseItemDefinitions(const std::string& content, std::vector<ExternalModItemDefinition>& outItems,
                                        std::string& outError);
    static bool TryParseItemStateDefinitions(const std::string& content, int32_t apiVersion,
                                             const std::string& modId,
                                             std::vector<ExternalModItemStateMachineDefinition>& outDefinitions,
                                             std::string& outError);
    static bool TryParseEffectGraphDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModEffectGraphDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseCombatHitRuleDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModCombatHitRuleDefinition>& outDefinitions,
                                                 std::string& outError);
    static bool TryParseActorTagDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModActorTagDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseInputDefinitions(const std::string& content, std::vector<ExternalModInputBinding>& outBindings,
                                         std::vector<ExternalModCameraHotkeyDefinition>& outCameraHotkeys,
                                         std::vector<ExternalModHotkeyDefinition>& outHotkeys,
                                         std::string& outError);
    static bool TryParseHookDefinitions(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModHookSubscription>& outSubscriptions,
                                        std::string& outError);
    static bool TryParseActorDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModActorDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseBehaviorDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModBehaviorDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseSceneDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModSceneDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseStatusDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModStatusDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseDamageDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModDamageProfile>& outDefinitions,
                                          std::string& outError);
    static bool TryParseTargetingDefinitions(const std::string& content, int32_t apiVersion,
                                             std::vector<ExternalModTargetingProfile>& outDefinitions,
                                             std::string& outError);
    static bool TryParseItemUseProfiles(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModItemUseProfile>& outDefinitions,
                                        std::string& outError);
    static bool TryParseProjectileDefinitions(const std::string& content, int32_t apiVersion,
                                              std::vector<ExternalModProjectileProfile>& outDefinitions,
                                              std::string& outError);
    static bool TryParseAoEDefinitions(const std::string& content, int32_t apiVersion,
                                       std::vector<ExternalModAoEProfile>& outDefinitions, std::string& outError);
    static bool TryParseMovementDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModMovementProfile>& outDefinitions,
                                            std::string& outError);
    static bool TryParseCameraDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModAimCameraProfile>& outDefinitions,
                                          std::string& outError);
    static bool TryParseUiScreenDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModUiScreenDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseUiHudDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModUiHudLayoutDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParsePlayerResourceDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModPlayerResourceDefinition>& outDefinitions,
                                                  std::string& outError);
    static bool TryParseResourceRingDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModResourceRingDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParsePlayerConsumableDefinitions(const std::string& content, int32_t apiVersion,
                                                    std::vector<ExternalModPlayerConsumableDefinition>& outDefinitions,
                                                    std::string& outError);
    static bool TryParseWorldForageDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModWorldForageRuleDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseInventoryExtensionDefinitions(const std::string& content, int32_t apiVersion,
                                                      std::vector<ExternalModInventoryPageDefinition>& outDefinitions,
                                                      std::string& outError);
    static bool TryParseContainerDefinitions(const std::string& content, int32_t apiVersion,
                                             std::vector<ExternalModContainerDefinition>& outDefinitions,
                                             std::string& outError);
    static bool TryParseRecipeDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModProcessingRecipeDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseInteractionDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModInteractionDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseActorArchetypeDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModActorArchetypeDefinition>& outDefinitions,
                                                  std::string& outError);
    static bool TryParseActorAdapterDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModActorAdapterDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseBehaviorTreeDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModBehaviorTreeDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseSensorDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModSensorDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseRouteDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModRouteDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseNavBridgeDefinitions(const std::string& content, int32_t apiVersion,
                                             std::vector<ExternalModNavBridgeDefinition>& outDefinitions,
                                             std::string& outError);
    static bool TryParseDebugOverlayDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModDebugOverlayDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseMaterialDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModMaterialDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParsePbrDefinitions(const std::string& content, int32_t apiVersion,
                                       std::vector<ExternalModPbrDefinition>& outDefinitions,
                                       std::string& outError);
    static bool TryParseLightingDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModLightProfileDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParsePostFxDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModPostFxPresetDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseSceneProfileDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModSceneProfileDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseRoomProfileDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModRoomProfileDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseAssetPackDefinitions(const std::string& content, int32_t apiVersion,
                                             std::vector<ExternalModAssetPackDefinition>& outDefinitions,
                                             std::string& outError);
    static bool TryParseAssetSourceDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModAssetSourceDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseMeshDefinitions(const std::string& content, int32_t apiVersion,
                                        std::vector<ExternalModMeshDefinition>& outDefinitions,
                                        std::string& outError);
    static bool TryParsePrefabDefinitions(const std::string& content, int32_t apiVersion,
                                          std::vector<ExternalModPrefabDefinition>& outDefinitions,
                                          std::string& outError);
    static bool TryParseWorldInstanceDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModWorldInstanceDefinition>& outDefinitions,
                                                 std::string& outError);
    static bool TryParseEditorPlacementDefinitions(const std::string& content, int32_t apiVersion,
                                                   std::vector<ExternalModEditorPlacementDefinition>& outDefinitions,
                                                   std::string& outError);
    static bool TryParseWorldAuthoringDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModWorldAuthoringDefinition>& outDefinitions,
                                                  std::string& outError);
    static bool TryParseRenderInspectorDefinitions(const std::string& content, int32_t apiVersion,
                                                   std::vector<ExternalModRenderInspectorDefinition>& outDefinitions,
                                                   std::string& outError);
    static bool TryParseFxPresetDefinitions(const std::string& content, int32_t apiVersion,
                                            std::vector<ExternalModFxPresetDefinition>& outDefinitions,
                                            std::string& outError);
    static bool TryParseStateDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModStateDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseSpellDefinitions(const std::string& content, int32_t apiVersion,
                                         std::vector<ExternalModSpellDefinition>& outDefinitions,
                                         std::string& outError);
    static bool TryParseWorldPersistenceDefinitions(const std::string& content, int32_t apiVersion,
                                                    std::vector<ExternalModWorldPersistenceDefinition>& outDefinitions,
                                                    std::string& outError);
    static bool TryParseWorldStorageDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModWorldStorageDomainDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseWorldSpawnProfileDefinitions(const std::string& content, int32_t apiVersion,
                                                     std::vector<ExternalModWorldSpawnProfileDefinition>& outDefinitions,
                                                     std::string& outError);
    static bool TryParseWorldTimeWeatherDefinitions(const std::string& content, int32_t apiVersion,
                                                    std::vector<ExternalModWorldTimeWeatherDefinition>& outDefinitions,
                                                    std::string& outError);
    static bool TryParseWorldSeedingDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModWorldSeedingDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseWorldMigrationDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModWorldMigrationDefinition>& outDefinitions,
                                                  std::string& outError);
    static bool TryParsePersistenceInspectorDefinitions(
        const std::string& content, int32_t apiVersion, std::vector<ExternalModPersistenceInspectorDefinition>& outDefinitions,
        std::string& outError);
    static bool TryParseNarrativeFlagDefinitions(const std::string& content, int32_t apiVersion,
                                                 std::vector<ExternalModNarrativeFlagDefinition>& outDefinitions,
                                                 std::string& outError);
    static bool TryParseNarrativeDialogueDefinitions(const std::string& content, int32_t apiVersion,
                                                     std::vector<ExternalModNarrativeDialogueDefinition>& outDefinitions,
                                                     std::string& outError);
    static bool TryParseNarrativeQuestDefinitions(const std::string& content, int32_t apiVersion,
                                                  std::vector<ExternalModNarrativeQuestDefinition>& outDefinitions,
                                                  std::string& outError);
    static bool TryParseNarrativeTimelineDefinitions(const std::string& content, int32_t apiVersion,
                                                     std::vector<ExternalModNarrativeTimelineDefinition>& outDefinitions,
                                                     std::string& outError);
    static bool TryParseNarrativeInspectorDefinitions(
        const std::string& content, int32_t apiVersion, std::vector<ExternalModNarrativeInspectorDefinition>& outDefinitions,
        std::string& outError);
    static bool TryParseDevHotReloadDefinitions(const std::string& content, int32_t apiVersion,
                                                std::vector<ExternalModDevHotReloadDefinition>& outDefinitions,
                                                std::string& outError);
    static bool TryParseDevConsoleDefinitions(const std::string& content, int32_t apiVersion,
                                              std::vector<ExternalModDevConsoleCommandDefinition>& outDefinitions,
                                              std::string& outError);
    static bool TryParseDevWatcherDefinitions(const std::string& content, int32_t apiVersion,
                                              std::vector<ExternalModDevWatcherDefinition>& outDefinitions,
                                              std::string& outError);
    static bool TryParseWasmSandboxDefinitions(const std::string& content, int32_t apiVersion,
                                               std::vector<ExternalModWasmSandboxDefinition>& outDefinitions,
                                               std::string& outError);
    static bool TryParseReloadInspectorDefinitions(
        const std::string& content, int32_t apiVersion, std::vector<ExternalModReloadInspectorDefinition>& outDefinitions,
        std::string& outError);
    static bool TryParseSettingsSchema(const std::string& content, ExternalModSettingsSchemaDefinition& outSchema,
                                       std::string& outError);

    static bool ReadManifestFromDirectory(const std::filesystem::path& dirPath, std::string& outContent,
                                          std::filesystem::path& outManifestRelativePath, std::string& outError);
    static bool ReadManifestFromZip(const std::filesystem::path& zipPath, std::string& outContent,
                                    std::filesystem::path& outManifestRelativePath, std::string& outError);

    static bool ReadFileFromDirectory(const std::filesystem::path& filePath, uint64_t maxBytes, std::string& outContent,
                                      std::string& outError);
    static bool ReadFileFromZip(const std::filesystem::path& zipPath, const std::filesystem::path& packageRelativePath,
                                uint64_t maxBytes, std::vector<char>& outBytes, std::string& outError);
    static std::filesystem::path GetPackageDataRoot(const ExternalModPackage& package);
    static std::filesystem::path GetPackageRuntimeRoot(const ExternalModPackage& package);
    static bool ResolvePackageFilePath(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath,
                                       std::filesystem::path& outRoot, std::filesystem::path& outFullPath,
                                       std::string& outError);
    static std::filesystem::path ResolveZipPathForPackage(const ExternalModPackage& package,
                                                          const std::filesystem::path& packageRelativePath);
    static bool ReadFileFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath,
                                    uint64_t maxBytes, std::string& outContent, std::string& outError);
    static bool ReadBinaryFromPackage(const ExternalModPackage& package, const std::filesystem::path& packageRelativePath,
                                      uint64_t maxBytes, std::vector<uint8_t>& outBytes, std::string& outError);
    static bool PreparePackageFileForHostAccess(const ExternalModPackage& package,
                                                const std::filesystem::path& packageRelativePath, uint64_t maxBytes,
                                                const char* categoryName, std::filesystem::path& outPreparedPath,
                                                bool& outFromCache, std::string& outError);

    static void UnmountAssetsForPackage(ExternalModPackage& package);
    static bool MountAssetsForPackage(ExternalModPackage& package, std::string& outError);
    static bool LoadDslDocumentsForPackage(const ExternalModPackage& package, ExternalModRuntime& runtime,
                                           std::string& outError);
    static bool LoadRuntimeForPackage(ExternalModPackage& package, std::string& outError);
    static bool IsSafePackageRelativePath(const std::string& pathValue, std::filesystem::path& outNormalizedPath,
                                          std::string& outError);
    static void ExecuteActions(ExternalModPackage& package, const std::vector<ExternalModAction>& actions,
                               const char* triggerName);
    static void DisableRuntime(ExternalModPackage& package, const std::string& reason);
    bool GrantConsumableStack(ExternalModPackage& package, const std::string& consumableId, int32_t amount,
                              const char* triggerName);
    bool ConsumeConsumableStack(ExternalModPackage& package, const std::string& consumableId, int32_t amount,
                                const char* triggerName, bool* outConsumed = nullptr);
    void SyncExtraInventoryGrid();
    void SyncButtonAssignments();
    void ApplyDefaultKeyboardMappingsForPackage(const ExternalModPackage& package) const;
    const ExternalModAimCameraProfile* FindAimCameraProfileById(const std::string& modId,
                                                                const std::string& profileId) const;
    const ExternalModAimCameraProfile* ResolveActiveAimCameraProfile() const;
    const ExternalModAimCameraProfile* ResolveAimCameraProfileForContext(ExternalModAimCameraContext context,
                                                                         bool requireMouseFire) const;
    void PruneAimCameraStateForUnavailableProfiles();
    void ClearAimSelectState(bool disableOverShoulder);
    const ExternalModItemDefinition* FindAimSelectItemDefinition(const std::string& modId,
                                                                 const std::string& itemId) const;
    bool TryInvokeAssignedModItem(int32_t buttonIndex, ::PlayState* play, ::Player* player);
    const ExternalModPackage* FindPackageByModId(const std::string& modId) const;
    ExternalModPackage* FindPackageByModId(const std::string& modId);
    const ExternalModItemDefinition* FindItemByAssignment(const ActionButtonAssignment& assignment) const;
    ExternalModItemDefinition* FindItemByAssignment(ActionButtonAssignment& assignment);
    bool IsAssignmentValid(const ActionButtonAssignment& assignment) const;
    void MarkPersistentInventoryDirty();
    void ProcessAssignedActionButtons(::PlayState* play, ::Player* player, void* input);
    int32_t WasmHostUseItemProfile(const std::string& modId, const std::string& itemOrProfileId);
    int32_t WasmHostResolveTarget(const std::string& modId, const std::string& profileId,
                                  std::vector<int32_t>& outHandles);
    int32_t WasmHostDealDamage(const std::string& modId, int32_t targetHandle, const std::string& damageProfileId);
    int32_t WasmHostApplyStatus(const std::string& modId, int32_t targetHandle, const std::string& statusId,
                                int32_t durationOverrideFrames);
    int32_t WasmHostSpawnProjectile(const std::string& modId, const std::string& profileId,
                                    const std::string& overridesJson);
    int32_t WasmHostSpawnAoE(const std::string& modId, const std::string& profileId, const std::string& originJson);
    int32_t WasmHostApplyMovementProfile(const std::string& modId, const std::string& profileId, int32_t durationFrames);
    int32_t WasmHostApplyImpulse(const std::string& modId, int32_t mode, float strength, float x, float y, float z);
    int32_t WasmHostGetGroundInfo(const std::string& modId, ExternalModWasmGroundInfo& outInfo);
    int32_t WasmHostRaycast(const std::string& modId, const std::string& queryJson, ExternalModWasmRaycastHit& outHit);
    int32_t WasmHostRaycastAll(const std::string& modId, const std::string& queryJson, int32_t outCapacity,
                               std::vector<ExternalModWasmRaycastHit>& outHits);
    int32_t WasmHostQueryPublicJson(const std::string& modId, const std::string& requestJson, std::string& outJson);
    int32_t WasmHostCallService(const std::string& modId, const std::string& targetModId, const std::string& serviceId,
                                const std::string& requestJson, std::string& outJson);
    int32_t WasmHostInvokeActionJson(const std::string& modId, const std::string& actionJson, std::string& outJson);
    void DispatchExtendedHook(ExternalModHookType hookType, const ExternalModHookEventContext& context,
                              const char* triggerName);

    void RegisterHooks();
    void UnregisterHooks();
    void OnLoadGame(int32_t fileNum);
    void OnExitGame(int32_t fileNum);
    void OnSceneInit(int16_t sceneNum);
    void OnAfterSceneCommands(int16_t sceneNum);
    void OnTransitionEnd(int16_t sceneNum);
    void OnFlagSet(int16_t flagType, int16_t flag);
    void OnFlagUnset(int16_t flagType, int16_t flag);
    void OnSceneFlagSet(int16_t sceneNum, int16_t flagType, int16_t flag);
    void OnSceneFlagUnset(int16_t sceneNum, int16_t flagType, int16_t flag);
    void OnPlayerUpdate();
    void OnGameFrameUpdate();
    void OnPlayDrawBegin();
    void OnPlayDrawEnd();
    void OnPlayerUseItem(void* player, int32_t itemId, bool* allowVanilla);
    void OnPlayerHealthChange(int16_t amount);
    void OnItemReceive(int16_t itemId);
    void OnActorHook(ExternalModHookType hookType, void* actor, const char* hookName);
    void OnPlayDestroy();

    friend class ExternalModParser;
};

} // namespace SOH

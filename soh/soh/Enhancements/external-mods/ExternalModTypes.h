#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Ship {
class Archive;
}

namespace SOH {

class ExternalModWasmRuntime;
class ExternalModNativeRuntime;

enum class ExternalModInputTriggerType {
    Pressed,
    Held,
    Released,
};

enum class ExternalModItemSlot {
    Stick,
    Nut,
    Bomb,
    Bow,
    FireArrow,
    DinsFire,
    Slingshot,
    Ocarina,
    Bombchu,
    Hookshot,
    IceArrow,
    FaroresWind,
    Boomerang,
    Lens,
    Bean,
    Hammer,
    LightArrow,
    NayrusLove,
    Bottle1,
    Bottle2,
    Bottle3,
    Bottle4,
    TradeAdult,
    TradeChild,
};

enum class ExternalModItemAgePolicy {
    RespectVanilla,
    AllowChild,
    AllowAdult,
};

enum class ExternalModItemUseMode {
    Vanilla,
    Override,
    Augment,
};

enum class ExternalModItemUseTrigger {
    OnUse,
    HammerGroundImpact,
};

enum class ExternalModItemPlacement {
    Legacy,
    Virtual,
};

enum class ExternalModMovementMode {
    Modifier,
    Surf,
};

enum class ExternalModAimCameraContext {
    CUp,
    Bow,
    Hookshot,
    Slingshot,
    Boomerang,
};

enum class ExternalModAimCameraMode {
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimMouseButton {
    Left,
    Middle,
    Right,
    Backward,
    Forward,
};

enum class ExternalModAimMouseFireMode {
    Both,
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimReticleVisibility {
    AimOnly,
    ButtonHold,
    Selected,
};

enum class ExternalModModelUvOrigin {
    Auto,
    BottomLeft,
    TopLeft,
};

enum class ExternalModModelTextureFilter {
    Auto,
    Point,
    Bilerp,
};

enum class ExternalModActorArchetype {
    Npc,
    Prop,
    Trigger,
};

enum class ExternalModRuntimeModuleFormat {
    WasmBinary,
    WatText,
    NativeLibrary,
};

enum class ExternalModRuntimeKind {
    DataOnly,
    Wasm,
    Native,
    Hybrid,
};

enum class ExternalModNativeMode {
    None,
    Sdk,
    Raw,
};

enum class ExternalModIssueSeverity {
    Info,
    Warn,
    Error,
    Fatal,
};

struct ExternalModIssue {
    ExternalModIssueSeverity severity = ExternalModIssueSeverity::Info;
    std::string code;
    std::string message;
    std::string sourcePath;
    std::string registryId;
    std::string referencedId;
    std::string suggestedFix;
};

struct ExternalModSeveritySummary {
    int32_t info = 0;
    int32_t warn = 0;
    int32_t error = 0;
    int32_t fatal = 0;
};

enum class ExternalModSettingValueType {
    Bool,
    Int,
    Float,
    Enum,
    String,
    Color,
    Keybind,
};

enum class ExternalModSettingDomain {
    Global,
    Save,
    Session,
};

enum class ExternalModSettingApplyMode {
    Realtime,
    SceneReload,
    Restart,
};

struct ExternalModSettingEntryDefinition {
    std::string key;
    std::string label;
    std::string help;
    std::string defaultValue;
    ExternalModSettingValueType type = ExternalModSettingValueType::String;
    float minValue = 0.0f;
    float maxValue = 0.0f;
    float stepValue = 0.0f;
    bool hasMin = false;
    bool hasMax = false;
    bool hasStep = false;
    std::vector<std::string> enumValues;
    ExternalModSettingDomain domain = ExternalModSettingDomain::Global;
    ExternalModSettingApplyMode applyMode = ExternalModSettingApplyMode::Realtime;
    std::string requiresCapability;
    std::string requiresRegistry;
    bool experimental = false;
};

struct ExternalModSettingGroupDefinition {
    std::string id;
    std::string label;
    int32_t order = 0;
    std::vector<ExternalModSettingEntryDefinition> entries;
};

struct ExternalModSettingsSchemaDefinition {
    bool valid = false;
    int32_t version = 1;
    std::vector<ExternalModSettingGroupDefinition> groups;
};

struct ExternalModManifest {
    struct SharePolicy {
        std::string read = "public";
        std::string write = "owner";
        std::string overrideAccess = "owner";
        std::string nativeService = "public";
        std::vector<std::string> denyMods;
        std::vector<std::string> friends;
    };

    struct ImportDefinition {
        std::string modId;
        std::string namespaceId;
        std::string access = "read";
        bool optional = false;
    };

    struct ExportDefinition {
        std::string id;
        std::string namespaceId;
        std::string kind;
        std::string source;
        std::string visibility = "public";
        std::string mutableBy = "owner";
        std::vector<std::string> friends;
        std::vector<std::string> denyMods;
    };

    struct ServiceEndpointDefinition {
        std::string id;
        std::string kind;
        std::string entry;
        std::string visibility = "public";
        std::string mutableBy = "owner";
        std::vector<std::string> friends;
        std::vector<std::string> denyMods;
    };

    std::string id;
    std::string name;
    std::string version;
    std::string schemaVersion = "mod.manifest.v2";
    std::string type = "content";
    std::string uiCategory = "mod";
    int32_t apiVersion = 0;
    std::string gameVersionMin;
    std::string engineVersionRange;
    std::string entryScript;
    std::string entryLibrary;
    std::vector<std::string> assets;
    struct Dependency {
        std::string modId;
        std::string versionRange;
    };
    std::vector<Dependency> dependencies;
    std::vector<std::string> releaseChannels;
    std::unordered_map<std::string, std::string> capabilityRationales;
    std::unordered_map<std::string, std::string> permissionRationales;
    std::vector<std::string> permissions;
    std::vector<std::string> provides;
    std::vector<std::string> uses;
    SharePolicy sharePolicy;
    std::vector<ImportDefinition> imports;
    std::vector<ExportDefinition> exports;
    std::vector<ServiceEndpointDefinition> serviceEndpoints;
    struct FilesPolicy {
        std::vector<std::string> optional;
    } files;
    struct SettingsPolicy {
        bool enabled = false;
        int32_t schemaVersion = 0;
        std::string schemaFile;
        std::string defaultDomain = "global";
    } settings;
    int32_t loadOrder = 0;
    int32_t loadPriority = 0;
    struct Entrypoints {
        std::string items;
        std::string combat;
        std::string movement;
        std::string camera;
        std::string ui;
        std::string actors;
        std::string quests;
        std::string wasm;
    } entrypoints;

    std::string runtimeType;
    std::string runtimeModule;
    int32_t runtimeAbiVersion = 1;
    std::string runtimeNativeMode = "sdk";
    std::string runtimeReloadPolicy = "manual";
    std::string runtimeBuildIdPolicy = "ignore";
    std::string runtimeThreadModel = "main_thread";
    int32_t runtimeMaxMemoryKb = 1024;
    int32_t runtimeMaxCallMs = 2;
    int32_t runtimeMaxFrameBudgetMs = 2;
    int32_t runtimeMaxHookCallsPerFrame = 256;
    int32_t runtimeMaxActorInstances = 64;
    int32_t runtimeMaxActiveStatuses = 256;
    std::string itemDefinitions;
    std::string inputDefinitions;
    std::string hookDefinitions;
    std::string actorDefinitions;
    std::string behaviorDefinitions;
    std::string sceneDefinitions;
    std::string statusDefinitions;
    std::string damageDefinitions;
    std::string targetingDefinitions;
    std::string projectileDefinitions;
    std::string aoeDefinitions;
    std::string movementDefinitions;
    std::string itemUseProfiles;
    std::string vanillaItemPatches;
    std::string cameraDefinitions;
    std::string itemStateDefinitions;
    std::string equippedModelDefinitions;
    std::string hudWidgetDefinitions;
    std::string hudReticleDefinitions;
    std::string uiScreenDefinitions;
    std::string uiHudDefinitions;
    std::string playerResourceDefinitions;
    std::string resourceRingDefinitions;
    std::string playerConsumableDefinitions;
    std::string worldForageDefinitions;
    std::string inventoryExtensionDefinitions;
    std::string containerDefinitions;
    std::string recipeDefinitions;
    std::string interactionDefinitions;
    std::string actorArchetypeDefinitions;
    std::string actorAdapterDefinitions;
    std::string behaviorTreeDefinitions;
    std::string sensorDefinitions;
    std::string routeDefinitions;
    std::string navBridgeDefinitions;
    std::string debugOverlayDefinitions;
    std::string effectGraphDefinitions;
    std::string combatHitRuleDefinitions;
    std::string surfDefinitions;
    std::string actorTagDefinitions;
    std::string worldPatchDefinitions;
    std::string questDefinitions;
    std::string dialogDefinitions;
    std::string sdkGeneratorDefinitions;
    std::string fxPresetDefinitions;
    std::string stateDefinitions;
    std::string spellDefinitions;
    std::string materialDefinitions;
    std::string pbrDefinitions;
    std::string lightingDefinitions;
    std::string postFxDefinitions;
    std::string sceneProfileDefinitions;
    std::string roomProfileDefinitions;
    std::string assetPackDefinitions;
    std::string assetSourceDefinitions;
    std::string meshDefinitions;
    std::string prefabDefinitions;
    std::string worldInstanceDefinitions;
    std::string renderInspectorDefinitions;
    std::string editorRuntimeDefinitions;
    std::string editorUiDefinitions;
    std::string editorSelectionDefinitions;
    std::string editorGizmoDefinitions;
    std::string editorLibraryDefinitions;
    std::string editorProjectDefinitions;
    std::string editorPlacementDefinitions;
    std::string assetImporterDefinitions;
    std::string worldAuthoringDefinitions;
    std::string collisionAuthoringDefinitions;
    std::string editorInspectorDefinitions;
    std::string worldPersistenceDefinitions;
    std::string worldStorageDefinitions;
    std::string worldSpawnProfileDefinitions;
    std::string worldTimeWeatherDefinitions;
    std::string worldSeedingDefinitions;
    std::string worldMigrationDefinitions;
    std::string persistenceInspectorDefinitions;
    std::string narrativeTimelineDefinitions;
    std::string narrativeDialogueDefinitions;
    std::string narrativeQuestDefinitions;
    std::string narrativeFlagDefinitions;
    std::string narrativeInspectorDefinitions;
    std::string devHotReloadDefinitions;
    std::string devConsoleDefinitions;
    std::string devWatcherDefinitions;
    std::string wasmSandboxDefinitions;
    std::string reloadInspectorDefinitions;
    std::vector<std::string> capabilities;
};

struct ExternalModDslDocument {
    std::string id;
    std::string category;
    std::string subtype;
    std::string sourcePath;
    std::string visibility = "public";
    std::string mutableBy = "owner";
    std::vector<std::string> friends;
    std::vector<std::string> denyMods;
    std::string rawJson;
};

enum class ExternalModActionType {
    ShowNotification,
    TeleportToEntrance,
    LoadModScene,
    PressButton,
    ShowEquippedItemGet,
    SpawnSmoke,
    SpawnKusa,
    LanternLight,
    ApplyStatus,
    UseItemProfile,
    DealDamage,
    SpawnProjectile,
    SpawnAoE,
    ApplyMovementProfile,
    ApplyImpulse,
    GetGroundInfo,
    Raycast,
    RaycastAll,
    ClearStatus,
    ClearAllStatuses,
    SpawnActor,
    DespawnActor,
    SetActorState,
    MoveActorToPathNode,
    OpenDialog,
    SetSwitchFlag,
    ClearSwitchFlag,
    SetEventChkInf,
    ClearEventChkInf,
    SetInfTable,
    ClearInfTable,
    GiveRupees,
    TakeRupees,
    GrantModItem,
    RevokeModItem,
    SetVar,
    AddVar,
    ClampVar,
    EmitSignal,
    CallBehavior,
    ToggleAimCameraMode,
    SetAimCameraMode,
    SetAimCameraProfile,
    InvokeWasm,
    FxSpawnEffectSs,
    FxSpawnActorFx,
    FxSpawnPreset,
    FxStopFx,
    StatesApplyState,
    StatesClearState,
    StatesHasState,
    PlayerGetStateFlags,
    PlayerSetStateFlag,
    PlayerClearStateFlag,
    PlayerSetControlLock,
    PlayerSetGravityScale,
    PlayerSetBoostType,
    PlayerSetDamageResponse,
    SpellsCastSpell,
    UiOpenScreen,
    UiCloseScreen,
    UiToggleScreen,
    UiFocusNext,
    UiFocusPrev,
    InventoryExtCreatePage,
    InventoryExtMoveItem,
    InventoryExtSave,
    InventoryExtLoad,
    ContainerOpen,
    ContainerMoveItem,
    ContainerStartProcess,
    ContainerCancelProcess,
    ContainerGetProgress,
    ActorsSpawnArchetype,
    ActorsDespawnArchetype,
    ActorsToggleArchetype,
    InteractionsInvoke,
    AiRunBehavior,
    AiSetBlackboard,
    AiClearBlackboard,
    SenseFindTargets,
    SenseLineOfSight,
    SenseDistance,
    NavRequestPath,
    NavGetPathPoints,
    NavReleasePath,
    DebugShowOverlay,
    DebugHideOverlay,
    WorldSetSceneProfile,
    WorldSetRoomProfile,
    RenderSetPostFxPreset,
    RenderSpawnLight,
    RenderDespawnLight,
    RenderSetSkylight,
    RenderClearPostFxPreset,
    RenderClearSkylight,
    RenderOverrideMaterial,
    RenderClearMaterialOverrides,
    PersistEnsureEntityGuid,
    PersistSaveEntityState,
    PersistLoadEntityState,
    PersistDeleteEntityState,
    PersistSetDomainValue,
    PersistGetDomainValue,
    PersistRunMigrations,
    WorldSpawnFromProfile,
    WorldTimeSetOverride,
    WorldWeatherSetOverride,
    NarrativeStartDialogue,
    NarrativeChooseOption,
    NarrativeAdvanceDialogue,
    NarrativeSetFlag,
    NarrativeClearFlag,
    NarrativeStartQuest,
    NarrativeUpdateObjective,
    NarrativeStartTimeline,
    NarrativeSkipTimeline,
    DevReloadAll,
    DevReloadTarget,
    DevConsoleExec,
    SettingsGet,
    SettingsSet,
    SettingsReset,
    SettingsList,
    SetResourceValue,
    AddResourceValue,
    ConsumeResource,
    RefillResource,
    SetResourceCapacity,
    GrantConsumableStack,
    ConsumeConsumableStack,
    FillActiveBottleContent,
};

enum class ExternalModStatusType {
    Fire,
    Freeze,
    Stun,
    Poison,
    Blind,
    Speed,
    Slow,
    HighJump,
    Strength,
    Weakness,
    Custom,
};

enum class ExternalModStatusTarget {
    FrontTarget,
    Self,
    Player,
    ActorHandle,
};

struct ExternalModAction {
    ExternalModActionType type = ExternalModActionType::ShowNotification;
    std::string text;
    int16_t entranceIndex = 0;
    std::string modSceneId;
    int32_t sceneSpawnId = 0;
    int32_t buttonMask = 0;
    std::string itemId;
    std::string exportName;
    std::vector<int32_t> args;

    std::string actorDefinitionId;
    uint32_t actorHandle = 0;
    std::string actorStateKey;
    std::string actorStateValue;
    int32_t pathNodeIndex = 0;
    int32_t dialogId = 0;
    int32_t intValue = 0;

    std::string variableScope;
    std::string variableKey;
    std::string variableValue;
    float variableNumber = 0.0f;
    float variableMin = 0.0f;
    float variableMax = 0.0f;
    bool variableHasNumber = false;
    bool variableHasRange = false;
    std::string signalName;
    std::string behaviorId;
    ExternalModStatusType statusType = ExternalModStatusType::Freeze;
    ExternalModStatusTarget statusTarget = ExternalModStatusTarget::FrontTarget;
    bool hasStatusTarget = false;
    int32_t durationFrames = 90;
    int32_t tickFrames = 15;
    int32_t damagePerTick = 1;
    int32_t shakeFrames = 12;
    float freezeRange = 180.0f;
    int32_t intensity = 255;
    int32_t blueIntensity = 255;
    float speedMultiplier = 0.5f;
    float jumpMultiplier = 1.5f;
    float strengthMultiplier = 2.0f;
    float weaknessMultiplier = 2.0f;
    float blindSkipChance = 0.35f;
    float blindYawJitterDeg = 20.0f;
    std::string statusId;
    std::string damageProfileId;
    std::string targetingProfileId;
    std::string itemUseProfileId;
    std::string projectileProfileId;
    std::string aoeProfileId;
    std::string movementProfileId;
    std::string fxPresetId;
    std::string stateId;
    std::string spellId;
    std::string aimCameraProfileId;
    ExternalModAimCameraMode aimCameraMode = ExternalModAimCameraMode::FirstPerson;
    std::string patchOperation;
    float range = 180.0f;
    float angle = 0.0f;
    float radius = 0.0f;
    float impulseStrength = 0.0f;
    std::string fxEffectName;
    int32_t fxEffectId = -1;
    int32_t fxActorId = -1;
    std::string fxOverlayName;
    float fxScale = 1.0f;
    int32_t fxLifeFrames = 20;
    bool fxAttachFollow = false;
    int32_t fxSpawnEveryFrames = 0;
    std::string fxStoreKey;
    std::string fxHandleKey;
    std::string stateDomain;
    std::string stateFlag;
    std::string stateControlMode;
    std::string uiScreenId;
    std::string resourceId;
    std::string consumableId;
    std::string inventoryPageId;
    int32_t inventorySlotCount = 0;
    std::string sourceBinding;
    std::string destinationBinding;
    int32_t moveCount = 0;
    std::string containerId;
    std::string recipeId;
    std::string interactionId;
    std::string archetypeId;
    std::string spawnOrigin;
    float forwardDistance = 0.0f;
    float upOffset = 0.0f;
    std::string behaviorTreeId;
    std::string blackboardKey;
    std::string blackboardValue;
    std::string sensorType;
    std::string routeId;
    std::string navHandleKey;
    std::string overlayId;
    std::string renderProfileId;
    std::string renderMatch;
    std::string renderScope;
    std::string renderAttach;
    int32_t sceneId = -1;
    int32_t roomId = -1;
    int32_t durationMs = 0;
    float blendValue = 1.0f;
    bool renderFollow = true;
    bool hasWorldPos = false;
    float worldPosX = 0.0f;
    float worldPosY = 0.0f;
    float worldPosZ = 0.0f;
    bool boolValue = false;
    bool hasBoolValue = false;
    float floatValue = 0.0f;
    bool hasFloatValue = false;
    std::string persistenceDomainId;
    std::string persistenceEntityGuid;
    std::string persistenceEntityScope;
    std::string persistenceStateKey;
    std::string persistenceStateValue;
    std::string migrationId;
    std::string spawnProfileId;
    std::string timeSegmentId;
    std::string weatherProfileId;
    std::string dialogueId;
    std::string dialogueNodeId;
    std::string dialogueOptionId;
    std::string questId;
    std::string questObjectiveId;
    std::string questState;
    std::string timelineId;
    std::string consoleCommandId;
    std::string settingsKey;
    std::string settingsValue;
    std::string settingsDomain;
    std::string settingsApplyMode;
    bool settingsResetAll = false;
};

struct ExternalModSceneAction {
    int16_t sceneId = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModTriggerVolume {
    std::string id;
    int16_t sceneId = 0;
    float minX = 0.0f;
    float minY = 0.0f;
    float minZ = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    float maxZ = 0.0f;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    bool wasInside = false;
    std::vector<ExternalModAction> actions;
};

struct ExternalModInputBinding {
    std::string id;
    int32_t defaultMask = 0;
    ExternalModInputTriggerType defaultTrigger = ExternalModInputTriggerType::Pressed;
    bool allowUserRemap = true;
    std::vector<int32_t> defaultKeyboardScancodes;
};

struct ExternalModCameraHotkeyDefinition {
    std::string id;
    bool allowUserRemap = true;
    std::vector<int32_t> defaultKeyboardScancodes;
    ExternalModAction action;
};

struct ExternalModHotkeyDefinition {
    std::string id;
    bool allowUserRemap = true;
    std::vector<int32_t> defaultKeyboardScancodes;
    ExternalModAction action;
};

struct ExternalModInputActionTrigger {
    std::string id;
    std::string bindingId;
    ExternalModInputTriggerType trigger = ExternalModInputTriggerType::Pressed;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModItemDefinition {
    std::string id;
    std::string sourceModId;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    bool hasSlot = true;
    ExternalModItemPlacement placement = ExternalModItemPlacement::Legacy;
    uint8_t assignableButtonsMask = 0x7F;
    std::string iconAsset;
    std::vector<uint8_t> iconRgba32;
    std::string modelAsset;
    std::string modelTextureAsset;
    std::string modelDisplayList;
    std::string hookshotMetalTextureAsset;
    std::string hookshotHandleTextureAsset;
    std::string hookshotDesignTextureAsset;
    std::string hookshotChainTextureAsset;
    std::string hookshotReticleTextureAsset;
    std::string aimReticleTextureAsset;
    float modelScale = 1.0f;
    ExternalModModelUvOrigin modelUvOrigin = ExternalModModelUvOrigin::Auto;
    ExternalModModelTextureFilter modelTextureFilter = ExternalModModelTextureFilter::Auto;
    int32_t modelTextureTargetWidth = 0;
    int32_t modelTextureTargetHeight = 0;
    struct CustomModelVertex {
        int16_t x = 0;
        int16_t y = 0;
        int16_t z = 0;
        int16_t s = 0;
        int16_t t = 0;
    };
    struct CustomModelTriangle {
        std::array<CustomModelVertex, 3> vertices{};
    };
    std::vector<CustomModelTriangle> customModelTriangles;
    std::vector<uint8_t> modelTextureRgba32;
    int32_t modelTextureWidth = 0;
    int32_t modelTextureHeight = 0;
    bool modelTextureHasTransparency = false;
    std::vector<uint8_t> hookshotMetalTextureRgba16;
    std::vector<uint8_t> hookshotHandleTextureCi8;
    std::vector<uint8_t> hookshotHandleTextureTlutRgba16;
    std::vector<uint8_t> hookshotDesignTextureCi8;
    std::vector<uint8_t> hookshotDesignTextureTlutRgba16;
    std::vector<uint8_t> hookshotChainTextureRgba16;
    std::vector<uint8_t> hookshotReticleTextureI8;
    std::vector<uint8_t> aimReticleTextureI8;
    std::string description;
    std::string onUseExport;
    std::string onUpdateExport;
    std::string onUseBehavior;
    std::string onEquipBehavior;
    std::string useProfile;
    bool aimSelectToggle = false;
    bool aimAttackButtonFire = false;
    ExternalModItemUseTrigger useTrigger = ExternalModItemUseTrigger::OnUse;
    int32_t acquireTextId = 0;
    std::string persistentStateKey;
    ExternalModItemAgePolicy agePolicy = ExternalModItemAgePolicy::AllowChild;
    ExternalModItemUseMode useMode = ExternalModItemUseMode::Vanilla;
    bool hasGrantItemId = false;
    int32_t grantItemId = -1;
    bool hasGrantAmmo = false;
    int32_t grantAmmo = 0;
    std::unordered_map<std::string, float> params;
    bool granted = false;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
};

enum class ExternalModTargetingMode {
    FrontTarget,
    LockedOnTarget,
    Raycast,
    Cone,
    Sphere,
    Self,
    Player,
};

enum class ExternalModAoETargetScope {
    AllNonPlayer,
    EnemiesBosses,
    EnemiesBossesProps,
    PlayerEnemiesBosses,
    AllWithPlayer,
};

enum class ExternalModAoEShape {
    Sphere,
    Capsule,
    Box,
    Cylinder,
};

enum class ExternalModAoEOrientation {
    World,
    OwnerYaw,
    TargetNormal,
};

enum class ExternalModFreezeMode {
    LegacyTimer,
    IceTrapNoDamage,
};

enum class ExternalModLevitationMode {
    None,
    LiftSuspend,
};

enum class ExternalModFreezeShellSize {
    Auto,
    Small,
    Medium,
    Large,
};

struct ExternalModFreezeProfile {
    ExternalModFreezeMode mode = ExternalModFreezeMode::LegacyTimer;
    bool spawnIceShell = false;
    ExternalModFreezeShellSize iceShellSize = ExternalModFreezeShellSize::Auto;
    bool lockPosition = true;
    bool lockRotation = false;
    bool playerInputLock = false;
    bool breakEffectOnExpire = true;
};

struct ExternalModLevitationProfile {
    ExternalModLevitationMode mode = ExternalModLevitationMode::None;
    float liftSpeed = 2.5f;
    float holdHeight = 80.0f;
    int32_t riseFrames = 12;
    int32_t suspendFrames = 45;
    bool lockHorizontal = true;
    float gravityScaleWhileActive = 0.0f;
};

struct ExternalModStatusDefinition {
    std::string id;
    std::string displayName;
    std::string baseStatusId;
    ExternalModStatusType baseStatusType = ExternalModStatusType::Custom;
    std::string stackingMode = "refresh";
    int32_t maxStacks = 1;
    int32_t durationFrames = 90;
    int32_t tickFrames = 15;
    int32_t damagePerTick = 0;
    int32_t intensity = 255;
    int32_t shakeFrames = 0;
    bool hasFreezeProfile = false;
    ExternalModFreezeProfile freezeProfile;
    bool hasLevitationProfile = false;
    ExternalModLevitationProfile levitationProfile;
    std::vector<std::string> visualsStartFx;
    std::vector<std::string> visualsLoopFx;
    std::vector<std::string> visualsEndFx;
    std::vector<ExternalModAction> onApply;
    std::vector<ExternalModAction> onTick;
    std::vector<ExternalModAction> onExpire;
};

struct ExternalModDamageProfile {
    std::string id;
    int32_t amount = 1;
    std::string type = "physical";
    int32_t iframesFrames = 0;
    std::string propInteraction = "none";
};

struct ExternalModTargetingProfile {
    std::string id;
    ExternalModTargetingMode mode = ExternalModTargetingMode::FrontTarget;
    float range = 180.0f;
    float radius = 0.0f;
    float angle = 0.0f;
    bool stopOnWall = true;
    bool includeProps = true;
    bool includeEnemies = true;
};

struct ExternalModUseProfileEffect {
    std::string action;
    std::string damageProfileId;
    std::string statusId;
    std::string projectileProfileId;
    std::string aoeProfileId;
    std::string movementProfileId;
    std::string fxPresetId;
    std::string fxStoreKey;
    std::string fxHandleKey;
    std::string fxOverlayName;
    int32_t fxActorId = -1;
    int32_t fxLifeFrames = 20;
    std::string stateId;
    std::string spellId;
    std::string shockwaveOrigin = "player";
    std::array<uint8_t, 4> shockwavePrimColor = { { 255, 255, 255, 255 } };
    std::array<uint8_t, 4> shockwaveEnvColor = { { 200, 200, 200, 255 } };
    int32_t shockwaveLife = 10;
    bool shockwaveSpawnIceSmoke = false;
    int32_t durationFrames = 0;
    int32_t tickFrames = 0;
    int32_t damagePerTick = 0;
    int32_t intensity = 255;
};

struct ExternalModItemUseProfile {
    std::string id;
    std::string targetingProfileId;
    int32_t cooldownFrames = 0;
    std::vector<ExternalModUseProfileEffect> effects;
};

struct ExternalModProjectileProfile {
    std::string id;
    std::string shape = "sphere";
    float speed = 0.0f;
    float gravityScale = 0.0f;
    int32_t lifetimeFrames = 1;
    float radius = 0.0f;
    int32_t maxHits = 1;
    bool stopOnWall = true;
    std::string damageProfileId;
    std::vector<std::string> applyStatuses;
};

struct ExternalModAoEProfile {
    std::string id;
    std::string shape = "sphere";
    ExternalModAoEShape shapeType = ExternalModAoEShape::Sphere;
    ExternalModAoEOrientation orientationType = ExternalModAoEOrientation::World;
    std::string orientation = "world";
    ExternalModAoETargetScope targetScope = ExternalModAoETargetScope::AllNonPlayer;
    float range = 0.0f;
    float radius = 0.0f;
    float angle = 0.0f;
    std::array<float, 3> halfExtents = { { 0.0f, 0.0f, 0.0f } };
    float height = 0.0f;
    float capsuleHalfHeight = 0.0f;
    int32_t durationFrames = 1;
    int32_t tickFrames = 0;
    std::vector<ExternalModUseProfileEffect> onEnter;
    std::vector<ExternalModUseProfileEffect> onTick;
    std::vector<ExternalModUseProfileEffect> onStayTick;
    std::vector<ExternalModUseProfileEffect> onExit;
};

struct ExternalModMovementProfile {
    std::string id;
    ExternalModMovementMode mode = ExternalModMovementMode::Modifier;
    int32_t durationFrames = 0;
    float speedMultiplier = 1.0f;
    float accelMultiplier = 1.0f;
    float gravityScale = 1.0f;
    bool boardRequired = true;
    std::string boardSpawnMode = "persistent_under_player";
    std::string idlePose = "stand";
    bool idleLock = true;
    float boardHeightOffset = 10.0f;
    bool boardPitchRollFromGround = true;
    bool boardVisibleWhenIdle = true;
    std::string boardModelAsset;
    float boardScale = 1.0f;
    float surfMaxSpeed = 12.0f;
    float surfDownhillAccel = 0.45f;
    float surfUphillBrake = 0.35f;
    float surfFlatDrag = 0.08f;
    float surfTurnRateDeg = 6.0f;
    float idleSpeedThreshold = 0.10f;
    float surfForwardAccel = 0.20f;
    int32_t surfBoostButtonMask = 0;
    float surfBoostAccel = 0.45f;
    float surfBoostMaxSpeed = 16.0f;
    int32_t surfBoostCooldownFrames = 12;
    float boardPitchOffsetDeg = 0.0f;
    float boardYawOffsetDeg = 0.0f;
    float boardRollOffsetDeg = 0.0f;
    float boardForwardOffset = 0.0f;
    float boardRightOffset = 0.0f;
    float boardUpOffset = 0.0f;
    float riderHeightOffset = 0.0f;
};

struct ExternalModAimCameraProfile {
    std::string id;
    uint8_t contextsMask = 0x1F;
    int16_t cUpFirstPersonMode = 6;
    int16_t bowFirstPersonMode = 7;
    int16_t hookshotFirstPersonMode = 9;
    int16_t slingshotFirstPersonMode = 11;
    int16_t boomerangFirstPersonMode = 7;
    int16_t cUpOverShoulderMode = 8;
    int16_t bowOverShoulderMode = 8;
    int16_t hookshotOverShoulderMode = 8;
    int16_t slingshotOverShoulderMode = 8;
    int16_t boomerangOverShoulderMode = 8;
    std::string shoulder = "right";
    std::string aimRay = "camera_center";
    float reticleX = 0.5f;
    float reticleY = 0.5f;
    bool mouseFireEnabled = false;
    ExternalModAimMouseButton mouseFireButton = ExternalModAimMouseButton::Left;
    ExternalModAimMouseFireMode mouseFireMode = ExternalModAimMouseFireMode::Both;
    ExternalModAimReticleVisibility reticleVisibility = ExternalModAimReticleVisibility::AimOnly;
};

struct ExternalModAimCameraState {
    bool overShoulderEnabled = false;
    std::string activeProfileId;
    std::string activeProfileOwnerModId;
};

struct ExternalModFxPresetDefinition {
    std::string id;
    int32_t maxInstances = 32;
    std::vector<ExternalModAction> actions;
};

struct ExternalModStateDefinition {
    std::string id;
    std::string domain = "player";
    std::vector<std::string> aliases;
    int32_t defaultDurationFrames = 0;
    std::vector<ExternalModAction> onApply;
    std::vector<ExternalModAction> onRemove;
};

struct ExternalModSpellDefinition {
    std::string id;
    std::string targetingProfileId;
    int32_t cooldownFrames = 0;
    std::vector<ExternalModUseProfileEffect> effects;
};

struct ExternalModUiScreenDefinition {
    std::string id;
    std::string sourceModId;
    bool closeOnEscape = true;
    std::string hudLayoutId;
    std::vector<ExternalModAction> onOpen;
    std::vector<ExternalModAction> onClose;
};

struct ExternalModUiHudLayoutDefinition {
    std::string id;
    std::string sourceModId;
};

struct ExternalModColorRgba {
    uint8_t r = 255;
    uint8_t g = 255;
    uint8_t b = 255;
    uint8_t a = 255;
};

enum class ExternalModResourceRingAnchorMode {
    Contextual,
    Fixed,
    Both,
};

enum class ExternalModResourceRingFixedAnchor {
    Left,
    Right,
    None,
};

struct ExternalModPlayerResourceActionRule {
    std::string tag;
    float startCost = 0.0f;
    float costPerSecond = 0.0f;
    bool blockWhenInsufficient = true;
    bool cancelOnDepleted = true;
    std::string depletedResponse;
    std::string inputBindingId;
};

struct ExternalModPlayerResourceTickRule {
    std::string id;
    float drainPerSecond = 0.0f;
    float movingMultiplier = 1.0f;
    float sprintMultiplier = 1.0f;
    float climbMultiplier = 1.0f;
    float swimMultiplier = 1.0f;
    bool manualGameplayOnly = true;
};

struct ExternalModPlayerResourceDepletionEffects {
    bool disableSprint = false;
    float movementMultiplier = 1.0f;
    float periodicDamageHearts = 0.0f;
    int32_t periodicDamageIntervalMs = 0;
};

struct ExternalModPlayerResourceLink {
    std::string targetResourceId;
    float onDepletedRegenMultiplier = 1.0f;
};

struct ExternalModPlayerResourceDefinition {
    std::string id;
    std::string sourceModId;
    std::string kind = "custom";
    std::string displayName;
    std::string settingsDomain = "global";
    std::string storageDomainId;
    std::string capacityStorageKey = "capacity";
    std::string currentStorageKey = "current";
    float baseCapacity = 100.0f;
    float wheelCapacity = 100.0f;
    int32_t segmentCount = 4;
    float initialValue = -1.0f;
    bool persistCurrentValue = false;
    bool refillOnLoad = true;
    bool refillOnSceneEnter = true;
    float regenRatePerSecond = 18.0f;
    int32_t regenDelayMs = 900;
    int32_t depletedRegenDelayMs = 1800;
    float lowPercent = 0.25f;
    float lowThresholdPercent = -1.0f;
    std::string enabledSettingKey;
    std::string drainMultiplierSettingKey;
    std::string regenMultiplierSettingKey;
    std::vector<ExternalModPlayerResourceActionRule> actionRules;
    std::vector<ExternalModPlayerResourceTickRule> tickRules;
    ExternalModPlayerResourceDepletionEffects depletionEffects;
    std::vector<ExternalModPlayerResourceLink> resourceLinks;
};

enum class ExternalModPlayerConsumableStorageMode {
    Stack,
    BottleContent,
};

enum class ExternalModItemStateEvent {
    Select,
    Deselect,
    Press,
    Hold,
    Release,
    Impact,
};

struct ExternalModItemStateTransition {
    ExternalModItemStateEvent event = ExternalModItemStateEvent::Press;
    std::string fromState;
    std::string toState;
    std::vector<ExternalModAction> actions;
};

struct ExternalModItemStateMachineDefinition {
    std::string id;
    std::string itemId;
    std::string bindingId;
    std::string initialState;
    std::vector<ExternalModItemStateTransition> transitions;
};

struct ExternalModItemStateMachineRuntimeState {
    std::string currentState;
    bool selected = false;
};

enum class ExternalModPlayerConsumableUseAnimation {
    None,
    DrinkDemo,
};

struct ExternalModPlayerConsumableEffect {
    std::string resourceId;
    float addValue = 0.0f;
};

struct ExternalModPlayerConsumableDefinition {
    std::string id;
    std::string sourceModId;
    std::string displayName;
    std::string settingsDomain = "save";
    std::string storageDomainId;
    std::string countStorageKey = "count";
    ExternalModPlayerConsumableStorageMode storageMode = ExternalModPlayerConsumableStorageMode::Stack;
    ExternalModPlayerConsumableUseAnimation useAnimation = ExternalModPlayerConsumableUseAnimation::None;
    int32_t maxStack = 99;
    int32_t usesPerFill = 1;
    std::string fillSource;
    int32_t placeholderItemId = -1;
    int32_t placeholderPartialItemId = -1;
    std::string placeholderIconAsset;
    std::string placeholderPartialIconAsset;
    std::vector<uint8_t> placeholderIconRgba32;
    std::vector<uint8_t> placeholderPartialIconRgba32;
    int32_t returnItemId = -1;
    int32_t inventoryPlaceholderItemId = -1;
    int32_t companionVanillaItemId = -1;
    std::string companionIconAsset;
    std::vector<uint8_t> companionIconRgba32;
    std::vector<ExternalModPlayerConsumableEffect> effects;
};

struct ExternalModBottleContentState {
    std::string consumableId;
    int32_t remainingUses = 0;
};

struct ExternalModWorldForageRuleDefinition {
    std::string id;
    std::string sourceModId;
    std::string source;
    float chance = 0.0f;
    bool replaceVanillaDrop = true;
    std::vector<ExternalModAction> actions;
};

struct ExternalModResourceRingDefinition {
    std::string id;
    std::string sourceModId;
    std::string resourceId;
    std::string style = "botw_green";
    std::string settingsDomain = "global";
    ExternalModResourceRingAnchorMode anchorMode = ExternalModResourceRingAnchorMode::Both;
    ExternalModResourceRingFixedAnchor fixedAnchor = ExternalModResourceRingFixedAnchor::Left;
    float screenOffsetX = 48.0f;
    float screenOffsetY = -48.0f;
    float worldOffsetX = 0.0f;
    float worldOffsetY = 40.0f;
    float worldOffsetZ = 0.0f;
    float scale = 1.0f;
    float opacity = 1.0f;
    float thickness = 7.0f;
    float ringSpacing = 5.0f;
    int32_t hideDelayMs = 900;
    bool lowPulse = true;
    bool showWhenFull = false;
    std::string contextualEnabledSettingKey;
    std::string fixedEnabledSettingKey;
    std::string scaleSettingKey;
    std::string opacitySettingKey;
    std::string fixedStackGroup;
    int32_t fixedStackOrder = 0;
    float fixedStackSpacing = 18.0f;
    std::string companionIconAsset;
    std::string companionCounterSource;
    int32_t companionVanillaItemId = -1;
    std::vector<uint8_t> companionIconRgba32;
    ExternalModColorRgba normalColor{ 110, 255, 110, 255 };
    ExternalModColorRgba lowColor{ 255, 235, 90, 255 };
    ExternalModColorRgba exhaustedColor{ 255, 110, 90, 255 };
    ExternalModColorRgba backgroundColor{ 0, 0, 0, 150 };
    ExternalModColorRgba segmentColor{ 255, 255, 255, 120 };
};

struct ExternalModInventoryPageDefinition {
    std::string id;
    std::string sourceModId;
    int32_t slotCount = 0;
    bool persist = true;
};

struct ExternalModContainerSlotDefinition {
    std::string id;
    std::string type;
    std::string filterTag;
    int32_t stackLimit = 1;
};

struct ExternalModContainerDefinition {
    std::string id;
    std::string sourceModId;
    std::vector<ExternalModContainerSlotDefinition> slots;
};

struct ExternalModProcessingRecipeDefinition {
    std::string id;
    std::string sourceModId;
    int32_t durationMs = 1000;
};

struct ExternalModInteractionDefinition {
    std::string id;
    std::string sourceModId;
    std::vector<ExternalModAction> actions;
};

struct ExternalModActorArchetypeDefinition {
    std::string id;
    std::string sourceModId;
    std::string actorDefinitionId;
    std::string interactionId;
    std::string behaviorId;
};

struct ExternalModActorAdapterDefinition {
    std::string id;
    std::string sourceModId;
    int32_t actorId = -1;
    std::vector<ExternalModAction> onInteract;
    std::vector<ExternalModAction> onDamage;
    std::vector<ExternalModAction> onTalk;
};

struct ExternalModBehaviorTreeDefinition {
    std::string id;
    std::string sourceModId;
    std::string rootNodeType;
};

struct ExternalModSensorDefinition {
    std::string id;
    std::string sourceModId;
    std::string sensorType;
    float range = 200.0f;
};

struct ExternalModRouteWaypoint {
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

struct ExternalModRouteDefinition {
    std::string id;
    std::string sourceModId;
    std::string mode = "loop";
    std::vector<ExternalModRouteWaypoint> waypoints;
};

struct ExternalModNavBridgeDefinition {
    std::string id;
    std::string sourceModId;
    std::string fallbackMode = "waypoint_direct";
};

struct ExternalModDebugOverlayDefinition {
    std::string id;
    std::string sourceModId;
    std::string overlayType;
};

struct ExternalModMaterialDefinition {
    struct Bind {
        std::string otrPath;
        int16_t sceneId = -1;
        int16_t roomId = -1;
        bool hasSceneId = false;
        bool hasRoomId = false;
    };

    std::string id;
    std::string bindOtrPath;
    std::vector<Bind> binds;
    std::string mapAlbedoAsset;
    std::string mapNormalAsset;
    std::string mapOrmAsset;
    std::string mapHeightAsset;
    std::string mapEmissiveAsset;
    std::string shadingModel = "lit_legacy";
    float normalScale = 1.0f;
    float emissiveIntensity = 1.0f;
    float parallaxScale = 0.0f;
    int32_t parallaxSteps = 8;
    std::string parallaxMode = "off";
    std::string alphaMode = "opaque";
};

struct ExternalModPbrDefinition {
    struct AmbientOcclusionSettings {
        bool enabled = false;
        std::string quality = "medium";
        float radius = 0.55f;
        float intensity = 0.9f;
        float bias = 0.02f;
        float power = 1.2f;
        float maxDistance = 1200.0f;
        int32_t blurPasses = 2;
    };

    std::string id;
    bool enabled = true;
    bool enablePom = false;
    int32_t pomSteps = 8;
    float pomMaxDistance = 1200.0f;
    int32_t maxDynamicLightsNear = 32;
    int32_t maxDynamicLightsTotal = 128;
    std::string backendPolicy = "auto";
    AmbientOcclusionSettings ambientOcclusion;
};

struct ExternalModLightProfileDefinition {
    std::string id;
    std::string lightType = "point";
    std::array<float, 3> colorLinear = { { 1.0f, 1.0f, 1.0f } };
    float kelvin = 0.0f;
    bool hasKelvin = false;
    float intensity = 1.0f;
    float radius = 250.0f;
    std::string falloff = "inverse_square";
    float innerConeDeg = 20.0f;
    float outerConeDeg = 40.0f;
    std::array<float, 3> direction = { { 0.0f, -1.0f, 0.0f } };
    bool flicker = false;
    float flickerAmount = 0.0f;
    int32_t defaultLifetimeMs = 0;
    bool castShadows = false;
    int32_t shadowResolution = 512;
    float shadowBias = 0.01f;
    float shadowNormalBias = 0.02f;
    float shadowRange = 1200.0f;
    float volumetricIntensity = 1.0f;
};

struct ExternalModPostFxPresetDefinition {
    struct VolumetricsSettings {
        bool enabled = false;
        std::string quality = "medium";
        float density = 0.03f;
        std::array<float, 3> color = { { 1.0f, 1.0f, 1.0f } };
        bool hasColor = false;
        float ambientIntensity = 0.35f;
        float anisotropy = 0.2f;
        float startDistance = 64.0f;
        float maxDistance = 2400.0f;
        bool heightFogEnabled = false;
        float baseHeight = 0.0f;
        float heightFalloff = 0.0025f;
        float lightShaftIntensity = 1.0f;
        float shadowIntensity = 0.6f;
        float temporalBlend = 0.88f;
        float jitterScale = 1.0f;
        bool debugView = false;
    };

    std::string id;
    std::string tonemap = "filmic";
    float exposure = 1.0f;
    float bloom = 0.0f;
    std::array<float, 4> fogColor = { { 0.0f, 0.0f, 0.0f, 1.0f } };
    float fogDensity = 0.0f;
    int32_t fogNear = 996;
    int32_t fogFar = 12800;
    bool hasFogNear = false;
    bool hasFogFar = false;
    float vignette = 0.0f;
    float saturation = 1.0f;
    bool forceFogOverlay = false;
    float fogOverlayStrength = 0.0f;
    bool forceDepthAwareFog = false;
    bool hasVolumetrics = false;
    VolumetricsSettings volumetrics;
};

struct ExternalModSceneProfileDefinition {
    std::string id;
    int16_t sceneId = -1;
    std::string postFxPresetId;
    std::string skylightProfileId;
    std::array<float, 3> ambientColor = { { 0.0f, 0.0f, 0.0f } };
};

struct ExternalModRoomProfileDefinition {
    std::string id;
    int16_t sceneId = -1;
    int16_t roomId = -1;
    std::string postFxPresetId;
    std::string skylightProfileId;
};

struct ExternalModAssetPackDefinition {
    std::string id;
    int32_t priority = 0;
    std::string materialSetPath;
};

struct ExternalModAssetSourceDefinition {
    std::string id;
    std::string path;
    std::string kind;
    std::string format;
    bool hostAccess = true;
    uint64_t sizeBytes = 0;
    bool loadedFromCache = false;
    std::string preparedPath;
};

struct ExternalModMeshDefinition {
    using CustomModelVertex = ExternalModItemDefinition::CustomModelVertex;
    using CustomModelTriangle = ExternalModItemDefinition::CustomModelTriangle;

    std::string id;
    std::string sourceModId;
    std::string modelAsset;
    std::string modelTextureAsset;
    std::string modelDisplayList;
    float modelScale = 1.0f;
    ExternalModModelUvOrigin modelUvOrigin = ExternalModModelUvOrigin::Auto;
    ExternalModModelTextureFilter modelTextureFilter = ExternalModModelTextureFilter::Auto;
    int32_t modelTextureTargetWidth = 0;
    int32_t modelTextureTargetHeight = 0;
    std::vector<CustomModelTriangle> customModelTriangles;
    std::vector<uint8_t> modelTextureRgba32;
    int32_t modelTextureWidth = 0;
    int32_t modelTextureHeight = 0;
    bool modelTextureHasTransparency = false;
};

struct ExternalModPrefabDefinition {
    std::string id;
    std::string sourceModId;
    std::string meshId;
    float offsetX = 0.0f;
    float offsetY = 0.0f;
    float offsetZ = 0.0f;
    float rotOffsetX = 0.0f;
    float rotOffsetY = 0.0f;
    float rotOffsetZ = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
    float lodDistance = 5000.0f;
    bool billboard = false;
};

struct ExternalModWorldInstanceDefinition {
    std::string id;
    std::string sourceModId;
    std::string prefabId;
    std::string entityId;
    int16_t sceneId = -1;
    int16_t roomId = -1;
    bool hasRoomId = false;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
};

struct ExternalModRenderInspectorDefinition {
    std::string id;
    bool enabledByDefault = false;
    bool showMaterialUnderCursor = true;
    bool showLightBudget = true;
    bool showPostFxState = true;
};

struct ExternalModEditorRuntimeDefinition {
    std::string id;
};

struct ExternalModEditorUiDefinition {
    std::string id;
};

struct ExternalModEditorSelectionDefinition {
    std::string id;
};

struct ExternalModEditorGizmoDefinition {
    std::string id;
};

struct ExternalModEditorLibraryDefinition {
    std::string id;
};

struct ExternalModEditorProjectDefinition {
    std::string id;
};

struct ExternalModEditorPlacementDefinition {
    std::string id;
    std::string sourceModId;
    std::string prefabId;
    std::string entityId;
    std::string displayName;
    std::string category = "default";
    int16_t sceneId = -1;
    int16_t roomId = -1;
    bool hasSceneId = false;
    bool hasRoomId = false;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
};

struct ExternalModAssetImporterDefinition {
    std::string id;
};

struct ExternalModWorldAuthoringDefinition {
    std::string id;
    std::string sourceModId;
    std::string placementId;
    std::string prefabId;
    std::string entityId;
    int16_t sceneId = -1;
    int16_t roomId = -1;
    bool hasSceneId = false;
    bool hasRoomId = false;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    bool hasRotation = false;
    float scaleX = 1.0f;
    float scaleY = 1.0f;
    float scaleZ = 1.0f;
    bool hasScale = false;
};

struct ExternalModCollisionAuthoringDefinition {
    std::string id;
};

struct ExternalModEditorInspectorDefinition {
    std::string id;
};

struct ExternalModWorldPersistenceDefinition {
    std::string id;
    std::string sourceModId;
    std::string scope = "room";
    bool autoSaveOnRoomExit = true;
    bool autoLoadOnRoomEnter = true;
};

struct ExternalModWorldStorageDomainDefinition {
    std::string id;
    std::string sourceModId;
    int32_t version = 1;
    std::string file = "domains/worldState.json";
};

struct ExternalModWorldSpawnProfileDefinition {
    std::string id;
    std::string sourceModId;
    std::string actorArchetypeId;
    int32_t maxActive = 1;
    int32_t cooldownFrames = 300;
    float minDistanceFromPlayer = 120.0f;
    float maxDistanceFromPlayer = 1200.0f;
};

struct ExternalModWorldTimeWeatherDefinition {
    std::string id;
    std::string sourceModId;
    std::string defaultTimeSegment = "day";
    std::string defaultWeather = "clear";
};

struct ExternalModWorldSeedingDefinition {
    std::string id;
    std::string sourceModId;
    int32_t seedOffset = 0;
};

struct ExternalModWorldMigrationDefinition {
    std::string id;
    std::string sourceModId;
    int32_t fromVersion = 1;
    int32_t toVersion = 1;
    std::vector<ExternalModAction> actions;
};

struct ExternalModPersistenceInspectorDefinition {
    std::string id;
    std::string sourceModId;
    bool enabledByDefault = false;
};

struct ExternalModNarrativeFlagDefinition {
    std::string id;
    std::string sourceModId;
    std::string valueType = "bool";
    std::string scope = "global";
    std::string defaultValue = "false";
};

struct ExternalModNarrativeDialogueOptionDefinition {
    std::string id;
    std::string text;
    std::string nextNodeId;
    std::vector<ExternalModAction> onChoose;
};

struct ExternalModNarrativeDialogueNodeDefinition {
    std::string id;
    std::string speaker;
    std::string line;
    std::string nextNodeId;
    std::vector<ExternalModNarrativeDialogueOptionDefinition> options;
    std::vector<ExternalModAction> onEnter;
};

struct ExternalModNarrativeDialogueDefinition {
    std::string id;
    std::string sourceModId;
    std::string startNodeId;
    std::vector<ExternalModNarrativeDialogueNodeDefinition> nodes;
};

struct ExternalModNarrativeQuestObjectiveDefinition {
    std::string id;
    std::string text;
    std::string state = "pending";
};

struct ExternalModNarrativeQuestDefinition {
    std::string id;
    std::string sourceModId;
    std::string startState = "inactive";
    std::vector<ExternalModNarrativeQuestObjectiveDefinition> objectives;
    std::vector<ExternalModAction> onStart;
    std::vector<ExternalModAction> onComplete;
};

struct ExternalModNarrativeTimelineTrackDefinition {
    std::string id;
    std::string type;
    int32_t atFrame = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModNarrativeTimelineDefinition {
    std::string id;
    std::string sourceModId;
    int32_t durationFrames = 0;
    bool skippable = true;
    std::vector<ExternalModNarrativeTimelineTrackDefinition> tracks;
    std::vector<ExternalModAction> onCompleted;
    std::vector<ExternalModAction> onSkipped;
};

struct ExternalModNarrativeInspectorDefinition {
    std::string id;
    std::string sourceModId;
    bool enabledByDefault = false;
};

struct ExternalModDevHotReloadDefinition {
    std::string id;
    std::string sourceModId;
    bool enabled = true;
    int32_t debounceMs = 250;
};

struct ExternalModDevConsoleCommandDefinition {
    std::string id;
    std::string sourceModId;
    std::string command;
    std::vector<ExternalModAction> actions;
};

struct ExternalModDevWatcherDefinition {
    std::string id;
    std::string sourceModId;
    std::string path;
    std::string target = "all";
    int32_t debounceMs = 250;
};

struct ExternalModWasmSandboxDefinition {
    std::string id;
    std::string sourceModId;
    int32_t maxCallMs = 2;
    int32_t maxFrameBudgetMs = 2;
    int32_t maxHookCallsPerFrame = 256;
    bool deterministicMode = false;
    std::vector<std::string> allowedSurfaces;
};

struct ExternalModReloadInspectorDefinition {
    std::string id;
    std::string sourceModId;
    bool enabledByDefault = false;
};

struct ExternalModBehaviorCondition {
    std::string type;
    std::string scope;
    std::string key;
    std::string op;
    std::string value;
    float numberValue = 0.0f;
    bool hasNumberValue = false;
};

struct ExternalModBehaviorRule {
    std::vector<ExternalModBehaviorCondition> conditions;
    std::vector<ExternalModAction> actions;
    float randomChance = 1.0f;
};

struct ExternalModBehaviorDefinition {
    std::string id;
    std::unordered_map<std::string, std::vector<ExternalModBehaviorRule>> events;
};

struct ExternalModSceneDefinition {
    std::string id;
    bool hasEntrance = false;
    int16_t entranceIndex = 0;
    bool useNamespacedScene = false;
    std::string sceneResourcePath;
    bool hasHostEntrance = false;
    int16_t hostEntranceIndex = 0;
    bool hasFallbackEntrance = false;
    int16_t fallbackEntranceIndex = 0;
    bool fallbackPlayable = true;
};

struct ExternalModPendingSceneLoadRequest {
    bool pending = false;
    std::string modId;
    std::string sceneId;
    std::string sceneResourcePath;
    int16_t expectedHostSceneId = -1;
    bool hasHostEntrance = false;
    int16_t hostEntranceIndex = 0;
    bool hasFallbackEntrance = false;
    int16_t fallbackEntranceIndex = 0;
    bool fallbackPlayable = true;
    int32_t spawnId = 0;
};

struct ExternalModActorDefinition {
    std::string id;
    ExternalModActorArchetype archetype = ExternalModActorArchetype::Npc;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t maxInstances = 1;
    int32_t tickRate = 1;
    float lodDistance = 5000.0f;
    bool interactable = false;
    float interactDistance = 80.0f;
    std::string behaviorId;
    std::string prefabId;
    std::vector<std::string> components;
    std::string exportOnInit;
    std::string exportOnUpdate;
    std::string exportOnInteract;
    std::string exportOnDestroy;
};

struct ExternalModActorInstance {
    uint32_t handle = 0;
    std::string definitionId;
    bool active = false;
    int16_t sceneId = -1;
    float posX = 0.0f;
    float posY = 0.0f;
    float posZ = 0.0f;
    float rotX = 0.0f;
    float rotY = 0.0f;
    float rotZ = 0.0f;
    int32_t tickCounter = 0;
    bool wasNearPlayer = false;
    bool wasInteracting = false;
    int32_t timerFrames = 0;
    std::unordered_map<std::string, std::string> state;
};

enum class ExternalModHookDispatchType {
    Actions,
    WasmExport,
};

enum class ExternalModHookType {
    OnLoadGame,
    OnExitGame,
    OnSceneInit,
    AfterSceneCommands,
    OnTransitionEnd,
    OnFlagSet,
    OnFlagUnset,
    OnSceneFlagSet,
    OnSceneFlagUnset,
    OnPlayerUpdate,
    OnPlayerUseItem,
    OnPlayerHealthChange,
    OnItemReceive,
    OnActorInit,
    OnActorSpawn,
    OnActorUpdate,
    OnActorKill,
    OnActorDestroy,
    OnEnemyDefeat,
    OnBossDefeat,
    OnStatusApplied,
    OnStatusTick,
    OnStatusExpired,
    OnStateApplied,
    OnStateRemoved,
    OnUiScreenOpened,
    OnUiScreenClosed,
    OnUiAction,
    OnContainerSlotChanged,
    OnProcessStart,
    OnProcessTick,
    OnProcessComplete,
    OnArchetypeSpawn,
    OnArchetypeDespawn,
    OnInteraction,
    OnBehaviorNodeChanged,
    OnPathRequested,
    OnPathFailed,
    OnWorldSceneLoaded,
    OnWorldRoomEntered,
    OnWorldRoomExited,
    OnWorldOverworldTick,
    OnWorldTimeOfDayChanged,
    OnWorldSkyboxChanged,
    OnRenderProfileResolved,
    OnRenderLightSpawned,
    OnRenderLightExpired,
    OnRenderFallbackApplied,
    OnPersistentEntityLoaded,
    OnPersistentEntitySaved,
    OnPersistentDomainMigrated,
    OnWorldSpawnProfileTick,
    OnWorldTimeSegmentChanged,
    OnWorldWeatherChanged,
    OnDialogueStarted,
    OnDialogueChoiceCommitted,
    OnQuestStateChanged,
    OnTimelineStarted,
    OnTimelineCompleted,
    OnTimelineSkipped,
    OnHotReloadApplied,
    OnHotReloadFailed,
    OnSandboxBudgetExceeded,
    OnSandboxPermissionDenied,
    OnSettingsChanged,
    OnResourceChanged,
    OnResourceDepleted,
    OnResourceRecovered,
    OnResourceCapacityChanged,
    OnPlayDestroy,
    OnGameFrameUpdate,
};

struct ExternalModHookFilter {
    bool hasScene = false;
    int16_t scene = 0;
    bool hasActorId = false;
    int16_t actorId = 0;
    bool hasCategory = false;
    int16_t category = 0;
    bool hasItemId = false;
    int16_t itemId = 0;
    bool hasFlagType = false;
    int16_t flagType = 0;
    bool hasFlagId = false;
    int16_t flagId = 0;
    bool hasHealthDeltaRange = false;
    int16_t healthDeltaMin = 0;
    int16_t healthDeltaMax = 0;
};

struct ExternalModHookSubscription {
    std::string id;
    ExternalModHookType hook = ExternalModHookType::OnGameFrameUpdate;
    ExternalModHookDispatchType dispatch = ExternalModHookDispatchType::Actions;
    std::vector<ExternalModAction> actions;
    std::string wasmExport;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    ExternalModHookFilter filters;
};

struct ExternalModRuntime {
    bool enabled = false;
    int32_t apiVersion = 1;
    std::vector<ExternalModAction> onGameLoadedActions;
    std::vector<ExternalModSceneAction> onSceneInitActions;
    std::vector<ExternalModTriggerVolume> frameTriggers;
    std::vector<ExternalModInputBinding> inputBindings;
    std::vector<ExternalModCameraHotkeyDefinition> cameraHotkeys;
    std::vector<ExternalModHotkeyDefinition> hotkeys;
    std::vector<ExternalModInputActionTrigger> inputTriggers;
    std::vector<ExternalModItemStateMachineDefinition> itemStateMachineDefinitions;
    std::unordered_map<std::string, ExternalModItemStateMachineRuntimeState> itemStateMachineStates;
    std::vector<ExternalModItemDefinition> itemDefinitions;
    std::vector<ExternalModHookSubscription> hookSubscriptions;
    std::vector<ExternalModActorDefinition> actorDefinitions;
    std::vector<ExternalModActorInstance> actorInstances;
    std::vector<ExternalModBehaviorDefinition> behaviorDefinitions;
    std::vector<ExternalModSceneDefinition> sceneDefinitions;
    std::vector<ExternalModStatusDefinition> statusDefinitions;
    std::vector<ExternalModDamageProfile> damageProfiles;
    std::vector<ExternalModTargetingProfile> targetingProfiles;
    std::vector<ExternalModItemUseProfile> itemUseProfiles;
    std::vector<ExternalModProjectileProfile> projectileProfiles;
    std::vector<ExternalModAoEProfile> aoeProfiles;
    std::vector<ExternalModMovementProfile> movementProfiles;
    std::vector<ExternalModAimCameraProfile> cameraProfiles;
    std::vector<ExternalModUiScreenDefinition> uiScreenDefinitions;
    std::vector<ExternalModUiHudLayoutDefinition> uiHudDefinitions;
    std::vector<ExternalModPlayerResourceDefinition> playerResourceDefinitions;
    std::vector<ExternalModResourceRingDefinition> resourceRingDefinitions;
    std::vector<ExternalModPlayerConsumableDefinition> playerConsumableDefinitions;
    std::vector<ExternalModWorldForageRuleDefinition> worldForageDefinitions;
    std::vector<ExternalModInventoryPageDefinition> inventoryPageDefinitions;
    std::vector<ExternalModContainerDefinition> containerDefinitions;
    std::vector<ExternalModProcessingRecipeDefinition> recipeDefinitions;
    std::vector<ExternalModInteractionDefinition> interactionDefinitions;
    std::vector<ExternalModActorArchetypeDefinition> actorArchetypeDefinitions;
    std::vector<ExternalModActorAdapterDefinition> actorAdapterDefinitions;
    std::vector<ExternalModBehaviorTreeDefinition> behaviorTreeDefinitions;
    std::vector<ExternalModSensorDefinition> sensorDefinitions;
    std::vector<ExternalModRouteDefinition> routeDefinitions;
    std::vector<ExternalModNavBridgeDefinition> navBridgeDefinitions;
    std::vector<ExternalModDebugOverlayDefinition> debugOverlayDefinitions;
    std::vector<ExternalModMaterialDefinition> materialDefinitions;
    std::vector<ExternalModPbrDefinition> pbrDefinitions;
    std::vector<ExternalModLightProfileDefinition> lightProfiles;
    std::vector<ExternalModPostFxPresetDefinition> postFxPresets;
    std::vector<ExternalModSceneProfileDefinition> sceneProfiles;
    std::vector<ExternalModRoomProfileDefinition> roomProfiles;
    std::vector<ExternalModAssetPackDefinition> assetPackDefinitions;
    std::vector<ExternalModAssetSourceDefinition> assetSourceDefinitions;
    std::vector<ExternalModMeshDefinition> meshDefinitions;
    std::vector<ExternalModPrefabDefinition> prefabDefinitions;
    std::vector<ExternalModWorldInstanceDefinition> worldInstanceDefinitions;
    std::vector<ExternalModRenderInspectorDefinition> renderInspectorDefinitions;
    std::vector<ExternalModEditorRuntimeDefinition> editorRuntimeDefinitions;
    std::vector<ExternalModEditorUiDefinition> editorUiDefinitions;
    std::vector<ExternalModEditorSelectionDefinition> editorSelectionDefinitions;
    std::vector<ExternalModEditorGizmoDefinition> editorGizmoDefinitions;
    std::vector<ExternalModEditorLibraryDefinition> editorLibraryDefinitions;
    std::vector<ExternalModEditorProjectDefinition> editorProjectDefinitions;
    std::vector<ExternalModEditorPlacementDefinition> editorPlacementDefinitions;
    std::vector<ExternalModAssetImporterDefinition> assetImporterDefinitions;
    std::vector<ExternalModWorldAuthoringDefinition> worldAuthoringDefinitions;
    std::vector<ExternalModCollisionAuthoringDefinition> collisionAuthoringDefinitions;
    std::vector<ExternalModEditorInspectorDefinition> editorInspectorDefinitions;
    std::vector<ExternalModWorldPersistenceDefinition> worldPersistenceDefinitions;
    std::vector<ExternalModWorldStorageDomainDefinition> worldStorageDefinitions;
    std::vector<ExternalModWorldSpawnProfileDefinition> worldSpawnProfiles;
    std::vector<ExternalModWorldTimeWeatherDefinition> worldTimeWeatherDefinitions;
    std::vector<ExternalModWorldSeedingDefinition> worldSeedingDefinitions;
    std::vector<ExternalModWorldMigrationDefinition> worldMigrationDefinitions;
    std::vector<ExternalModPersistenceInspectorDefinition> persistenceInspectorDefinitions;
    std::vector<ExternalModNarrativeFlagDefinition> narrativeFlagDefinitions;
    std::vector<ExternalModNarrativeDialogueDefinition> narrativeDialogueDefinitions;
    std::vector<ExternalModNarrativeQuestDefinition> narrativeQuestDefinitions;
    std::vector<ExternalModNarrativeTimelineDefinition> narrativeTimelineDefinitions;
    std::vector<ExternalModNarrativeInspectorDefinition> narrativeInspectorDefinitions;
    std::vector<ExternalModDevHotReloadDefinition> devHotReloadDefinitions;
    std::vector<ExternalModDevConsoleCommandDefinition> devConsoleDefinitions;
    std::vector<ExternalModDevWatcherDefinition> devWatcherDefinitions;
    std::vector<ExternalModWasmSandboxDefinition> wasmSandboxDefinitions;
    std::vector<ExternalModReloadInspectorDefinition> reloadInspectorDefinitions;
    std::vector<ExternalModDslDocument> dslDocuments;
    ExternalModSettingsSchemaDefinition settingsSchema;
    std::vector<ExternalModFxPresetDefinition> fxPresets;
    std::vector<ExternalModStateDefinition> stateDefinitions;
    std::vector<ExternalModSpellDefinition> spellDefinitions;
    bool hasEffectImpactPosition = false;
    float effectImpactPosX = 0.0f;
    float effectImpactPosY = 0.0f;
    float effectImpactPosZ = 0.0f;
    bool useProfileSpawnedShockwave = false;
    struct StatusEffectState {
        uintptr_t actorAddress = 0;
        int16_t actorId = -1;
        std::string statusId;
        std::string sourceModId;
        ExternalModStatusType statusType = ExternalModStatusType::Freeze;
        bool isPlayerTarget = false;
        int32_t framesRemaining = 0;
        int32_t totalDurationFrames = 0;
        int32_t tickFrames = 15;
        int32_t tickCountdown = 15;
        int32_t damagePerTick = 1;
        int32_t shakeFrames = 0;
        int32_t intensity = 255;
        float speedMultiplier = 0.5f;
        float jumpMultiplier = 1.5f;
        float strengthMultiplier = 2.0f;
        float weaknessMultiplier = 2.0f;
        float blindSkipChance = 0.35f;
        float blindYawJitterDeg = 20.0f;
        float baseX = 0.0f;
        float baseY = 0.0f;
        float baseZ = 0.0f;
        int16_t baseRotX = 0;
        int16_t baseRotY = 0;
        int16_t baseRotZ = 0;
        bool fallbackLogged = false;
        int32_t stacks = 1;
        bool justApplied = true;
        std::string stackingMode = "refresh";
        int32_t maxStacks = 1;
        ExternalModFreezeProfile freezeProfile;
        ExternalModLevitationProfile levitationProfile;
        float baseGravity = -1.0f;
        uintptr_t freezeShellActorAddress = 0;
        std::vector<std::string> loopFxHandleKeys;
    };
    struct ActiveFxHandleState {
        int32_t handle = 0;
        std::string key;
        uintptr_t actorAddress = 0;
        int32_t ttlFrames = 0;
        std::string source;
    };
    struct ActiveStateState {
        uintptr_t actorAddress = 0;
        int16_t actorId = -1;
        std::string stateId;
        std::string domain;
        std::string sourceModId;
        int32_t framesRemaining = 0;
        bool wasAppliedThisFrame = true;
    };
    struct ActiveAoEState {
        std::string profileId;
        std::string sourceItemId;
        int32_t framesRemaining = 0;
        int32_t tickFrames = 1;
        int32_t tickCountdown = 1;
        float originX = 0.0f;
        float originY = 0.0f;
        float originZ = 0.0f;
    };
    struct InventoryExtPageState {
        std::string pageId;
        int32_t slotCount = 0;
        std::vector<std::string> slots;
    };
    struct ContainerProcessState {
        std::string containerId;
        std::string recipeId;
        int32_t remainingMs = 0;
        int32_t totalMs = 0;
    };
    struct NavPathState {
        int32_t handle = 0;
        std::string routeId;
        std::vector<ExternalModRouteWaypoint> points;
    };
    struct DynamicLightState {
        int32_t handle = 0;
        std::string profileId;
        std::string profileOwnerModId;
        uintptr_t actorAddress = 0;
        int16_t actorId = -1;
        uint32_t actorHandle = 0;
        float posX = 0.0f;
        float posY = 0.0f;
        float posZ = 0.0f;
        bool hasWorldPos = false;
        bool follow = true;
        std::string attach = "player";
        int32_t remainingMs = 0;
    };
    struct MaterialOverrideState {
        std::string match;
        std::string materialId;
        std::string scope;
        int32_t framesRemaining = 0;
    };
    struct PlayerResourceState {
        std::string resourceId;
        float currentValue = 0.0f;
        float capacityValue = 0.0f;
        bool initialized = false;
        bool depleted = false;
        bool recovering = false;
        bool consumedThisFrame = false;
        int32_t regenDelayRemainingMs = 0;
        int32_t visibleMs = 0;
        int32_t periodicEffectTimerMs = 0;
        int16_t lastSceneInitialized = -1;
        int16_t lastRoomInitialized = -1;
    };
    struct PersistentEntityState {
        std::string entityGuid;
        std::string scope = "room";
        int16_t sceneId = -1;
        int16_t roomId = -1;
        std::unordered_map<std::string, std::string> values;
    };
    struct NarrativeQuestState {
        std::string questId;
        std::string state;
        std::unordered_map<std::string, std::string> objectives;
    };
    struct NarrativeTimelineState {
        std::string timelineId;
        int32_t frame = 0;
        int32_t durationFrames = 0;
        bool skippable = true;
    };
    struct SurfState {
        bool active = false;
        std::string sourceModId;
        std::string movementProfileId;
        std::string sourceItemId;
        int32_t framesRemaining = 0;
        float speed = 0.0f;
        int16_t headingYaw = 0;
        float boardPosX = 0.0f;
        float boardPosY = 0.0f;
        float boardPosZ = 0.0f;
        int16_t boardRotX = 0;
        int16_t boardRotY = 0;
        int16_t boardRotZ = 0;
        float boardScale = 1.0f;
        int32_t boostCooldownRemaining = 0;
        bool idle = false;
    };
    std::vector<StatusEffectState> statusEffects;
    std::vector<ActiveFxHandleState> activeFxHandles;
    std::vector<ActiveStateState> activeStates;
    std::vector<ActiveAoEState> activeAoEs;
    std::vector<InventoryExtPageState> inventoryExtPages;
    std::vector<ContainerProcessState> activeContainerProcesses;
    std::unordered_map<int32_t, NavPathState> activeNavPaths;
    std::vector<DynamicLightState> activeDynamicLights;
    std::vector<MaterialOverrideState> materialOverrides;
    std::unordered_map<std::string, PlayerResourceState> playerResourceStates;
    std::unordered_map<std::string, int32_t> consumableStackCounts;
    std::unordered_map<int32_t, ExternalModBottleContentState> customBottleContentBySlot;
    std::unordered_map<int32_t, std::string> customInventoryConsumableBySlot;
    std::unordered_set<std::string> openUiScreens;
    std::unordered_map<std::string, bool> debugOverlayVisibility;
    std::string activeSceneProfileId;
    std::string activeRoomProfileId;
    std::string activePostFxPresetId;
    std::string activeSkylightProfileId;
    std::string activePostFxOwnerModId;
    int32_t activePostFxDurationMs = 0;
    int32_t activePostFxRemainingMs = 0;
    float activePostFxBlend = 1.0f;
    SurfState surfState;
    int32_t nextFxHandle = 1;
    int32_t nextNavPathHandle = 1;
    int32_t nextDynamicLightHandle = 1;
    std::unordered_map<std::string, int32_t> fxHandleByKey;
    std::unordered_map<std::string, int32_t> spellCooldownsById;
    std::unordered_map<std::string, bool> permissionGrants;
    std::unordered_map<std::string, std::string> globalBlackboard;
    std::unordered_map<int16_t, std::unordered_map<std::string, std::string>> sceneBlackboard;
    std::unordered_map<std::string, std::unordered_map<std::string, std::string>> persistentDomains;
    std::unordered_map<std::string, std::string> settingsGlobalValues;
    std::unordered_map<std::string, std::string> settingsSaveValues;
    std::unordered_map<std::string, std::string> settingsSessionValues;
    std::unordered_map<std::string, std::string> pendingSceneReloadSettings;
    std::unordered_map<std::string, std::string> pendingRestartSettings;
    bool settingsGlobalDirty = false;
    bool settingsSaveDirty = false;
    std::unordered_map<std::string, PersistentEntityState> persistentEntities;
    std::unordered_map<uint32_t, std::string> persistentEntityGuidByActorHandle;
    bool persistentStateDirty = false;
    std::unordered_map<std::string, std::string> narrativeFlags;
    std::unordered_map<std::string, NarrativeQuestState> narrativeQuests;
    std::unordered_map<std::string, NarrativeTimelineState> activeTimelines;
    std::string activeDialogueId;
    std::string activeDialogueNodeId;
    std::string worldTimeSegment = "day";
    std::string worldWeatherState = "clear";
    int32_t worldSeed = 0;
    bool hotReloadEnabled = false;
    int32_t hotReloadDebounceMs = 250;
    int32_t hotReloadCooldownMs = 0;
    int32_t sandboxMaxCallMs = 2;
    int32_t sandboxMaxFrameBudgetMs = 2;
    int32_t sandboxMaxHookCallsPerFrame = 256;
    bool sandboxDeterministicMode = false;
    std::unordered_set<std::string> sandboxAllowedSurfaces;
    std::vector<std::pair<uint32_t, std::string>> pendingSignals;
    int16_t lastSceneSeen = -1;
    int16_t lastRoomSeen = -1;
    int16_t lastSkyboxSeen = -1;
    bool hasLastDayNight = false;
    bool lastIsNight = false;
    bool switchSnapshotInitialized = false;
    std::array<uint8_t, 64> switchSnapshot{};
    uint32_t nextActorHandle = 1;
    int32_t frameBudgetMs = 2;
    int32_t maxHookCallsPerFrame = 256;
    int32_t hookCallsThisFrame = 0;
    int32_t maxActorInstances = 64;
    int32_t wasmCallsThisFrame = 0;
    int32_t wasmBudgetDropsThisFrame = 0;
    std::unordered_map<int32_t, uintptr_t> wasmTargetHandles;
    int32_t wasmNextTargetHandle = 1;
    int32_t behaviorMaxStepsPerActorPerFrame = 64;
    int32_t behaviorMaxStepsPerModPerFrame = 5000;
    int32_t maxActiveStatusEffects = 256;
    int32_t behaviorStepsThisFrame = 0;
    ExternalModRuntimeKind kind = ExternalModRuntimeKind::Wasm;
    ExternalModNativeMode nativeMode = ExternalModNativeMode::None;
    ExternalModRuntimeModuleFormat moduleFormat = ExternalModRuntimeModuleFormat::WasmBinary;
    std::string moduleSourcePath;
    size_t compiledModuleSizeBytes = 0;
    int32_t moduleCompileTimeMs = 0;
    std::string moduleCompileDiagnostics;
    std::string nativeLibrarySourcePath;
    std::string nativeLibraryLoadedPath;
    int32_t nativeAbiVersion = 1;
    std::string nativeReloadPolicy = "manual";
    std::string nativeBuildIdPolicy = "ignore";
    std::string nativeThreadModel = "main_thread";
    std::string nativePluginName;
    std::string nativePluginVersion;
    std::string nativePluginAuthor;
    std::string nativePluginBuildId;
    bool nativeUnsafeRaw = false;
    bool nativeLoadedFromCache = false;
    std::vector<std::filesystem::path> generatedAssetPaths;
    std::vector<std::shared_ptr<Ship::Archive>> generatedAssetArchives;
    std::unique_ptr<ExternalModWasmRuntime> wasmRuntime;
    std::unique_ptr<ExternalModNativeRuntime> nativeRuntime;
};

struct ExternalModPackage {
    std::filesystem::path sourcePath;
    std::filesystem::path dataRootPath;
    std::filesystem::path runtimeRootPath;
    std::filesystem::path manifestPath;
    std::string zipDataPrefix;
    std::string zipRuntimePrefix;
    bool isZip = false;
    bool isSplitLayout = false;
    bool valid = false;
    std::string error;
    std::vector<ExternalModIssue> issues;
    ExternalModSeveritySummary severitySummary;
    ExternalModManifest manifest;
    std::vector<std::filesystem::path> mountedAssets;
    ExternalModRuntime runtime;
};

struct ExternalModHookEventContext {
    int16_t scene = -1;
    int16_t actorId = -1;
    int16_t actorCategory = -1;
    int32_t actorHandle = 0;
    int16_t itemId = -1;
    int16_t flagType = -1;
    int16_t flagId = -1;
    int16_t healthDelta = 0;
    std::string statusId;
    std::string stateId;
    std::string settingsKey;
    std::string settingsDomain;
    std::string settingsApplyMode;
    std::string resourceId;
    std::string value;
    float resourceCurrentValue = 0.0f;
    float resourceCapacityValue = 0.0f;
    float resourcePercent = 0.0f;
    int32_t stackCount = 0;
};

struct ExternalModInventoryCellView {
    size_t index = 0;
    bool occupied = false;
    std::string modId;
    std::string modName;
    std::string itemId;
    std::string displayName;
    ExternalModItemSlot slot = ExternalModItemSlot::Hookshot;
    bool hasSlot = true;
    ExternalModItemPlacement placement = ExternalModItemPlacement::Legacy;
    bool granted = false;
    uint8_t assignableButtonsMask = 0x7F;
    uint8_t assignedButtonsMask = 0;
    const uint8_t* iconRgba32 = nullptr;
};

} // namespace SOH

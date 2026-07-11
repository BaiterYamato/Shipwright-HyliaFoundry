[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$docsDir = Join-Path $resolvedRoot "docs"

if (-not (Test-Path $docsDir)) {
    throw "docs directory not found: $docsDir"
}

$generatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")

$catalogs = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    runtime = @{
        type = "wasm3-v1|native-cpp-v1|hybrid-v1"
        budgetDefaults = @{
            maxCallMs = 2
            maxFrameBudgetMs = 2
            maxHookCallsPerFrame = 256
            maxActorInstances = 64
            maxActiveStatuses = 256
        }
        settingsDomains = @("global", "save", "session")
        settingsApplyModes = @("realtime", "scene_reload", "restart")
    }
    capabilities = @(
        @{ id = "hooks.extended.v1"; fileField = "hookDefinitions"; fileDefault = "hooks/hooks.json" },
        @{ id = "actors.vm.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "actors.generic.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "behaviors.graph.v1"; fileField = "behaviorDefinitions"; fileDefault = "behaviors/behaviors.json" },
        @{ id = "scenes.bundle.v1"; fileField = "sceneDefinitions"; fileDefault = "scenes/scenes.json" },
        @{ id = "statuses.catalog.v1"; fileField = "statusDefinitions"; fileDefault = "statuses/statuses.json"; schemaVersion = 1 },
        @{ id = "combat.damage.v1"; fileField = "damageDefinitions"; fileDefault = "combat/damage_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.targeting.v1"; fileField = "targetingDefinitions"; fileDefault = "combat/targeting_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.projectiles.v1"; fileField = "projectileDefinitions"; fileDefault = "combat/projectiles.json"; schemaVersion = 1 },
        @{ id = "combat.aoe.v1"; fileField = "aoeDefinitions"; fileDefault = "combat/aoe_profiles.json"; schemaVersion = 1 },
        @{ id = "movement.profiles.v1"; fileField = "movementDefinitions"; fileDefault = "movement/movement_profiles.json"; schemaVersion = 1 },
        @{ id = "items.use_profiles.v1"; fileField = "itemUseProfiles"; fileDefault = "items/use_profiles.json"; schemaVersion = 1 },
        @{ id = "patches.vanilla_items.v1"; fileField = "vanillaItemPatches"; fileDefault = "patches/vanilla_items.patch.json" },
        @{ id = "world.queries.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables world query actions." },
        @{ id = "input.bindings.v2"; fileField = "inputDefinitions"; fileDefault = "config/input.json"; schemaVersion = 1 },
        @{ id = "ui.runtime.v1"; fileField = "uiScreenDefinitions"; fileDefault = "ui/screens.json"; schemaVersion = 1 },
        @{ id = "ui.hud.v1"; fileField = "uiHudDefinitions"; fileDefault = "ui/hud_layouts.json"; schemaVersion = 1 },
        @{ id = "player.resources.v1"; fileField = "playerResourceDefinitions"; fileDefault = "player/player_resources.json"; schemaVersion = 1 },
        @{ id = "ui.resource_rings.v1"; fileField = "resourceRingDefinitions"; fileDefault = "ui/resource_rings.json"; schemaVersion = 1 },
        @{ id = "player.consumables.v1"; fileField = "playerConsumableDefinitions"; fileDefault = "player/player_consumables.json"; schemaVersion = 1 },
        @{ id = "ui.inventory_ext.v1"; fileField = "inventoryExtensionDefinitions"; fileDefault = "inventory_ext/pages.json"; schemaVersion = 1 },
        @{ id = "containers.v1"; fileField = "containerDefinitions"; fileDefault = "containers/containers.json"; schemaVersion = 1 },
        @{ id = "recipes.processing.v1"; fileField = "recipeDefinitions"; fileDefault = "recipes/processing_recipes.json"; schemaVersion = 1 },
        @{ id = "interactions.v1"; fileField = "interactionDefinitions"; fileDefault = "interactions/interactions.json"; schemaVersion = 1 },
        @{ id = "actors.archetypes.v1"; fileField = "actorArchetypeDefinitions"; fileDefault = "actors/archetypes.json"; schemaVersion = 1 },
        @{ id = "actors.adapters.v1"; fileField = "actorAdapterDefinitions"; fileDefault = "actors/adapters.json"; schemaVersion = 1 },
        @{ id = "ai.behavior_trees.v1"; fileField = "behaviorTreeDefinitions"; fileDefault = "ai/behavior_trees.json"; schemaVersion = 1 },
        @{ id = "ai.sensors.v1"; fileField = "sensorDefinitions"; fileDefault = "ai/sensors.json"; schemaVersion = 1 },
        @{ id = "nav.routes.v1"; fileField = "routeDefinitions"; fileDefault = "nav/routes.json"; schemaVersion = 1 },
        @{ id = "nav.navmesh_bridge.v1"; fileField = "navBridgeDefinitions"; fileDefault = "nav/nav_bridge.json"; schemaVersion = 1 },
        @{ id = "debug.overlay.v1"; fileField = "debugOverlayDefinitions"; fileDefault = "debug/overlays.json"; schemaVersion = 1 },
        @{ id = "items.state_machine.v1"; fileField = "itemStateDefinitions"; fileDefault = "items/item_states.json"; schemaVersion = 1 },
        @{ id = "render.equipped_models.v1"; fileField = "equippedModelDefinitions"; fileDefault = "render/equipped_models.json"; schemaVersion = 1 },
        @{ id = "hud.widgets.v1"; fileField = "hudWidgetDefinitions"; fileDefault = "ui/widgets.json"; schemaVersion = 1 },
        @{ id = "hud.reticles.v2"; fileField = "hudReticleDefinitions"; fileDefault = "ui/reticles.json"; schemaVersion = 1 },
        @{ id = "camera.aim_profiles.v2"; fileField = "cameraDefinitions"; fileDefault = "camera/camera_profiles.json"; schemaVersion = 1 },
        @{ id = "effects.graph.v2"; fileField = "effectGraphDefinitions"; fileDefault = "effects/effect_graphs.json"; schemaVersion = 1 },
        @{ id = "combat.hit_rules.v2"; fileField = "combatHitRuleDefinitions"; fileDefault = "combat/hit_rules.json"; schemaVersion = 1 },
        @{ id = "movement.surf.v2"; fileField = "surfDefinitions"; fileDefault = "movement/surf_profiles.json"; schemaVersion = 1 },
        @{ id = "actors.tags.v1"; fileField = "actorTagDefinitions"; fileDefault = "actors/actor_tags.json"; schemaVersion = 1 },
        @{ id = "world.patchsets.v1"; fileField = "worldPatchDefinitions"; fileDefault = "world/patchsets.json"; schemaVersion = 1 },
        @{ id = "quests.graph.v1"; fileField = "questDefinitions"; fileDefault = "quests/quests.json"; schemaVersion = 1 },
        @{ id = "dialog.nodes.v1"; fileField = "dialogDefinitions"; fileDefault = "dialog/dialogs.json"; schemaVersion = 1 },
        @{ id = "sdk.generators.v1"; fileField = "sdkGeneratorDefinitions"; fileDefault = "sdk/generators.json"; schemaVersion = 1 },
        @{ id = "fx.presets.v1"; fileField = "fxPresetDefinitions"; fileDefault = "fx/fx_presets.json"; schemaVersion = 1 },
        @{ id = "states.catalog.v1"; fileField = "stateDefinitions"; fileDefault = "states/states.json"; schemaVersion = 1 },
        @{ id = "spells.catalog.v1"; fileField = "spellDefinitions"; fileDefault = "spells/spells.json"; schemaVersion = 1 },
        @{ id = "render.materials.v1"; fileField = "materialDefinitions"; fileDefault = "render/materials.json"; schemaVersion = 1 },
        @{ id = "render.pbr.v1"; fileField = "pbrDefinitions"; fileDefault = "render/pbr_profiles.json"; schemaVersion = 1 },
        @{ id = "render.lighting.v1"; fileField = "lightingDefinitions"; fileDefault = "render/light_profiles.json"; schemaVersion = 1 },
        @{ id = "render.postfx.v1"; fileField = "postFxDefinitions"; fileDefault = "render/postfx_presets.json"; schemaVersion = 1 },
        @{ id = "world.scenes.v1"; fileField = "sceneProfileDefinitions"; fileDefault = "world/scene_profiles.json"; schemaVersion = 1 },
        @{ id = "world.rooms.v1"; fileField = "roomProfileDefinitions"; fileDefault = "world/room_profiles.json"; schemaVersion = 1 },
        @{ id = "assets.packs.v2"; fileField = "assetPackDefinitions"; fileDefault = "assets/packs.json"; schemaVersion = 1 },
        @{ id = "assets.raw.v1"; fileField = "assetSourceDefinitions"; fileDefault = "assets/sources.json"; schemaVersion = 1; note = "Declares raw loose assets that can be prepared for native host access." },
        @{ id = "render.meshes.v1"; fileField = "meshDefinitions"; fileDefault = "render/meshes.json"; schemaVersion = 1; note = "Registers reusable meshes using the same external model pipeline as custom item models." },
        @{ id = "actors.prefabs.v1"; fileField = "prefabDefinitions"; fileDefault = "actors/prefabs.json"; schemaVersion = 1; note = "Declares visual prefabs that resolve render.meshes.v1 entries for actor/world placement." },
        @{ id = "world.instances.v1"; fileField = "worldInstanceDefinitions"; fileDefault = "world/instances.json"; schemaVersion = 1; note = "Places prefab-backed world visuals by scene and optional room filters." },
        @{ id = "editor.placement.v1"; fileField = "editorPlacementDefinitions"; fileDefault = "editor/placement.json"; schemaVersion = 1; note = "Registers reusable placeable entries that point at prefab visuals and carry default editor/world placement hints." },
        @{ id = "world.authoring.v1"; fileField = "worldAuthoringDefinitions"; fileDefault = "world/authoring.json"; schemaVersion = 1; note = "Places authored prefab visuals using direct prefab references or editor placement library entries." },
        @{ id = "debug.render_inspector.v1"; fileField = "renderInspectorDefinitions"; fileDefault = "debug/render_inspector.json"; schemaVersion = 1 },
        @{ id = "world.persistence.v1"; fileField = "worldPersistenceDefinitions"; fileDefault = "world/persistence.json"; schemaVersion = 1 },
        @{ id = "world.storage.v1"; fileField = "worldStorageDefinitions"; fileDefault = "world/storage_domains.json"; schemaVersion = 1 },
        @{ id = "world.spawn_profiles.v1"; fileField = "worldSpawnProfileDefinitions"; fileDefault = "world/spawn_profiles.json"; schemaVersion = 1 },
        @{ id = "world.forage.v1"; fileField = "worldForageDefinitions"; fileDefault = "world/forage.json"; schemaVersion = 1 },
        @{ id = "world.time_weather.v1"; fileField = "worldTimeWeatherDefinitions"; fileDefault = "world/time_weather.json"; schemaVersion = 1 },
        @{ id = "world.seeding.v1"; fileField = "worldSeedingDefinitions"; fileDefault = "world/seeding.json"; schemaVersion = 1 },
        @{ id = "world.migrations.v1"; fileField = "worldMigrationDefinitions"; fileDefault = "world/migrations.json"; schemaVersion = 1 },
        @{ id = "debug.persistence_inspector.v1"; fileField = "persistenceInspectorDefinitions"; fileDefault = "debug/persistence_inspector.json"; schemaVersion = 1 },
        @{ id = "narrative.timeline.v1"; fileField = "narrativeTimelineDefinitions"; fileDefault = "narrative/timelines.json"; schemaVersion = 1 },
        @{ id = "narrative.dialogue.v1"; fileField = "narrativeDialogueDefinitions"; fileDefault = "narrative/dialogues.json"; schemaVersion = 1 },
        @{ id = "narrative.quests.v1"; fileField = "narrativeQuestDefinitions"; fileDefault = "narrative/quests.json"; schemaVersion = 1 },
        @{ id = "narrative.flags.v1"; fileField = "narrativeFlagDefinitions"; fileDefault = "narrative/flags.json"; schemaVersion = 1 },
        @{ id = "debug.narrative_inspector.v1"; fileField = "narrativeInspectorDefinitions"; fileDefault = "debug/narrative_inspector.json"; schemaVersion = 1 },
        @{ id = "dev.hot_reload.v1"; fileField = "devHotReloadDefinitions"; fileDefault = "dev/hot_reload.json"; schemaVersion = 1 },
        @{ id = "dev.console.v1"; fileField = "devConsoleDefinitions"; fileDefault = "dev/console_commands.json"; schemaVersion = 1 },
        @{ id = "dev.watchers.v1"; fileField = "devWatcherDefinitions"; fileDefault = "dev/watchers.json"; schemaVersion = 1 },
        @{ id = "native.sdk.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables runtime.entryLibrary loading via the stable native SDK ABI." },
        @{ id = "native.raw_cpp.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables raw native engine interop and requires explicit raw permission grants." },
        @{ id = "wasm.sandbox.v2"; fileField = "wasmSandboxDefinitions"; fileDefault = "runtime/wasm_sandbox.json"; schemaVersion = 1 },
        @{ id = "debug.reload_inspector.v1"; fileField = "reloadInspectorDefinitions"; fileDefault = "debug/reload_inspector.json"; schemaVersion = 1 }
    )
    contracts = @{
        statuses = @{
            freezeProfileMode = @("legacy_timer", "ice_trap_no_damage")
            coreStatusPresets = @("core:freeze_ice_trap_no_damage")
        }
        aoe = @{
            targetScope = @("all_non_player", "enemies_bosses", "enemies_bosses_props", "player_enemies_bosses", "all_with_player")
        }
        worldGraphics = @{
            sceneHooks = @("onWorldSceneLoaded", "onWorldRoomEntered", "onWorldRoomExited")
            tickHooks = @("onWorldOverworldTick", "onWorldTimeOfDayChanged", "onWorldSkyboxChanged")
            renderActions = @("render.setPostFxPreset", "render.spawnLight", "render.setSkylight", "render.overrideMaterial")
        }
        settings = @{
            schemaFile = "settings/settings.schema.json"
            actions = @("settings.get", "settings.set", "settings.reset", "settings.list")
            event = "settings.changed"
        }
        playerResources = @{
            manifestFields = @("playerResourceDefinitions", "resourceRingDefinitions")
            fileDefaults = @("player/player_resources.json", "ui/resource_rings.json")
            actions = @("setResourceValue", "addResourceValue", "consumeResource", "refillResource", "setResourceCapacity")
            conditions = @("resourceIsEmpty", "resourceBelowPercent", "resourceCanConsume")
            hooks = @("onResourceChanged", "onResourceDepleted", "onResourceRecovered", "onResourceCapacityChanged")
        }
        playerConsumables = @{
            manifestFields = @("playerConsumableDefinitions")
            fileDefaults = @("player/player_consumables.json")
            actions = @("grantConsumableStack", "consumeConsumableStack", "fillActiveBottleContent")
            conditions = @("consumableStackAtLeast", "activeItemIsEmptyBottle", "playerInWater")
        }
        worldForage = @{
            manifestFields = @("worldForageDefinitions")
            fileDefaults = @("world/forage.json")
            sources = @("tall_grass.cut")
        }
        persistence = @{
            rootPath = "<save_root>/external_mods/persist/<worldSlotId>/<modId>/"
            domainFiles = @("manifest.json", "entities/<scene>_<room>.json", "domains/*.json")
        }
        narrative = @{
            keyActions = @("narrative.startDialogue", "narrative.chooseOption", "narrative.startQuest", "narrative.startTimeline")
        }
    }
}

$actions = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    actions = @(
        @{ name = "showNotification"; category = "core" },
        @{ name = "teleportToEntrance"; category = "core" },
        @{ name = "loadModScene"; category = "scene" },
        @{ name = "pressButton"; category = "core" },
        @{ name = "showEquippedItemGet"; category = "core" },
        @{ name = "spawnSmoke"; category = "fx" },
        @{ name = "spawnKusa"; category = "fx" },
        @{ name = "lanternLight"; category = "fx" },
        @{ name = "applyStatus"; category = "status" },
        @{ name = "clearStatus"; category = "status" },
        @{ name = "clearAllStatuses"; category = "status" },
        @{ name = "useItemProfile"; category = "items" },
        @{ name = "dealDamage"; category = "combat" },
        @{ name = "spawnProjectile"; category = "combat" },
        @{ name = "spawnAoE"; category = "combat" },
        @{ name = "applyMovementProfile"; category = "movement" },
        @{ name = "applyImpulse"; category = "movement" },
        @{ name = "getGroundInfo"; category = "world_query" },
        @{ name = "raycast"; category = "world_query" },
        @{ name = "raycastAll"; category = "world_query" },
        @{ name = "spawnActor"; category = "actor" },
        @{ name = "despawnActor"; category = "actor" },
        @{ name = "setActorState"; category = "actor" },
        @{ name = "moveActorToPathNode"; category = "actor" },
        @{ name = "openDialog"; category = "dialog" },
        @{ name = "setSwitchFlag"; category = "flags" },
        @{ name = "clearSwitchFlag"; category = "flags" },
        @{ name = "setEventChkInf"; category = "flags" },
        @{ name = "clearEventChkInf"; category = "flags" },
        @{ name = "setInfTable"; category = "flags" },
        @{ name = "clearInfTable"; category = "flags" },
        @{ name = "giveRupees"; category = "economy" },
        @{ name = "takeRupees"; category = "economy" },
        @{ name = "grantModItem"; category = "items" },
        @{ name = "revokeModItem"; category = "items" },
        @{ name = "setVar"; category = "behavior" },
        @{ name = "addVar"; category = "behavior" },
        @{ name = "clampVar"; category = "behavior" },
        @{ name = "emitSignal"; category = "behavior" },
        @{ name = "callBehavior"; category = "behavior" },
        @{ name = "toggleAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraProfile"; category = "camera" },
        @{ name = "fx.spawnEffectSs"; category = "fx" },
        @{ name = "fx.spawnActorFx"; category = "fx" },
        @{ name = "fx.spawnPreset"; category = "fx" },
        @{ name = "fx.stopFx"; category = "fx" },
        @{ name = "states.applyState"; category = "state" },
        @{ name = "states.clearState"; category = "state" },
        @{ name = "states.hasState"; category = "state" },
        @{ name = "player.getStateFlags"; category = "player" },
        @{ name = "player.setStateFlag"; category = "player" },
        @{ name = "player.clearStateFlag"; category = "player" },
        @{ name = "player.setControlLock"; category = "player" },
        @{ name = "player.setGravityScale"; category = "player" },
        @{ name = "player.setBoostType"; category = "player" },
        @{ name = "player.setDamageResponse"; category = "player" },
        @{ name = "setResourceValue"; category = "player_resource" },
        @{ name = "addResourceValue"; category = "player_resource" },
        @{ name = "consumeResource"; category = "player_resource" },
        @{ name = "refillResource"; category = "player_resource" },
        @{ name = "setResourceCapacity"; category = "player_resource" },
        @{ name = "grantConsumableStack"; category = "player_consumable" },
        @{ name = "consumeConsumableStack"; category = "player_consumable" },
        @{ name = "fillActiveBottleContent"; category = "player_consumable" },
        @{ name = "settings.get"; category = "settings" },
        @{ name = "settings.set"; category = "settings" },
        @{ name = "settings.reset"; category = "settings" },
        @{ name = "settings.list"; category = "settings" },
        @{ name = "spells.castSpell"; category = "spells" },
        @{ name = "ui.openScreen"; category = "ui" },
        @{ name = "ui.closeScreen"; category = "ui" },
        @{ name = "ui.toggleScreen"; category = "ui" },
        @{ name = "ui.focusNext"; category = "ui" },
        @{ name = "ui.focusPrev"; category = "ui" },
        @{ name = "inventoryExt.createPage"; category = "inventory_ext" },
        @{ name = "inventoryExt.moveItem"; category = "inventory_ext" },
        @{ name = "inventoryExt.save"; category = "inventory_ext" },
        @{ name = "inventoryExt.load"; category = "inventory_ext" },
        @{ name = "container.open"; category = "container" },
        @{ name = "container.moveItem"; category = "container" },
        @{ name = "container.startProcess"; category = "container" },
        @{ name = "container.cancelProcess"; category = "container" },
        @{ name = "container.getProgress"; category = "container" },
        @{ name = "actors.spawnArchetype"; category = "actors" },
        @{ name = "actors.despawnArchetype"; category = "actors" },
        @{ name = "interactions.invoke"; category = "interactions" },
        @{ name = "ai.runBehavior"; category = "ai" },
        @{ name = "ai.setBlackboard"; category = "ai" },
        @{ name = "ai.clearBlackboard"; category = "ai" },
        @{ name = "sense.findTargets"; category = "sensors" },
        @{ name = "sense.lineOfSight"; category = "sensors" },
        @{ name = "sense.distance"; category = "sensors" },
        @{ name = "nav.requestPath"; category = "nav" },
        @{ name = "nav.getPathPoints"; category = "nav" },
        @{ name = "nav.releasePath"; category = "nav" },
        @{ name = "debug.showOverlay"; category = "debug" },
        @{ name = "debug.hideOverlay"; category = "debug" },
        @{ name = "world.setSceneProfile"; category = "world" },
        @{ name = "world.setRoomProfile"; category = "world" },
        @{ name = "render.setPostFxPreset"; category = "render" },
        @{ name = "render.spawnLight"; category = "render" },
        @{ name = "render.setSkylight"; category = "render" },
        @{ name = "render.overrideMaterial"; category = "render" },
        @{ name = "persist.ensureEntityGuid"; category = "persistence" },
        @{ name = "persist.saveEntityState"; category = "persistence" },
        @{ name = "persist.loadEntityState"; category = "persistence" },
        @{ name = "persist.deleteEntityState"; category = "persistence" },
        @{ name = "persist.setDomainValue"; category = "persistence" },
        @{ name = "persist.getDomainValue"; category = "persistence" },
        @{ name = "persist.runMigrations"; category = "persistence" },
        @{ name = "world.spawnFromProfile"; category = "world" },
        @{ name = "world.time.setOverride"; category = "world" },
        @{ name = "world.weather.setOverride"; category = "world" },
        @{ name = "narrative.startDialogue"; category = "narrative" },
        @{ name = "narrative.chooseOption"; category = "narrative" },
        @{ name = "narrative.advanceDialogue"; category = "narrative" },
        @{ name = "narrative.setFlag"; category = "narrative" },
        @{ name = "narrative.clearFlag"; category = "narrative" },
        @{ name = "narrative.startQuest"; category = "narrative" },
        @{ name = "narrative.updateObjective"; category = "narrative" },
        @{ name = "narrative.startTimeline"; category = "narrative" },
        @{ name = "narrative.skipTimeline"; category = "narrative" },
        @{ name = "dev.reloadAll"; category = "dev" },
        @{ name = "dev.reloadTarget"; category = "dev" },
        @{ name = "dev.console.exec"; category = "dev" },
        @{ name = "invokeWasm"; category = "wasm" }
    )
    removedInApiV4 = @(
        "igniteFrontTarget",
        "freezeFrontTarget",
        "items.params.freezeOnMeleeHit",
        "items.params.freezeOnHitDuration",
        "items.params.freezeOnHitShake",
        "items.params.freezeOnHitIntensity"
    )
    wasmHostImports = @(
        "host_useItemProfile",
        "host_resolveTarget",
        "host_dealDamage",
        "host_applyStatus",
        "host_spawnProjectile",
        "host_spawnAoE",
        "host_applyMovementProfile",
        "host_applyImpulse",
        "host_getGroundInfo",
        "host_raycast",
        "host_raycastAll"
    )
    disableReasonCodes = @(
        "BUDGET_HOOK",
        "BUDGET_WASM_CALL",
        "BUDGET_WASM_FRAME",
        "WASM_IMPORT_FAIL",
        "WASM_EXPORT_FAIL",
        "WASM_RUNTIME_ERROR"
    )
}

$events = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    hooks = @(
        "onLoadGame",
        "onExitGame",
        "onSceneInit",
        "afterSceneCommands",
        "onTransitionEnd",
        "onFlagSet",
        "onFlagUnset",
        "onSceneFlagSet",
        "onSceneFlagUnset",
        "onPlayerUpdate",
        "onPlayerUseItem",
        "onPlayerHealthChange",
        "onItemReceive",
        "onActorInit",
        "onActorSpawn",
        "onActorUpdate",
        "onActorKill",
        "onActorDestroy",
        "onEnemyDefeat",
        "onBossDefeat",
        "onStatusApplied",
        "onStatusTick",
        "onStatusExpired",
        "onStateApplied",
        "onStateRemoved",
        "onUiScreenOpened",
        "onUiScreenClosed",
        "onUiAction",
        "onContainerSlotChanged",
        "onProcessStart",
        "onProcessTick",
        "onProcessComplete",
        "onArchetypeSpawn",
        "onArchetypeDespawn",
        "onInteraction",
        "onBehaviorNodeChanged",
        "onPathRequested",
        "onPathFailed",
        "onWorldSceneLoaded",
        "onWorldRoomEntered",
        "onWorldRoomExited",
        "onWorldOverworldTick",
        "onWorldTimeOfDayChanged",
        "onWorldSkyboxChanged",
        "onPersistentEntityLoaded",
        "onPersistentEntitySaved",
        "onPersistentDomainMigrated",
        "onWorldSpawnProfileTick",
        "world.time.segmentChanged",
        "world.weather.changed",
        "onDialogueStarted",
        "onDialogueChoiceCommitted",
        "onQuestStateChanged",
        "onTimelineStarted",
        "onTimelineCompleted",
        "onTimelineSkipped",
        "onHotReloadApplied",
        "onHotReloadFailed",
        "onSandboxBudgetExceeded",
        "onSandboxPermissionDenied",
        "onResourceChanged",
        "onResourceDepleted",
        "onResourceRecovered",
        "onResourceCapacityChanged",
        "onPlayDestroy",
        "onGameFrameUpdate"
    )
    behaviorEvents = @(
        "manual",
        "onInit",
        "onSpawn",
        "onDespawn",
        "onDestroy",
        "onUpdate",
        "onRandomTick",
        "onRoomEnter",
        "onTimeOfDayChanged",
        "onSwitchFlagChanged",
        "onPlayerNear",
        "onPlayerFar",
        "onTimer",
        "onInteract",
        "onSignal",
        "onSceneEnter",
        "onItemUsed",
        "onItemGranted",
        "onItemEquipped",
        "onCooldownReady",
        "settings.changed"
    )
    aliases = @(
        @{ from = "oninit"; to = "onspawn" },
        @{ from = "ondespawn"; to = "ondestroy" },
        @{ from = "onitemequipped"; to = "onitemgranted" }
    )
}

$actionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for action validation/dispatch contracts."
    actions = @(
        @{ name = "applyStatus"; params = @("status", "target?", "actorHandle?", "durationFrames?") },
        @{ name = "useItemProfile"; params = @("profile|useProfile|profileId") },
        @{ name = "dealDamage"; params = @("profile|damageProfileId", "target?", "actorHandle?") },
        @{ name = "spawnProjectile"; params = @("projectile|profile|projectileProfileId") },
        @{ name = "spawnAoE"; params = @("aoe|profile|aoeProfileId") },
        @{ name = "applyMovementProfile"; params = @("movement|profile|movementProfileId", "durationFrames?") },
        @{ name = "applyImpulse"; params = @("mode?", "strength?") },
        @{ name = "getGroundInfo"; params = @() },
        @{ name = "raycast"; params = @("range?") },
        @{ name = "raycastAll"; params = @("range?") },
        @{ name = "toggleAimCameraMode"; params = @("profileId?", "itemId?") },
        @{ name = "setAimCameraMode"; params = @("mode", "profileId?", "itemId?") },
        @{ name = "setAimCameraProfile"; params = @("profileId") },
        @{ name = "fx.spawnEffectSs"; params = @("name|effect|effectId", "scale?", "lifeFrames?", "attachFollow?", "storeKey?") },
        @{ name = "fx.spawnActorFx"; params = @("actorId", "overlay?", "scale?", "lifeFrames?", "attachFollow?", "storeKey?") },
        @{ name = "fx.spawnPreset"; params = @("presetId|preset", "storeKey?") },
        @{ name = "fx.stopFx"; params = @("handleKey|storeKey|key") },
        @{ name = "states.applyState"; params = @("stateId|state", "durationFrames?", "domain?") },
        @{ name = "states.clearState"; params = @("stateId|state", "domain?") },
        @{ name = "states.hasState"; params = @("stateId|state", "storeKey?") },
        @{ name = "player.getStateFlags"; params = @("storeKey?") },
        @{ name = "player.setStateFlag"; params = @("flag") },
        @{ name = "player.clearStateFlag"; params = @("flag") },
        @{ name = "player.setControlLock"; params = @("enabled") },
        @{ name = "player.setGravityScale"; params = @("scale") },
        @{ name = "player.setBoostType"; params = @("mode") },
        @{ name = "player.setDamageResponse"; params = @("mode") },
        @{ name = "setResourceValue"; params = @("resourceId|resource", "value") },
        @{ name = "addResourceValue"; params = @("resourceId|resource", "delta|value|amount") },
        @{ name = "consumeResource"; params = @("resourceId|resource", "amount|value") },
        @{ name = "refillResource"; params = @("resourceId|resource", "amount?") },
        @{ name = "setResourceCapacity"; params = @("resourceId|resource", "value|capacity") },
        @{ name = "grantConsumableStack"; params = @("consumableId|consumable", "amount|value?") },
        @{ name = "consumeConsumableStack"; params = @("consumableId|consumable", "amount|value?") },
        @{ name = "fillActiveBottleContent"; params = @("consumableId|consumable") },
        @{ name = "settings.get"; params = @("key", "domain?", "storeKey?") },
        @{ name = "settings.set"; params = @("key", "value", "domain?") },
        @{ name = "settings.reset"; params = @("key?", "domain?") },
        @{ name = "settings.list"; params = @("storeKey?", "domain?") },
        @{ name = "spells.castSpell"; params = @("spellId|spell") },
        @{ name = "ui.openScreen"; params = @("screen|screenId") },
        @{ name = "ui.closeScreen"; params = @("screen|screenId") },
        @{ name = "ui.toggleScreen"; params = @("screen|screenId") },
        @{ name = "ui.focusNext"; params = @() },
        @{ name = "ui.focusPrev"; params = @() },
        @{ name = "inventoryExt.createPage"; params = @("pageId", "slots") },
        @{ name = "inventoryExt.moveItem"; params = @("srcBinding|src", "dstBinding|dst", "count?") },
        @{ name = "inventoryExt.save"; params = @() },
        @{ name = "inventoryExt.load"; params = @() },
        @{ name = "container.open"; params = @("containerId|container") },
        @{ name = "container.moveItem"; params = @("srcBinding|src", "dstBinding|dst", "count?") },
        @{ name = "container.startProcess"; params = @("containerId|container", "recipeId?") },
        @{ name = "container.cancelProcess"; params = @("containerId|container") },
        @{ name = "container.getProgress"; params = @("containerId|container", "storeKey?") },
        @{ name = "actors.spawnArchetype"; params = @("archetypeId|archetype") },
        @{ name = "actors.despawnArchetype"; params = @("actorHandle|handle|id") },
        @{ name = "interactions.invoke"; params = @("interactionId|interaction") },
        @{ name = "ai.runBehavior"; params = @("behaviorId|behavior") },
        @{ name = "ai.setBlackboard"; params = @("key", "value") },
        @{ name = "ai.clearBlackboard"; params = @("key") },
        @{ name = "sense.findTargets"; params = @("storeKey?") },
        @{ name = "sense.lineOfSight"; params = @("range?", "storeKey?") },
        @{ name = "sense.distance"; params = @("storeKey?") },
        @{ name = "nav.requestPath"; params = @("routeId?", "storeKey?") },
        @{ name = "nav.getPathPoints"; params = @("handleKey|storeKey") },
        @{ name = "nav.releasePath"; params = @("handleKey|storeKey") },
        @{ name = "debug.showOverlay"; params = @("overlayId|overlay") },
        @{ name = "debug.hideOverlay"; params = @("overlayId|overlay") },
        @{ name = "world.setSceneProfile"; params = @("profileId|profile", "sceneId?") },
        @{ name = "world.setRoomProfile"; params = @("profileId|profile", "roomId", "sceneId?") },
        @{ name = "render.setPostFxPreset"; params = @("presetId|profileId|preset", "durationMs?", "blend?") },
        @{ name = "render.spawnLight"; params = @("profileId|profile", "actorHandle?", "lifetimeMs?|durationMs?", "storeKey?") },
        @{ name = "render.setSkylight"; params = @("profileId|profile") },
        @{ name = "render.overrideMaterial"; params = @("materialId|material", "match?", "scope?", "durationFrames?") },
        @{ name = "persist.ensureEntityGuid"; params = @("entityGuid?", "scope?") },
        @{ name = "persist.saveEntityState"; params = @("entityGuid?", "key?", "value?") },
        @{ name = "persist.loadEntityState"; params = @("entityGuid?", "key?") },
        @{ name = "persist.deleteEntityState"; params = @("entityGuid?") },
        @{ name = "persist.setDomainValue"; params = @("domainId|domain", "key", "value") },
        @{ name = "persist.getDomainValue"; params = @("domainId|domain", "key") },
        @{ name = "persist.runMigrations"; params = @("migrationId?") },
        @{ name = "world.spawnFromProfile"; params = @("profileId|profile") },
        @{ name = "world.time.setOverride"; params = @("segment|value") },
        @{ name = "world.weather.setOverride"; params = @("weather|value") },
        @{ name = "narrative.startDialogue"; params = @("dialogueId|id") },
        @{ name = "narrative.chooseOption"; params = @("optionId|id") },
        @{ name = "narrative.advanceDialogue"; params = @("nodeId?") },
        @{ name = "narrative.setFlag"; params = @("flagId|key", "value?") },
        @{ name = "narrative.clearFlag"; params = @("flagId|key") },
        @{ name = "narrative.startQuest"; params = @("questId|id") },
        @{ name = "narrative.updateObjective"; params = @("questId", "objectiveId", "state?") },
        @{ name = "narrative.startTimeline"; params = @("timelineId|id") },
        @{ name = "narrative.skipTimeline"; params = @("timelineId?") },
        @{ name = "dev.reloadAll"; params = @() },
        @{ name = "dev.reloadTarget"; params = @("target|registry") },
        @{ name = "dev.console.exec"; params = @("commandId|id") },
        @{ name = "invokeWasm"; params = @("export", "args?") }
    )
}

$conditionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for behavior condition contracts."
    conditions = @(
        @{ name = "isChild"; fields = @() },
        @{ name = "isAdult"; fields = @() },
        @{ name = "isDay"; fields = @() },
        @{ name = "isNight"; fields = @() },
        @{ name = "randomChance"; fields = @("value|numberValue") },
        @{ name = "hasItem"; fields = @("value") },
        @{ name = "hasStatus"; fields = @("scope?", "key|value", "op?") },
        @{ name = "statusRemaining"; fields = @("scope?", "key|value", "op", "value|numberValue") },
        @{ name = "distanceToPlayer"; fields = @("op?", "numberValue") },
        @{ name = "sceneIs"; fields = @("op?", "value|numberValue") },
        @{ name = "roomIs"; fields = @("op?", "value|numberValue") },
        @{ name = "hasSwitchFlag"; fields = @("op?", "value|numberValue") },
        @{ name = "var"; fields = @("scope", "key", "op?", "value") },
        @{ name = "resourceIsEmpty"; fields = @("resourceId|resource") },
        @{ name = "resourceBelowPercent"; fields = @("resourceId|resource", "percent|value|numberValue") },
        @{ name = "resourceCanConsume"; fields = @("resourceId|resource", "amount|value|numberValue") },
        @{ name = "consumableStackAtLeast"; fields = @("consumableId|consumable", "amount|value|numberValue") },
        @{ name = "activeItemIsEmptyBottle"; fields = @() },
        @{ name = "playerInWater"; fields = @() }
    )
}

$outputs = @(
    @{ path = (Join-Path $docsDir "catalogs.json"); data = $catalogs },
    @{ path = (Join-Path $docsDir "actions.json"); data = $actions },
    @{ path = (Join-Path $docsDir "events.json"); data = $events },
    @{ path = (Join-Path $docsDir "runtime_contract/actions.registry.json"); data = $actionsRegistry },
    @{ path = (Join-Path $docsDir "runtime_contract/conditions.registry.json"); data = $conditionsRegistry }
)

foreach ($out in $outputs) {
    $parentDir = Split-Path -Path $out.path -Parent
    if (-not [string]::IsNullOrWhiteSpace($parentDir) -and -not (Test-Path $parentDir)) {
        if (-not $DryRun) {
            New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
        }
    }
    $json = $out.data | ConvertTo-Json -Depth 12
    if ($DryRun) {
        Write-Host "[DryRun] Would write $($out.path)"
        continue
    }
    Set-Content -Path $out.path -Value $json -Encoding UTF8
    Write-Host "Wrote $($out.path)"
}

$docsGeneratorScript = Join-Path $resolvedRoot "tools/external_mods/generate_contract_docs.ps1"
if (Test-Path $docsGeneratorScript) {
    & $docsGeneratorScript -RepoRoot $resolvedRoot -OutDir "docs/generated" -DryRun:$DryRun *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "generate_contract_docs.ps1 failed while exporting runtime references"
    }
    if (-not $DryRun) {
        Write-Host "Generated docs indexes under docs/generated"
    } else {
        Write-Host "[DryRun] Would generate docs indexes under docs/generated"
    }
} else {
    Write-Warning "docs generator script not found: $docsGeneratorScript"
}

# Sylian Foundry Modloader (API v4)

This is the canonical entry point for data-driven external mods in Ship of Harkinian / Hylian Foundry.

> Contract baseline: `apiVersion: 4`

## What it is

The **Sylian Foundry Modloader** loads external mod packages (folder/zip), validates capability-gated JSON contracts, mounts assets, and runs behavior/WASM/native logic with per-mod isolation.

## Start here (quick path)

1. Read this file first.
2. Pick a demo under `docs/examples/external_mods/`.
3. Confirm your `mod.json` uses `apiVersion: 4`.
4. Implement item/effect logic with catalogs + behaviors.
5. Test in runtime and inspect logs (`x64/Release/logs/Ship of Harkinian.log`).

## Runtime contract baseline

- Required for new mods: `mod.json.apiVersion = 4`
- Runtime type: `wasm3-v1 | native-cpp-v1 | hybrid-v1`
- Native runtime fields:
  - `runtime.entryLibrary` for `native-cpp-v1|hybrid-v1`
  - `runtime.abiVersion`
  - `runtime.nativeMode = sdk|raw`
  - `runtime.reloadPolicy = disabled|manual|safe`
  - `runtime.buildIdPolicy = ignore|match`
  - `runtime.threadModel = main_thread|background_readonly`
- Native package layout (Windows-first):
  - `native/win64/<mod>.dll`
  - optional `native/win64/<mod>.pdb`
  - optional `native/plugins.json` for future native-side registry metadata
- Namespaced IDs required (`modId:*`, with `core:*` reserved for built-ins)
- External Mods UI category tag: `mod.json.uiCategory = core_api | mod`
- Capability-gated files: if capability is declared, required file/path must be valid
- Dependency graph is enforced (`dependencies[]` with semver ranges)

### v5.2 loader/UX polish baseline

- Manifest can optionally declare:
  - `provides`
  - `uses`
  - `files.optional`
  - `settings`
- Missing unused registries no longer hard-disable a mod.
- Severity model:
  - `INFO`
  - `WARN`
  - `ERROR`
  - `FATAL`
- Only `FATAL` disables a mod.
- External Mods details panel exposes:
  - health state (`Enabled`, `Enabled (Warnings)`, `Enabled (Errors)`, `Disabled (Fatal)`)
  - structured diagnostics
  - settings UI auto-generated from `settings/settings.schema.json`
  - copy-to-clipboard diagnostics

### v10 ecosystem/tooling baseline

- Official CLI: `tools/external_mods/modtool.ps1`
  - `init`, `validate`, `pack`, `sign`, `doctor`, `diff`, `report`
  - `pack` accepts `-ReleaseChannel stable|beta|testers` (non-stable artifacts are for manual/testing workflows)
  - `report` also refreshes generated docs indexes under `docs/generated/*`
  - contract docs generator script: `tools/external_mods/generate_contract_docs.ps1`
  - wrapper command: `tools/external_mods/foundry-cli.ps1 generate-contract-docs`
- Packaging contracts:
  - `docs/runtime_contract/mod.manifest.v2.json`
  - `docs/runtime_contract/mod.lockfile.v1.json`
  - `docs/runtime_contract/pack.manifest.v2.json`
  - `docs/runtime_contract/pack.materials_index.v1.json`
- Security/tooling contracts:
  - `docs/runtime_contract/security.capabilities.v2.json`
  - `docs/runtime_contract/security.permissions.v1.json`
  - `docs/runtime_contract/security.signing.v1.json`
  - `docs/runtime_contract/tool.validate_report.v1.json`
  - `docs/runtime_contract/tool.doctor_report.v1.json`
  - `docs/runtime_contract/tool.diff_report.v1.json`
- Framework onboarding wiki:
  - `docs/framework_wiki/README.md`

Channel policy:
- Manifests can declare `releaseChannels` (`stable|beta|testers`), but modloader UI must treat non-stable channels as gated until v1.0.

Permissions policy (v10 runtime):
- Requested permissions are read from `mod.json.permissions`.
- Optional rationales can be declared in `mod.json.permissionRationales`.
- Per-mod grants are persisted at `Save/external_mods/permissions/<modId>.json`.
- External Mods menu exposes per-mod permission toggles (saved per profile) plus bulk presets (`Allow Safe`, `Deny Risky`, `Allow All`, `Reset Defaults`).
- If a mod boots with denied risky permissions, runtime emits a guidance notification to open External Mods and review grants.
- Native DLL mods require `nativeinterop`; raw native DLL mods additionally require `nativeinterop.raw`.
- `engine.memory` and `editor.livelink` are permissioned surfaces reserved for native/editor flows.
- Current enforced action gates:
  - `filesystem` for `persist.*` actions
  - `process` for `dev.reload*` and `dev.console.exec`

Failure quarantine (v10 hardening):
- Repeated runtime failures are tracked per mod under `Save/external_mods/quarantine/<modId>.json`.
- After repeated failures, the mod is quarantined on next discovery and shown as disabled with clear reason text.
- External Mods UI exposes **Clear Quarantine** to allow retry (then use Reload External Mods).

## Capability and file map

Use `docs/catalogs.json` as machine-readable source of truth. Common capabilities include:

- `hooks.extended.v1` -> `hookDefinitions` (`hooks/hooks.json`)
- `behaviors.graph.v1` -> `behaviorDefinitions` (`behaviors/behaviors.json`)
- `statuses.catalog.v1` -> `statusDefinitions` (`statuses/statuses.json`)
- `combat.damage.v1` -> `damageDefinitions` (`combat/damage_profiles.json`)
- `combat.targeting.v1` -> `targetingDefinitions` (`combat/targeting_profiles.json`)
- `combat.projectiles.v1` -> `projectileDefinitions` (`combat/projectiles.json`)
- `combat.aoe.v1` -> `aoeDefinitions` (`combat/aoe_profiles.json`)
- `movement.profiles.v1` -> `movementDefinitions` (`movement/movement_profiles.json`)
- `items.use_profiles.v1` -> `itemUseProfiles` (`items/use_profiles.json`)
- `patches.vanilla_items.v1` -> `vanillaItemPatches` (`patches/vanilla_items.patch.json`)
- `world.queries.v1` -> no file required (enables world query actions)
- `items.state_machine.v1` -> `itemStateDefinitions` (`items/item_states.json`)
- `render.equipped_models.v1` -> `equippedModelDefinitions` (`render/equipped_models.json`)
- `hud.widgets.v1` -> `hudWidgetDefinitions` (`ui/widgets.json`)
- `hud.reticles.v2` -> `hudReticleDefinitions` (`ui/reticles.json`)
- `player.resources.v1` -> `playerResourceDefinitions` (`player/player_resources.json`)
- `ui.resource_rings.v1` -> `resourceRingDefinitions` (`ui/resource_rings.json`)
- `player.consumables.v1` -> `playerConsumableDefinitions` (`player/player_consumables.json`)
- `world.forage.v1` -> `worldForageDefinitions` (`world/forage.json`)
- `player.consumables.v1` bottle entries support `usesPerFill` + `placeholderPartialItemId` for Lon Lon Milk-style multi-use fills
- `player.consumables.v1` bottle entries can also provide `placeholderIconAsset` + `placeholderPartialIconAsset` to override the milk placeholder icon shown on equipped C-buttons
- `player.consumables.v1` stack entries can expose a vanilla inventory slot through `inventoryPlaceholderItemId` and route consumption through `consumeAnimation = drinkDemo`
- `camera.aim_profiles.v2` -> `cameraDefinitions` (`camera/camera_profiles.json`)
- `effects.graph.v2` -> `effectGraphDefinitions` (`effects/effect_graphs.json`)
- `combat.hit_rules.v2` -> `combatHitRuleDefinitions` (`combat/hit_rules.json`)
- `movement.surf.v2` -> `surfDefinitions` (`movement/surf_profiles.json`)
- `actors.tags.v1` -> `actorTagDefinitions` (`actors/actor_tags.json`)
- `world.patchsets.v1` -> `worldPatchDefinitions` (`world/patchsets.json`)
- `quests.graph.v1` -> `questDefinitions` (`quests/quests.json`)
- `dialog.nodes.v1` -> `dialogDefinitions` (`dialog/dialogs.json`)
- `sdk.generators.v1` -> `sdkGeneratorDefinitions` (`sdk/generators.json`)
- `fx.presets.v1` -> `fxPresetDefinitions` (`fx/fx_presets.json`)
- `states.catalog.v1` -> `stateDefinitions` (`states/states.json`)
- `spells.catalog.v1` -> `spellDefinitions` (`spells/spells.json`)

### v6 core extension capabilities

- `ui.runtime.v1` -> `uiScreenDefinitions` (`ui/screens.json`)
- `ui.hud.v1` -> `uiHudDefinitions` (`ui/hud_layouts.json`)
- `ui.inventory_ext.v1` -> `inventoryExtensionDefinitions` (`inventory_ext/pages.json`)
- `containers.v1` -> `containerDefinitions` (`containers/containers.json`)
- `recipes.processing.v1` -> `recipeDefinitions` (`recipes/processing_recipes.json`)
- `interactions.v1` -> `interactionDefinitions` (`interactions/interactions.json`)
- `actors.archetypes.v1` -> `actorArchetypeDefinitions` (`actors/archetypes.json`)
- `actors.adapters.v1` -> `actorAdapterDefinitions` (`actors/adapters.json`)
- `ai.behavior_trees.v1` -> `behaviorTreeDefinitions` (`ai/behavior_trees.json`)
- `ai.sensors.v1` -> `sensorDefinitions` (`ai/sensors.json`)
- `nav.routes.v1` -> `routeDefinitions` (`nav/routes.json`)
- `nav.navmesh_bridge.v1` -> `navBridgeDefinitions` (`nav/nav_bridge.json`)
- `debug.overlay.v1` -> `debugOverlayDefinitions` (`debug/overlays.json`)

### v7 world graphics extension capabilities

- `render.materials.v1` -> `materialDefinitions` (`materials/materials.json`)
- `render.pbr.v1` -> `pbrDefinitions` (`render/pbr_profiles.json`)
- `render.lighting.v1` -> `lightingDefinitions` (`lighting/light_profiles.json`)
- `render.postfx.v1` -> `postFxDefinitions` (`render/postfx_presets.json`)
- `world.scenes.v1` -> `sceneProfileDefinitions` (`world/scene_profiles.json`)
- `world.rooms.v1` -> `roomProfileDefinitions` (`world/room_profiles.json`)
- `assets.packs.v2` -> `assetPackDefinitions` (`assets/packs.json`)
- `debug.render_inspector.v1` -> `renderInspectorDefinitions` (`debug/render_inspector.json`)

### v9 persistence / narrative / dev extension capabilities

Persistence + living world:
- `world.persistence.v1` -> `worldPersistenceDefinitions` (`world/persistence.json`)
- `world.storage.v1` -> `worldStorageDefinitions` (`world/storage_domains.json`)
- `world.spawn_profiles.v1` -> `worldSpawnProfileDefinitions` (`world/spawn_profiles.json`)
- `world.time_weather.v1` -> `worldTimeWeatherDefinitions` (`world/time_weather.json`)
- `world.seeding.v1` -> `worldSeedingDefinitions` (`world/seeding.json`)
- `world.migrations.v1` -> `worldMigrationDefinitions` (`world/migrations.json`)
- `debug.persistence_inspector.v1` -> `persistenceInspectorDefinitions` (`debug/persistence_inspector.json`)

Narrative:
- `narrative.timeline.v1` -> `narrativeTimelineDefinitions` (`narrative/timelines.json`)
- `narrative.dialogue.v1` -> `narrativeDialogueDefinitions` (`narrative/dialogues.json`)
- `narrative.quests.v1` -> `narrativeQuestDefinitions` (`narrative/quests.json`)
- `narrative.flags.v1` -> `narrativeFlagDefinitions` (`narrative/flags.json`)
- `debug.narrative_inspector.v1` -> `narrativeInspectorDefinitions` (`debug/narrative_inspector.json`)

Dev iteration + sandbox:
- `dev.hot_reload.v1` -> `devHotReloadDefinitions` (`dev/hot_reload.json`)
- `dev.console.v1` -> `devConsoleDefinitions` (`dev/console_commands.json`)
- `dev.watchers.v1` -> `devWatcherDefinitions` (`dev/watchers.json`)
- `wasm.sandbox.v2` -> `wasmSandboxDefinitions` (`runtime/wasm_sandbox.json`)
- `debug.reload_inspector.v1` -> `reloadInspectorDefinitions` (`debug/reload_inspector.json`)

## Runtime lifecycle

1. **Discover** packages in mods folders
2. **Validate** manifest + capability files
3. **Reference-scan** known files to classify used registries and unresolved ids
4. **Register** catalogs, items, hooks, behaviors
4. **Run** actions/effects/WASM with budgets
5. **Reload/Shutdown** with deterministic cleanup

At runtime the loader applies flexible-contract rules:

- provided registry/file missing -> fatal
- optional/unused file missing -> warning or silent empty fallback
- unresolved reference -> error with degraded feature path where safe

### Settings runtime (v5.2)

Settings are part of the runtime surface even without a dedicated capability.

Supported actions:

- `settings.get`
- `settings.set`
- `settings.reset`
- `settings.list`

Event:

- `settings.changed`

Apply modes:

- `realtime`
- `scene_reload`
- `restart`

### Freeze no-damage preset (data-driven)

For Ice Trap-like freeze without periodic damage, use:

- `core:freeze_ice_trap_no_damage`

Or define a custom status with `baseStatus: core:freeze` + `freezeProfile.mode: ice_trap_no_damage`.

### SSS framework examples

- `docs/examples/external_mods/sss`
- `docs/examples/external_mods/sss_content_pack`

### v6 reference suite (frameworks + demos)

Framework layer:
- `docs/examples/external_mods/ui_kit`
- `docs/examples/external_mods/container_kit`
- `docs/examples/external_mods/ai_templates`

Content layer:
- `demo_mini_inventory_h`
- `demo_bag_inventory_ext`
- `demo_furnace_system`
- `sss_content_pack`
- `demo_npc_patrol`
- `demo_enemy_templates`
- `demo_vanilla_adapters`

These demonstrate a library-style mod (`com.sylian.sss`) plus a dependent content pack (`dependencies[]` enforced by loader).

### v5.2 polish demos

- `docs/examples/external_mods/framework_no_items_demo`
- `docs/examples/external_mods/unresolved_reference_demo`
- `docs/examples/external_mods/live_settings_realtime_demo`
- `docs/examples/external_mods/compat_wrapper_demo`

These prove:

- framework mods can stay enabled without unrelated `items.json`
- unresolved references degrade to `ERROR` diagnostics instead of auto-disable
- settings schema UI renders and applies realtime values safely
- wrapper compatibility can preserve legacy framework dependencies

### Player resource demo (planned branch contract)

- `docs/examples/external_mods/player_resources_stamina_botw_demo`
- `docs/examples/external_mods/player_survival_needs_demo`

Shows stamina authoring for `player.resources.v1` + `ui.resource_rings.v1`, sprint input bindings, settings-driven drain/regen, and hook notifications.
The survival needs demo extends the same framework with stacked hunger/thirst bars, custom consumable stacks, bottle-filled water, and tall-grass forage rules.

### v7 world graphics reference suite (frameworks + demos)

Framework layer:
- `docs/examples/external_mods/lighting_kit`
- `docs/examples/external_mods/world_profiles`
- `docs/examples/external_mods/material_helpers`

Content layer:
- `docs/examples/external_mods/demo_room_profiles`
- `docs/examples/external_mods/demo_actor_lights`
- `docs/examples/external_mods/demo_green_fire_magic`

### v9 reference suite (frameworks + demos)

Framework layer:
- `docs/examples/external_mods/persistence_kit`
- `docs/examples/external_mods/narrative_kit`
- `docs/examples/external_mods/devtools_kit`

Content layer:
- `docs/examples/external_mods/demo_persistent_tents`
- `docs/examples/external_mods/demo_loot_respawn`
- `docs/examples/external_mods/demo_dialogue_npc`
- `docs/examples/external_mods/demo_cutscene_intro`
- `docs/examples/external_mods/demo_quest_chain`
- `docs/examples/external_mods/demo_hot_reload_playground`

Authoring guides:
- `docs/examples/external_mods/v7_reference_suite_docs/PACK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/FRAMEWORK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/DEMO_PLAYBOOK.md`

### v7.1 Ambient Occlusion (SSAO)

`render/pbr_profiles.json` supports optional SSAO controls through `ambientOcclusion`:

- `enabled`
- `quality` (`low|medium|high`)
- `radius`, `intensity`, `bias`, `power`
- `maxDistance`, `blurPasses`

Runtime/backend behavior:

- OpenGL: SSAO supported.
- DirectX11: SSAO supported.
- Metal: AO fallback (render continues, AO disabled) with `onRenderFallbackApplied`.

Global override CVars:

- `gEnhancements.Graphics.AO.Enabled`
- `gEnhancements.Graphics.AO.Quality` (`0..3`)
- `gEnhancements.Graphics.AO.IntensityScale`
- `gEnhancements.Graphics.AO.DebugView`

### v7.2 Volumetric fog / lighting

`render/postfx_presets.json` supports an optional `volumetrics` block:

- `enabled`
- `quality` (`low|medium|high`)
- `density`, `anisotropy`
- `startDistance`, `maxDistance`
- `heightFogEnabled`, `baseHeight`, `heightFalloff`
- `lightShaftIntensity`, `shadowIntensity`
- `temporalBlend`, `jitterScale`
- `debugView`

`lighting/light_profiles.json` can opt lights into the volumetric pass with:

- `type` (`directional|point|spot`)
- `direction`
- `innerConeDeg`, `outerConeDeg`
- `castShadows`
- `shadowResolution`, `shadowBias`, `shadowNormalBias`, `shadowRange`
- `volumetricIntensity`

Runtime/backend behavior:

- OpenGL: volumetrics supported.
- DirectX11: volumetrics supported.
- Metal: explicit fallback event if the pass is unavailable.

Global override CVars:

- `gEnhancements.Graphics.Volumetrics.Enabled`
- `gEnhancements.Graphics.Volumetrics.Quality` (`0..3`, `0=auto`)
- `gEnhancements.Graphics.Volumetrics.DensityScale`
- `gEnhancements.Graphics.Volumetrics.ShadowQuality`
- `gEnhancements.Graphics.Volumetrics.DebugView`
- `gEnhancements.Graphics.Volumetrics.UseCustom`
- `gEnhancements.Graphics.Volumetrics.Color`
- `gEnhancements.Graphics.Volumetrics.AmbientIntensity`
- `gEnhancements.Graphics.Volumetrics.Anisotropy`
- `gEnhancements.Graphics.Volumetrics.StartDistance`
- `gEnhancements.Graphics.Volumetrics.MaxDistance`
- `gEnhancements.Graphics.Volumetrics.HeightFog`
- `gEnhancements.Graphics.Volumetrics.BaseHeight`
- `gEnhancements.Graphics.Volumetrics.HeightFalloff`
- `gEnhancements.Graphics.Volumetrics.LightShaftIntensity`
- `gEnhancements.Graphics.Volumetrics.ShadowIntensity`
- `gEnhancements.Graphics.Volumetrics.TemporalBlend`
- `gEnhancements.Graphics.Volumetrics.JitterScale`
- `gEnhancements.Graphics.Volumetrics.DebugMode`
- `gEnhancements.Graphics.PostFx.UseCustom`
- `gEnhancements.Graphics.PostFx.Exposure`
- `gEnhancements.Graphics.PostFx.Bloom`
- `gEnhancements.Graphics.PostFx.Saturation`
- `gEnhancements.Graphics.PostFx.FogColor`
- `gEnhancements.Graphics.PostFx.FogDensity`

Composition/order notes:

- SSAO runs before volumetrics when both are active.
- The initial volumetrics path forces single-sample rendering for the affected offscreen frame instead of combining with MSAA.
- DirectX11 volumetrics now include a colored medium fog term plus light scattering, so fog remains visible even in scenes with few volumetric lights.
- Example packs:
  - `docs/examples/external_mods/world_profiles/`
  - `docs/examples/external_mods/lighting_kit/`

## Safety and failure model

- Invalid mod data disables only the failing mod.
- Runtime budgets are enforced per mod.
- Logs include context (`modId`, file, JSON path when available).
- Reload clears mod-owned state to avoid orphan behavior.

## Where to extend

- Actions reference: `docs/actions.json`
- Events reference: `docs/events.json`
- Catalog/runtime capabilities: `docs/catalogs.json`
- Declarative action registry: `docs/runtime_contract/actions.registry.json`
- Declarative condition registry: `docs/runtime_contract/conditions.registry.json`

## Detailed references

- `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`
- `docs/EXTERNAL_MOD_MANAGER_REFERENCE.md`
- `docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md`
- `docs/examples/external_mods/`

## Notes for contributors

- Keep docs aligned with runtime exports (`tools/external_mods/export_runtime_reference.ps1`).
- Run drift checks before publishing docs updates (`check-doc-drift.ps1`).

# External Mods Data-Driven Reference (API v4)

Reference for JSON-driven mod content used by the Sylian Foundry Modloader.

> External mod contract baseline: `apiVersion: 4`

## 1) Core rules

- IDs must be namespaced (`modId:*`); `core:*` is reserved for built-ins.
- Declared capabilities must provide valid files/paths when required.
- Unknown/invalid schema versions fail the affected mod capability.

## 2) Manifest essentials (`mod.json`)

Typical fields:

- `id`, `name`, `version`, `apiVersion`
- `type` (`framework|content|tools`)
- `loadPriority`
- `dependencies` (`id`/`modId` + optional `versionRange`)
- `capabilities`
- `provides` (declared registries/content roots this mod owns)
- `uses` (optional hint list; runtime reference scan remains authoritative)
- `files.optional` (legacy escape hatch for non-fatal optional files)
- `settings` (`schemaVersion`, `schemaFile`, `defaultDomain`)
- `capabilityRationales` (optional object keyed by capability id)
- `permissionRationales` (optional object keyed by permission id)
- `uiCategory` (`core_api` or `mod`) for External Mods tab classification
- `releaseChannels` (`stable|beta|testers`; non-stable is gated in modloader UI until v1.0)
- `permissions` (`filesystem|network|process|nativeInterop`)
- Runtime security UX:
  - per-mod grants are persisted in `Save/external_mods/permissions/<modId>.json`
  - External Mods UI includes bulk permission presets (`Allow Safe`, `Deny Risky`, `Allow All`, `Reset Defaults`)
  - denied risky permissions trigger a startup guidance notification for that mod
- Runtime hardening:
  - repeated runtime-disable failures are quarantined in `Save/external_mods/quarantine/<modId>.json`
  - quarantined mods stay isolated until user clears quarantine from External Mods UI
- capability file fields (e.g., `damageDefinitions`, `itemUseProfiles`)
- optional runtime budgets (`runtime`)

Machine-readable mapping: `docs/catalogs.json` (`capabilities[]`).
Generated contract/registry indexes:
- `docs/generated/contract_index.json`
- `docs/generated/registry_index.json`

### Flexible contracts (v5.2)

The loader no longer treats unrelated missing files as hard blockers.

- Missing file for a registry declared in `provides` -> **FATAL**
- Missing file for an unused registry -> **WARN** + empty fallback
- Unresolved referenced id -> **ERROR** (feature degraded, mod stays enabled when safe)
- Only **FATAL** disables a mod

Structured diagnostics are exposed in runtime UI and validator output with:

- `severitySummary`
- `issues[]`
  - `severity`
  - `code`
  - `message`
  - `sourcePath`
  - `registryId`
  - `referencedId`
  - `suggestedFix`

## 3) Key content files

- `items/items.json`
- `items/use_profiles.json`
- `statuses/statuses.json`
- `combat/damage_profiles.json`
- `combat/targeting_profiles.json`
- `combat/projectiles.json`
- `combat/aoe_profiles.json`
- `movement/movement_profiles.json`
- `fx/fx_presets.json`
- `states/states.json`
- `spells/spells.json`
- `hooks/hooks.json`
- `behaviors/behaviors.json`
- `ui/screens.json`
- `ui/hud_layouts.json`
- `player/player_resources.json`
- `ui/resource_rings.json`
- `inventory_ext/pages.json`
- `containers/containers.json`
- `recipes/processing_recipes.json`
- `interactions/interactions.json`
- `actors/archetypes.json`
- `actors/adapters.json`
- `ai/behavior_trees.json`
- `ai/sensors.json`
- `nav/routes.json`
- `nav/nav_bridge.json`
- `debug/overlays.json`
- `settings/settings.schema.json` (optional, when `mod.json.settings.schemaVersion = 1`)

## 3.1) Settings schema (`settings/settings.schema.json`)

Settings are optional and data-driven.

Top-level fields:

- `version`
- `groups[]`
  - `id`, `label`, `order`
  - `entries[]`

Entry fields:

- `key`
- `type`: `bool|int|float|enum|string|color|keybind`
- `default`
- `label`, `help`
- `domain`: `global|save|session`
- `applyMode`: `realtime|scene_reload|restart`
- numeric clamps: `min|max|step`
- enum values: `enumValues[]`
- conditional visibility: `visibility.requiresCapability`, `visibility.requiresRegistry`, `visibility.experimental`

Runtime actions/events:

- `settings.get`
- `settings.set`
- `settings.reset`
- `settings.list`
- hook event: `settings.changed`

Persistence roots:

- global -> `Save/external_mods/settings/global/<modId>.json`
- save -> `Save/external_mods/settings/save/<slot>/<modId>.json`
- session -> in-memory only

## 4) Item definitions (`items/items.json`)

Common fields include:

- identifiers/display metadata
- `slot` / `placement`
- `useMode`
- `useProfile`
- `overrideVanillaItem`
- `aimReticleTextureAsset`
- `aimSelectToggle`
- `aimAttackButtonFire`
- `useTrigger` (`onUse` or `hammerGroundImpact`)

## 5) Use profiles (`items/use_profiles.json`)

Effects are ordered and data-driven. Common actions include:

- damage/status application
- projectile/AoE spawn
- movement profile application
- impulse/world-query dependent effects
- `spawnShockwave` for impact-style FX
- `spells.castSpell`

## 6) Combat catalogs

- Damage profiles support amount/type/flags and prop interaction policy.
- Targeting profiles resolve actor targets (front, lock-on, raycast, cone, sphere, self/player).
- Projectile and AoE profiles define hit/tick behavior, including target scope controls for AoE.
- AoE `targetScope` supports:
  - `all_non_player`
  - `enemies_bosses`
  - `enemies_bosses_props`
  - `player_enemies_bosses`
  - `all_with_player`

## 7) Status freeze profile (ice-trap no-damage mode)

`statuses/statuses.json` can extend a freeze status with:

- `freezeProfile.mode`: `legacy_timer | ice_trap_no_damage`
- `spawnIceShell`, `iceShellSize`
- `lockPosition`, `lockRotation`
- `playerInputLock`
- `breakEffectOnExpire`

Built-in reusable preset status id:

- `core:freeze_ice_trap_no_damage`

This mode freezes player/enemy/boss without periodic damage while preserving freeze lock behavior.

## 8) Status levitation profile (lift + suspend, no freeze)

`statuses/statuses.json` can extend a `core:high_jump` status with:

- `levitationProfile.mode`: `none | lift_suspend`
- `liftSpeed`, `holdHeight`
- `riseFrames`, `suspendFrames`
- `lockHorizontal`
- `gravityScaleWhileActive`

When `mode=lift_suspend`, targets are lifted and suspended by status runtime without using freeze/stun timers.

## 9) Movement profiles

`movement/movement_profiles.json` supports modifier and surf-oriented behavior with profile-defined parameters (speed/drag/slope/board behavior).

### Player resources and resource rings

Planned API v4 branch contract:

- `player.resources.v1` -> `playerResourceDefinitions` (`player/player_resources.json`)
- `ui.resource_rings.v1` -> `resourceRingDefinitions` (`ui/resource_rings.json`)
- `player.consumables.v1` -> `playerConsumableDefinitions` (`player/player_consumables.json`)
- `world.forage.v1` -> `worldForageDefinitions` (`world/forage.json`)
- `player/player_resources.json` typically declares `resources[]` with capacity, regen, persistence keys, and `actionRules[]`
- `resources[]` may also declare `tickRules[]`, `depletionEffects`, `lowThresholdPercent`, and `resourceLinks[]`
- `actionRules[]` map tags such as `player.sprint` to drain rules and optional `inputBindingId`
- `ui/resource_rings.json` declares `rings[]` with `resourceId`, `anchorMode` (`contextual|fixed|both`), `fixedAnchor` (`left|right|none|magic_bar_stack`), visibility timing, colors, and settings keys
- fixed bars can use `fixedStackGroup`, `fixedStackOrder`, `fixedStackSpacing`, `companionIconAsset`, and `companionCounterSource`
- `player/player_consumables.json` declares `consumables[]` with `storageMode` (`stack|bottleContent`), persistence keys, optional `consumeAnimation`, optional `inventoryPlaceholderItemId` for inventory-backed stack consumables, placeholder bottle visuals, optional `placeholderIconAsset` + `placeholderPartialIconAsset` for custom C-button bottle icons, optional `usesPerFill` + `placeholderPartialItemId` for multi-use bottle contents, and resource-restoring `effects[]`
- `world/forage.json` declares `rules[]` for environment sources such as `tall_grass.cut`
- actions: `setResourceValue`, `addResourceValue`, `consumeResource`, `refillResource`, `setResourceCapacity`
- actions: `grantConsumableStack`, `consumeConsumableStack`, `fillActiveBottleContent`
- conditions: `resourceIsEmpty`, `resourceBelowPercent`, `resourceCanConsume`
- conditions: `consumableStackAtLeast`, `activeItemIsEmptyBottle`, `playerInWater`
- hooks: `onResourceChanged`, `onResourceDepleted`, `onResourceRecovered`, `onResourceCapacityChanged`
- example pack: `docs/examples/external_mods/player_resources_stamina_botw_demo`
- example pack: `docs/examples/external_mods/player_survival_needs_demo`

## 10) Hooks and behavior actions

- Hook list: `docs/events.json` (`hooks`)
- Behavior events: `docs/events.json` (`behaviorEvents`)
- Action names: `docs/actions.json`
- New lifecycle hooks include:
  - `onStatusApplied`, `onStatusTick`, `onStatusExpired`
  - `onStateApplied`, `onStateRemoved`

## 11) FX presets, states and spells catalogs

- `fx/fx_presets.json`: reusable action bundles, invoked by `fx.spawnPreset`.
- `states/states.json`: actor/player-oriented state definitions with `apply[]` and `remove[]`.
- `spells/spells.json`: reusable effect pipelines with cooldown and optional targeting profile.

These catalogs are capability-gated by:

- `fx.presets.v1`
- `states.catalog.v1`
- `spells.catalog.v1`

## 12) v6 UI/container/actors/AI/nav/debug contracts

New capabilities:

- `ui.runtime.v1`, `ui.hud.v1`, `ui.inventory_ext.v1`
- `containers.v1`, `recipes.processing.v1`, `interactions.v1`
- `actors.archetypes.v1`, `actors.adapters.v1`
- `ai.behavior_trees.v1`, `ai.sensors.v1`
- `nav.routes.v1`, `nav.navmesh_bridge.v1`
- `debug.overlay.v1`

Common action families:

- `ui.*` (`openScreen`, `closeScreen`, `toggleScreen`, `focusNext`, `focusPrev`)
- `inventoryExt.*` (`createPage`, `moveItem`, `save`, `load`)
- `container.*` (`open`, `moveItem`, `startProcess`, `cancelProcess`, `getProgress`)
- `actors.*` (`spawnArchetype`, `despawnArchetype`)
- `interactions.invoke`
- `ai.*`, `sense.*`, `nav.*`, `debug.*`

New hook events:

- `onUiScreenOpened`, `onUiScreenClosed`, `onUiAction`
- `onContainerSlotChanged`, `onProcessStart`, `onProcessTick`, `onProcessComplete`
- `onArchetypeSpawn`, `onArchetypeDespawn`, `onInteraction`
- `onBehaviorNodeChanged`, `onPathRequested`, `onPathFailed`

## 13) Validation workflow

1. Validate schema/capabilities via runtime load.
2. Check logs for contextual parser/runtime errors.
3. Run `tools/external_mods/modtool.ps1 report` to emit validate+doctor diagnostics and regenerate docs indexes.
4. Use `modtool pack -ReleaseChannel <stable|beta|testers>` for deterministic channel-tagged artifacts.
3. Re-export references after contract changes.

```powershell
tools/external_mods/export_runtime_reference.ps1
```

## 14) Example packs

See `docs/examples/external_mods/` for runnable reference mods.

## 15) v7 world graphics contracts

New capabilities:

- `render.materials.v1`
- `render.pbr.v1`
- `render.lighting.v1`
- `render.postfx.v1`
- `world.scenes.v1`
- `world.rooms.v1`
- `assets.packs.v2`
- `debug.render_inspector.v1`

Capability-gated manifest fields:

- `materialDefinitions` -> `materials/materials.json`
- `pbrDefinitions` -> `render/pbr_profiles.json`
- `lightingDefinitions` -> `lighting/light_profiles.json`
- `postFxDefinitions` -> `render/postfx_presets.json`
- `sceneProfileDefinitions` -> `world/scene_profiles.json`
- `roomProfileDefinitions` -> `world/room_profiles.json`
- `assetPackDefinitions` -> `assets/packs.json`
- `renderInspectorDefinitions` -> `debug/render_inspector.json`

v7 action family additions:

- `world.setSceneProfile`
- `world.setRoomProfile`
- `render.setPostFxPreset`
- `render.spawnLight`
- `render.setSkylight`
- `render.overrideMaterial`

`render/pbr_profiles.json` supports optional SSAO controls:

```json
"ambientOcclusion": {
  "enabled": true,
  "quality": "low|medium|high",
  "radius": 0.55,
  "intensity": 0.9,
  "bias": 0.02,
  "power": 1.2,
  "maxDistance": 1200.0,
  "blurPasses": 2
}
```

Runtime precedence:

1. `gEnhancements.Graphics.AO.Enabled=0` disables AO globally.
2. If enabled globally, profile values are used (plus `AO.IntensityScale` multiplier).
3. If no profile is active, quality/CVar fallback is applied.

Backend support:

- OpenGL: SSAO pass supported.
- DirectX11: SSAO pass supported.
- Metal: fallback to non-AO rendering with `onRenderFallbackApplied`.

`render/postfx_presets.json` also supports optional volumetrics controls through `volumetrics`:

```json
"volumetrics": {
  "enabled": true,
  "quality": "low|medium|high",
  "density": 0.22,
  "color": [0.72, 0.84, 1.0],
  "ambientIntensity": 0.5,
  "anisotropy": 0.55,
  "startDistance": 35.0,
  "maxDistance": 1300.0,
  "heightFogEnabled": true,
  "baseHeight": -20.0,
  "heightFalloff": 0.0035,
  "lightShaftIntensity": 1.4,
  "shadowIntensity": 0.75,
  "temporalBlend": 0.82,
  "jitterScale": 0.9,
  "debugView": false
}
```

`lighting/light_profiles.json` now drives volumetric lights and per-light shadow budgets with:

- `type`: `directional|point|spot`
- `direction`, `innerConeDeg`, `outerConeDeg`
- `castShadows`
- `shadowResolution`, `shadowBias`, `shadowNormalBias`, `shadowRange`
- `volumetricIntensity`

Runtime/backend behavior for volumetrics:

- OpenGL: volumetrics pass supported.
- DirectX11: volumetrics pass supported.
- Metal: explicit fallback with `onRenderFallbackApplied` when the backend cannot run the pass.

Global volumetrics overrides:

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
- `gEnhancements.Graphics.Volumetrics.DebugMode` (`0=off`, `1=fog density`, `2=light energy`, `3=final volume`)

Advanced postfx overrides:

- `gEnhancements.Graphics.PostFx.UseCustom`
- `gEnhancements.Graphics.PostFx.Exposure`
- `gEnhancements.Graphics.PostFx.Bloom`
- `gEnhancements.Graphics.PostFx.Saturation`
- `gEnhancements.Graphics.PostFx.FogColor`
- `gEnhancements.Graphics.PostFx.FogDensity`

Notes:

- SSAO is applied before volumetrics when both are active.
- The first volumetrics implementation forces the affected frame onto the single-sample path instead of combining with MSAA.
- In DirectX11, volumetrics now accumulate both a colored medium fog term and light scattering, so the fog remains visible even with few dynamic lights.
- See runnable examples in:
  - `docs/examples/external_mods/world_profiles/render/postfx_presets.json`
  - `docs/examples/external_mods/world_profiles/lighting/light_profiles.json`
  - `docs/examples/external_mods/lighting_kit/lighting/light_profiles.json`

v7 extended hooks:

- `onWorldSceneLoaded`
- `onWorldRoomEntered`
- `onWorldRoomExited`
- `onWorldOverworldTick`
- `onWorldTimeOfDayChanged`
- `onWorldSkyboxChanged`

## 16) v9 persistence + narrative + dev iteration contracts

### New capabilities

Persistence / living world:

- `world.persistence.v1`
- `world.storage.v1`
- `world.spawn_profiles.v1`
- `world.time_weather.v1`
- `world.seeding.v1`
- `world.migrations.v1`
- `debug.persistence_inspector.v1`

Narrative:

- `narrative.timeline.v1`
- `narrative.dialogue.v1`
- `narrative.quests.v1`
- `narrative.flags.v1`
- `debug.narrative_inspector.v1`

Dev / sandbox:

- `dev.hot_reload.v1`
- `dev.console.v1`
- `dev.watchers.v1`
- `wasm.sandbox.v2`
- `debug.reload_inspector.v1`

### New action families

- Persistence: `persist.*` (`ensureEntityGuid`, `saveEntityState`, `loadEntityState`, `deleteEntityState`, `setDomainValue`, `getDomainValue`, `runMigrations`)
- World runtime: `world.spawnFromProfile`, `world.time.setOverride`, `world.weather.setOverride`
- Narrative: `narrative.*` (`startDialogue`, `chooseOption`, `advanceDialogue`, `setFlag`, `clearFlag`, `startQuest`, `updateObjective`, `startTimeline`, `skipTimeline`)
- Dev: `dev.reloadAll`, `dev.reloadTarget`, `dev.console.exec`

### New hooks

- `onPersistentEntityLoaded`
- `onPersistentEntitySaved`
- `onPersistentDomainMigrated`
- `onWorldSpawnProfileTick`
- `world.time.segmentChanged`
- `world.weather.changed`
- `onDialogueStarted`
- `onDialogueChoiceCommitted`
- `onQuestStateChanged`
- `onTimelineStarted`
- `onTimelineCompleted`
- `onTimelineSkipped`
- `onHotReloadApplied`
- `onHotReloadFailed`
- `onSandboxBudgetExceeded`
- `onSandboxPermissionDenied`

### Persistence backend path (v9)

External mod persistence is stored outside vanilla save sections:

`<save_root>/external_mods/persist/<worldSlotId>/<modId>/`

See also:
- `docs/PERSISTENCE_GUIDE.md`
- `docs/NARRATIVE_GUIDE.md`
- `docs/HOT_RELOAD_GUIDE.md`

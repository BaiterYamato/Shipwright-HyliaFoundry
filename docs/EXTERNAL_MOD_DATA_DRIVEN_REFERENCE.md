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
- `loadPriority`
- `dependencies` (`id`/`modId` + optional `versionRange`)
- `capabilities`
- `uiCategory` (`core_api` or `mod`) for External Mods tab classification
- capability file fields (e.g., `damageDefinitions`, `itemUseProfiles`)
- optional runtime budgets (`runtime`)

Machine-readable mapping: `docs/catalogs.json` (`capabilities[]`).

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

v7 extended hooks:

- `onWorldSceneLoaded`
- `onWorldRoomEntered`
- `onWorldRoomExited`
- `onWorldOverworldTick`
- `onWorldTimeOfDayChanged`
- `onWorldSkyboxChanged`

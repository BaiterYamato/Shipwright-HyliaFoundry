# Patch v7 — World Graphics Core API

## Scope
Deliver v7 world graphics/runtime contracts as a capability-gated extension on top of API v4.

## Objectives
1. Keep vanilla identical when no v7 mod/pack is active.
2. Introduce data-driven room/scene/world render controls.
3. Add material/PBR/light/postfx contracts that can be consumed by framework mods.
4. Keep failures isolated to the offending mod.

## Capability map
- `render.materials.v1` -> `materialDefinitions` (`materials/materials.json`)
- `render.pbr.v1` -> `pbrDefinitions` (`render/pbr_profiles.json`)
- `render.lighting.v1` -> `lightingDefinitions` (`lighting/light_profiles.json`)
- `render.postfx.v1` -> `postFxDefinitions` (`render/postfx_presets.json`)
- `world.scenes.v1` -> `sceneProfileDefinitions` (`world/scene_profiles.json`)
- `world.rooms.v1` -> `roomProfileDefinitions` (`world/room_profiles.json`)
- `assets.packs.v2` -> `assetPackDefinitions` (`assets/packs.json`)
- `debug.render_inspector.v1` -> `renderInspectorDefinitions` (`debug/render_inspector.json`)

## Runtime actions/hooks added
Actions:
- `world.setSceneProfile`
- `world.setRoomProfile`
- `render.setPostFxPreset`
- `render.spawnLight`
- `render.setSkylight`
- `render.overrideMaterial`

Hooks:
- `onWorldSceneLoaded`
- `onWorldRoomEntered`
- `onWorldRoomExited`
- `onWorldOverworldTick`
- `onWorldTimeOfDayChanged`
- `onWorldSkyboxChanged`

## Milestones
1. Contract + parser + validator support for v7 capabilities.
2. Runtime state plumbing (active scene/room/postfx/skylight, dynamic lights, material overrides).
3. Exported references updated (`actions/events/catalogs` + runtime registries).
4. Lifecycle safety (reload/disable/scene change cleanup).

## Acceptance
- Build passes (`Release` target `soh`).
- v7 capability files validate with contextual errors.
- v7 actions parse and execute without crashing.
- Hooks fire with deterministic scene/room/day-night transitions.

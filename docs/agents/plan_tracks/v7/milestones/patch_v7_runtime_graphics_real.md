# Patch v7.1 — Runtime Gráfico Real (World Graphics + Render Runtime)

## Scope
Turn v7 from parser/scaffolding into real in-frame world graphics behavior for External Mods API v4.

## Execution focus
1. Resolve scene/room/postfx/skylight state each draw frame with deterministic precedence.
2. Apply resolved state to `envCtx/lightCtx` safely (begin/end draw capture+restore).
3. Spawn/track dynamic lights with runtime lifecycle (attach/follow/world + expiry/cleanup).
4. Extend render actions/hooks and parser fields required for real runtime orchestration.

## Implemented runtime blocks
- Dedicated runtime module:
  - `ExternalModWorldGraphicsRuntime.{h,cpp}`
- Manager integration:
  - `OnPlayDrawBegin` / `OnPlayDrawEnd` hooks
  - lifecycle reset on load/scene/destroy/shutdown
- Real-time state resolution:
  - action override > room > scene > vanilla fallback
  - cross-package lookup by namespaced IDs and load priority
- Dynamic light pool execution:
  - insertion/removal via `LightContext_InsertLight/RemoveLight`
  - attach modes (`player|actor|world`), `follow`, `worldPos`
  - expiry + per-frame cleanup + render hooks for spawn/expire
- Parser/runtime extensions:
  - `materials` binds/maps/params runtime fields
  - `pbr_profiles` dynamic light budgets and backend policy knobs
  - `light_profiles` type/kelvin/falloff/cones/direction
  - `postfx_presets` fogNear/fogFar/vignette/saturation
  - new actions: `render.despawnLight`, `render.clearPostFxPreset`, `render.clearSkylight`, `render.clearMaterialOverrides`
  - new hooks: `onRenderProfileResolved`, `onRenderLightSpawned`, `onRenderLightExpired`, `onRenderFallbackApplied`

## Validation snapshot
- Reconfigure + Build: `cmake -S . -B build/x64` then Release build succeeded.
- Contract export succeeded:
  - `docs/actions.json`
  - `docs/events.json`
  - `docs/catalogs.json`
  - `docs/runtime_contract/actions.registry.json`
  - `docs/runtime_contract/conditions.registry.json`
- v7 demo validation passed:
  - `lighting_kit`, `world_profiles`, `material_helpers`,
  - `demo_room_profiles`, `demo_actor_lights`, `demo_green_fire_magic`
- Doc/runtime drift dry-run: zero issues.

## Remaining expansion for next block
1. Bind material overrides to renderer backend material provider path (full draw-time override propagation).
2. Complete PBR backend bridge parity checks for GL/DX11 and explicit Metal fallback telemetry path.
3. Expand render inspector overlay UI with per-light/material provenance panels.

# 03 - Capabilities and Permissions

## Canonical Ownership

- `framework_render_world` owns the render/world authoring surfaces: meshes, prefabs, world instances, placement, authoring, materials, PBR, lighting, postfx, scene profiles, room profiles, and render inspection.
- `framework_narrative` owns flags, dialogue, quests, timelines, and narrative inspection.
- `framework_persistence` owns persistence, storage, spawn profiles, time/weather, seeding, migrations, and persistence inspection.
- `framework_ui_inventory` owns UI runtime, HUD, inventory extensions, containers, processing recipes, and interactions.
- `framework_combat_ai` owns archetypes, adapters, behavior trees, sensors, routes, nav bridge, combat targeting/AOE, statuses, states, spells, FX presets, and shared use profiles.
- `framework_devtools_native` owns native SDK/raw interop, dev hot reload, console, watchers, wasm sandboxing, reload inspection, and native asset host access.

Consumer demos then add the remaining stable capability coverage such as hooks, player resources, movement profiles, camera aim profiles, and world queries.

## Permission Rules

- `nativeinterop` is required for hybrid/native packages that load a DLL.
- `nativeinterop.raw` is reserved for packages that intentionally use raw engine interop.
- `process` is limited to devtool-oriented packages.
- `filesystem`, `network`, and other elevated permissions still need explicit rationales.

## Rationale Policy

- Every declared capability should have a matching `capabilityRationales.<capability>`.
- Every declared permission should have a matching `permissionRationales.<permission>`.
- `doctor` and `validate_mod` treat missing rationales as structural debt and surface them immediately.

# Patch v7 — World Graphics Reference Suite

## Scope
Ship a runnable v7 reference suite composed of framework mods + content demos + authoring docs.

## Framework mods
1. `lighting_kit`
   - reusable light profiles (`torch_orange`, `sky_white`, `fire_green`, `magic_purple`)
   - render inspector overlay definition
2. `world_profiles`
   - scene/room profile presets
   - postfx presets + skylight profile references
3. `material_helpers`
   - material descriptors + pbr profile + asset pack metadata

## Content demos
1. `demo_room_profiles`
   - applies scene/room profiles from `world_profiles`
2. `demo_actor_lights`
   - spawns runtime light + sets skylight using `lighting_kit`
3. `demo_green_fire_magic`
   - postfx flash + green light spawn

## Companion docs
- `docs/examples/external_mods/v7_reference_suite_docs/PACK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/FRAMEWORK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/DEMO_PLAYBOOK.md`

## Delivery rules
1. Keep examples under `docs/examples/external_mods/*` as source of truth.
2. Mirror to runtime via `tools/external_mods/sync_examples_to_runtime.ps1 -Clean`.
3. Validate each new mod with `tools/external_mods/validate_mod.ps1`.
4. Re-export runtime references after contract changes.

## Acceptance
- All v7 frameworks/demos pass `validate_mod.ps1`.
- Sync script mirrors all new folders to `x64/Release/mods`.
- Docs index/readme includes v7 suite entries.
- No critical doc-runtime drift in dry-run check.

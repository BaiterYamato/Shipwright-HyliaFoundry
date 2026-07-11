# External Mods Catalog

This folder now exposes the canonical framework-first API v4 suite.

## Frameworks

- `framework_render_world` -> `com.sylian.framework.render_world`
- `framework_narrative` -> `com.sylian.framework.narrative`
- `framework_persistence` -> `com.sylian.framework.persistence`
- `framework_ui_inventory` -> `com.sylian.framework.ui_inventory`
- `framework_combat_ai` -> `com.sylian.framework.combat_ai`
- `framework_devtools_native` -> `com.sylian.framework.devtools_native`

All six frameworks are `hybrid-v1` and ship `scripts/` plus `native/` in the same package.

## Consumer Demos

- `demo_render_world_showcase`
- `demo_narrative_quest_chain`
- `demo_persistence_world_state`
- `demo_ui_inventory_flow`
- `demo_combat_ai_showcase`
- `demo_devtools_native_smoke`

These are the runnable examples that consume the canonical frameworks and cover the currently shipped API v4 capabilities.

## Split V3 Seed

- `free_mod_v3_seed`

This package is the canonical seed for `mod.manifest.v3`, split `mods_data` / `mods_runtime`, convention-first path discovery, the generic DSL roots (`components`, `entities`, `graphs`, `services`), and the shared host surface consumed from both `WASM` and native SDK plugins: `queryPublicJson` for metadata plus runtime state (`runtime`, `settings`, `world`, `narrative`, `storage`, `inventory`), `callService` for public query endpoints, and `invokeActionJson` for own-mod action execution without hard-coding another runtime path. The current seed also proves the first executable DSL composition slice: a `components/*.json` visual document synthesizes a prefab from `meshId`, an `entities/*.json` document binds that component, and `world/instances.json` plus `world/placement.json` reference the entity by `entityId` instead of hard-coded prefab ids.

## Reference Packs

- `reference_compat_bridge`
- `reference_framework_bootstrap`
- `reference_live_settings`
- `reference_spell_content_pack`
- `reference_unresolved_dependency`

These packages are valid reference cases for dependency wiring, live settings, spell content, and multi-framework manifests. They are no longer intentionally broken demos.

## Migration Map

| Old catalog | New canonical owner |
| --- | --- |
| `lighting_kit`, `material_helpers`, `world_profiles`, `lanterna_test` | `framework_render_world` |
| `narrative_kit` | `framework_narrative` |
| `persistence_kit` | `framework_persistence` |
| `ui_kit`, `container_kit` | `framework_ui_inventory` |
| `ai_templates`, `sss` | `framework_combat_ai` |
| `devtools_kit`, native/hybrid smokes | `framework_devtools_native` |
| `framework_no_items_demo` | `reference_framework_bootstrap` |
| `compat_wrapper_demo` | `reference_compat_bridge` |
| `live_settings_realtime_demo` | `reference_live_settings` |
| `sss_content_pack` | `reference_spell_content_pack` |
| `unresolved_reference_demo` | `reference_unresolved_dependency` |

The archived legacy tree lives under `docs/examples_legacy/external_mods_20260407`.

## Commands

Sync the active catalog into runtime buckets. Legacy packages still go to `x64/Release/mods/*`; split `v3` packages go to `x64/Release/mods_data/*` plus `x64/Release/mods_runtime/*`:

```powershell
pwsh ./tools/external_mods/sync_examples_to_runtime.ps1 -Clean
```

Scaffold a canonical framework:

```powershell
pwsh ./tools/external_mods/modtool.ps1 init -Template framework -Family render_world -Path docs/examples/external_mods/my_framework
```

Scaffold a consumer pack bound to a framework family:

```powershell
pwsh ./tools/external_mods/modtool.ps1 init -Template content -Family combat_ai -Path docs/examples/external_mods/my_demo
```

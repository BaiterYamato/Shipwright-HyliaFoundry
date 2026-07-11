# Framework Wiki

This wiki documents the framework-first API v4 topology that backs the active external mod catalog.

## Read Order

1. `01_thin_modloader_principles.md`
2. `02_framework_dependency_pattern.md`
3. `03_capabilities_permissions.md`
4. `04_migrations_budgets.md`

## Canonical Topology

- Frameworks are the primary distribution unit for reusable runtime surfaces.
- Content packs depend on frameworks through `{ id, versionRange }` and declare `uses[]`.
- Stable packages carry `manifest.lock.json`.
- Frameworks are `hybrid-v1` by default and keep `scripts/` plus `native/` together.

## Tooling

- Scaffold with `pwsh ./tools/external_mods/modtool.ps1 init -Template framework|content -Family <family>`
- Validate with `modtool validate`
- Audit structure and coverage with `modtool doctor`
- Audit hybrid/native packages with `modtool report-native`
- Sync the active catalog with `sync_examples_to_runtime.ps1`

## Reference Sources

- Active catalog: `docs/examples/external_mods`
- Archived legacy catalog: `docs/examples_legacy/external_mods_20260407`
- Runtime contracts: `docs/runtime_contract/*.json`

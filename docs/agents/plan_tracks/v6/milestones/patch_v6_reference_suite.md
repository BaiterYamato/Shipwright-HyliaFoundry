# Patch v6 Reference Suite Milestone

Plan: PLN-20260303-0004
Track: v6

## Scope

Create layered reference suite in `docs/examples/external_mods`:
- Framework mods
- Content demo mods depending on frameworks

## Framework mods created

- `ui_kit` (`com.sylian.ui_kit`)
- `container_kit` (`com.sylian.container_kit`)
- `ai_templates` (`com.sylian.ai_templates`)

## Content demos created

- `demo_mini_inventory_h`
- `demo_bag_inventory_ext`
- `demo_furnace_system`
- `demo_firewall_staff`
- `demo_purple_din_lev_glove`
- `demo_npc_patrol`
- `demo_enemy_templates`
- `demo_vanilla_adapters`

## Notes

- All demos/frameworks are API v4 manifests with capability-gated files.
- Dependencies are declared using `id + versionRange` to exercise loader dependency logic.
- Runtime mirrors are synchronized via `sync_examples_to_runtime.ps1`.

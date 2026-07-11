# Modding

This repository supports two modding paths.

## Path A ? Sylian Foundry Modloader (Data-Driven External Mods)

If you want content mods without editing C++ runtime code, use the modloader path.

> External mod contract baseline: `apiVersion: 4`

Start here:

1. `docs/SYLIAN_FOUNDRY_MODLOADER.md`
2. `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`
3. `docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md`
4. `docs/EXTERNAL_MOD_MANAGER_REFERENCE.md`
5. `docs/examples/external_mods/`

v6 reference suite:

- Frameworks: `ui_kit`, `container_kit`, `ai_templates`, `sss`
- Content demos: `demo_mini_inventory_h`, `demo_bag_inventory_ext`, `demo_furnace_system`,
  `sss_content_pack`, `demo_npc_patrol`, `demo_enemy_templates`, `demo_vanilla_adapters`

v7 world graphics suite:

- Frameworks: `lighting_kit`, `world_profiles`, `material_helpers`
- Content demos: `demo_room_profiles`, `demo_actor_lights`, `demo_green_fire_magic`
- Authoring guides: `docs/examples/external_mods/v7_reference_suite_docs/*`

v5.2 polish demos:

- `framework_no_items_demo`
- `unresolved_reference_demo`
- `live_settings_realtime_demo`
- `compat_wrapper_demo`

These cover flexible contracts, severity diagnostics, settings schema UI, and wrapper compatibility.

v9 persistence/narrative/dev suite:

- Frameworks: `persistence_kit`, `narrative_kit`, `devtools_kit`
- Content demos: `demo_persistent_tents`, `demo_loot_respawn`, `demo_dialogue_npc`,
  `demo_cutscene_intro`, `demo_quest_chain`, `demo_hot_reload_playground`
- Guides: `docs/PERSISTENCE_GUIDE.md`, `docs/NARRATIVE_GUIDE.md`, `docs/HOT_RELOAD_GUIDE.md`, `docs/COOKBOOK.md`

Operational references:

- Runtime exports: `docs/actions.json`, `docs/events.json`, `docs/catalogs.json`
- Demo sync: `tools/external_mods/sync_examples_to_runtime.ps1`
- Runtime export generation: `tools/external_mods/export_runtime_reference.ps1`
- Contract docs generation: `tools/external_mods/generate_contract_docs.ps1`
- Validation/lint: `tools/external_mods/validate_mod.ps1`
- Manifest/settings contracts: `docs/runtime_contract/mod.manifest.v2.json`
- v10 CLI tooling: `tools/external_mods/modtool.ps1`
- Generated indexes: `docs/generated/contract_index.json`, `docs/generated/registry_index.json`
- Framework wiki: `docs/framework_wiki/README.md`

## Path B ? Engine/C++ Modding

Use this path if you need to change core engine/gameplay code.

1. Follow `docs/BUILDING.md` for your platform setup.
2. Create a feature branch from `develop` (or your integration branch).
3. Implement and validate changes with local build + in-game testing.

### Minimal Git workflow

```bash
git checkout develop
git pull
git checkout -b feature/my-change
```

### Where to search first

- Gameplay actor logic: `soh/src/overlays/actors/`
- External mod systems: `soh/soh/Enhancements/external-mods/`
- UI/menu integrations: `soh/soh/Enhancements/`

## Notes

- Keep docs aligned with runtime behavior.
- For agent-governed workflows, see `docs/agents/AGENTS.md`.

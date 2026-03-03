# Sylian Foundry Modloader (API v4)

This is the canonical entry point for data-driven external mods in Ship of Harkinian / Hylian Foundry.

> Contract baseline: `apiVersion: 4`

## What it is

The **Sylian Foundry Modloader** loads external mod packages (folder/zip), validates capability-gated JSON contracts, mounts assets, and runs behavior/WASM logic with per-mod isolation.

## Start here (quick path)

1. Read this file first.
2. Pick a demo under `docs/examples/external_mods/`.
3. Confirm your `mod.json` uses `apiVersion: 4`.
4. Implement item/effect logic with catalogs + behaviors.
5. Test in runtime and inspect logs (`x64/Release/logs/Ship of Harkinian.log`).

## Runtime contract baseline

- Required for new mods: `mod.json.apiVersion = 4`
- Runtime type: `wasm3-v1`
- Namespaced IDs required (`modId:*`, with `core:*` reserved for built-ins)
- Capability-gated files: if capability is declared, required file/path must be valid
- Dependency graph is enforced (`dependencies[]` with semver ranges)

## Capability and file map

Use `docs/catalogs.json` as machine-readable source of truth. Common capabilities include:

- `hooks.extended.v1` -> `hookDefinitions` (`hooks/hooks.json`)
- `behaviors.graph.v1` -> `behaviorDefinitions` (`behaviors/behaviors.json`)
- `statuses.catalog.v1` -> `statusDefinitions` (`statuses/statuses.json`)
- `combat.damage.v1` -> `damageDefinitions` (`combat/damage_profiles.json`)
- `combat.targeting.v1` -> `targetingDefinitions` (`combat/targeting_profiles.json`)
- `combat.projectiles.v1` -> `projectileDefinitions` (`combat/projectiles.json`)
- `combat.aoe.v1` -> `aoeDefinitions` (`combat/aoe_profiles.json`)
- `movement.profiles.v1` -> `movementDefinitions` (`movement/movement_profiles.json`)
- `items.use_profiles.v1` -> `itemUseProfiles` (`items/use_profiles.json`)
- `patches.vanilla_items.v1` -> `vanillaItemPatches` (`patches/vanilla_items.patch.json`)
- `world.queries.v1` -> no file required (enables world query actions)
- `items.state_machine.v1` -> `itemStateDefinitions` (`items/item_states.json`)
- `render.equipped_models.v1` -> `equippedModelDefinitions` (`render/equipped_models.json`)
- `hud.widgets.v1` -> `hudWidgetDefinitions` (`ui/widgets.json`)
- `hud.reticles.v2` -> `hudReticleDefinitions` (`ui/reticles.json`)
- `camera.aim_profiles.v2` -> `cameraDefinitions` (`camera/camera_profiles.json`)
- `effects.graph.v2` -> `effectGraphDefinitions` (`effects/effect_graphs.json`)
- `combat.hit_rules.v2` -> `combatHitRuleDefinitions` (`combat/hit_rules.json`)
- `movement.surf.v2` -> `surfDefinitions` (`movement/surf_profiles.json`)
- `actors.tags.v1` -> `actorTagDefinitions` (`actors/actor_tags.json`)
- `world.patchsets.v1` -> `worldPatchDefinitions` (`world/patchsets.json`)
- `quests.graph.v1` -> `questDefinitions` (`quests/quests.json`)
- `dialog.nodes.v1` -> `dialogDefinitions` (`dialog/dialogs.json`)
- `sdk.generators.v1` -> `sdkGeneratorDefinitions` (`sdk/generators.json`)
- `fx.presets.v1` -> `fxPresetDefinitions` (`fx/fx_presets.json`)
- `states.catalog.v1` -> `stateDefinitions` (`states/states.json`)
- `spells.catalog.v1` -> `spellDefinitions` (`spells/spells.json`)

### v6 core extension capabilities

- `ui.runtime.v1` -> `uiScreenDefinitions` (`ui/screens.json`)
- `ui.hud.v1` -> `uiHudDefinitions` (`ui/hud_layouts.json`)
- `ui.inventory_ext.v1` -> `inventoryExtensionDefinitions` (`inventory_ext/pages.json`)
- `containers.v1` -> `containerDefinitions` (`containers/containers.json`)
- `recipes.processing.v1` -> `recipeDefinitions` (`recipes/processing_recipes.json`)
- `interactions.v1` -> `interactionDefinitions` (`interactions/interactions.json`)
- `actors.archetypes.v1` -> `actorArchetypeDefinitions` (`actors/archetypes.json`)
- `actors.adapters.v1` -> `actorAdapterDefinitions` (`actors/adapters.json`)
- `ai.behavior_trees.v1` -> `behaviorTreeDefinitions` (`ai/behavior_trees.json`)
- `ai.sensors.v1` -> `sensorDefinitions` (`ai/sensors.json`)
- `nav.routes.v1` -> `routeDefinitions` (`nav/routes.json`)
- `nav.navmesh_bridge.v1` -> `navBridgeDefinitions` (`nav/nav_bridge.json`)
- `debug.overlay.v1` -> `debugOverlayDefinitions` (`debug/overlays.json`)

### v7 world graphics extension capabilities

- `render.materials.v1` -> `materialDefinitions` (`materials/materials.json`)
- `render.pbr.v1` -> `pbrDefinitions` (`render/pbr_profiles.json`)
- `render.lighting.v1` -> `lightingDefinitions` (`lighting/light_profiles.json`)
- `render.postfx.v1` -> `postFxDefinitions` (`render/postfx_presets.json`)
- `world.scenes.v1` -> `sceneProfileDefinitions` (`world/scene_profiles.json`)
- `world.rooms.v1` -> `roomProfileDefinitions` (`world/room_profiles.json`)
- `assets.packs.v2` -> `assetPackDefinitions` (`assets/packs.json`)
- `debug.render_inspector.v1` -> `renderInspectorDefinitions` (`debug/render_inspector.json`)

## Runtime lifecycle

1. **Discover** packages in mods folders
2. **Validate** manifest + capability files
3. **Register** catalogs, items, hooks, behaviors
4. **Run** actions/effects/WASM with budgets
5. **Reload/Shutdown** with deterministic cleanup

### Freeze no-damage preset (data-driven)

For Ice Trap-like freeze without periodic damage, use:

- `core:freeze_ice_trap_no_damage`

Or define a custom status with `baseStatus: core:freeze` + `freezeProfile.mode: ice_trap_no_damage`.

### SSS framework examples

- `docs/examples/external_mods/sss`
- `docs/examples/external_mods/sss_content_pack`

### v6 reference suite (frameworks + demos)

Framework layer:
- `docs/examples/external_mods/ui_kit`
- `docs/examples/external_mods/container_kit`
- `docs/examples/external_mods/ai_templates`

Content layer:
- `demo_mini_inventory_h`
- `demo_bag_inventory_ext`
- `demo_furnace_system`
- `demo_firewall_staff`
- `demo_purple_din_lev_glove`
- `demo_npc_patrol`
- `demo_enemy_templates`
- `demo_vanilla_adapters`

These demonstrate a library-style mod (`com.sylian.sss`) plus a dependent content pack (`dependencies[]` enforced by loader).

### v7 world graphics reference suite (frameworks + demos)

Framework layer:
- `docs/examples/external_mods/lighting_kit`
- `docs/examples/external_mods/world_profiles`
- `docs/examples/external_mods/material_helpers`

Content layer:
- `docs/examples/external_mods/demo_room_profiles`
- `docs/examples/external_mods/demo_actor_lights`
- `docs/examples/external_mods/demo_green_fire_magic`

Authoring guides:
- `docs/examples/external_mods/v7_reference_suite_docs/PACK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/FRAMEWORK_AUTHOR_GUIDE.md`
- `docs/examples/external_mods/v7_reference_suite_docs/DEMO_PLAYBOOK.md`

## Safety and failure model

- Invalid mod data disables only the failing mod.
- Runtime budgets are enforced per mod.
- Logs include context (`modId`, file, JSON path when available).
- Reload clears mod-owned state to avoid orphan behavior.

## Where to extend

- Actions reference: `docs/actions.json`
- Events reference: `docs/events.json`
- Catalog/runtime capabilities: `docs/catalogs.json`
- Declarative action registry: `docs/runtime_contract/actions.registry.json`
- Declarative condition registry: `docs/runtime_contract/conditions.registry.json`

## Detailed references

- `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`
- `docs/EXTERNAL_MOD_MANAGER_REFERENCE.md`
- `docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md`
- `docs/examples/external_mods/`

## Notes for contributors

- Keep docs aligned with runtime exports (`tools/external_mods/export_runtime_reference.ps1`).
- Run drift checks before publishing docs updates (`check-doc-drift.ps1`).

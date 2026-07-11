# Plans Ledger

This file is the fixed, append-only plan ledger for this repository.

## Rules

1. Every new execution plan must be appended here before implementation starts.
2. Existing plan entries must not be overwritten or removed.
3. Progress changes must be appended as `[UPDATE]` entries.
4. Use `tools/agents/append-plan-ledger.ps1` for writes.
5. Use `tools/agents/query-plan-ledger.ps1` for reads.

## Entry Format

```
## [PLN-YYYYMMDD-####] Plan Title
- createdUtc: 2026-02-25T00:00:00Z
- status: planned|in_progress|blocked|done|canceled
- scope: engine|docs|mod|tooling|mixed
- summary: ...
- milestones:
  1. ...
  2. ...
- tags: a, b
- refs:
  - path
```

## Update Format

```
## [PLN-YYYYMMDD-####][UPDATE] 2026-02-25T00:00:00Z
- status: in_progress|blocked|done|canceled
- note: ...
- refs:
  - path
```

## [PLN-20260225-0001] Plan ledger governance rollout
- createdUtc: 2026-02-25T02:41:23Z
- status: in_progress
- scope: tooling
- summary: Create fixed Plans.md and enforce append-only planning before implementation.
- milestones:
  1. create Plans.md
  2. add plan skill
  3. sync global skills
  4. update AGENTS protocol
- tags: plans, agents
- refs:
  - docs/agents/Plans.md
  - docs/agents/AGENTS.md

## [PLN-20260225-0001][UPDATE] 2026-02-25T02:42:51Z
- status: done
- note: Implemented fixed Plans.md ledger, created soh-agents-plan-ledger skill, updated AGENTS policy, and synced 13 skills globally.
- refs:
  - docs/agents/Plans.md
  - docs/agents/skills/soh-agents-plan-ledger/SKILL.md
  - tools/agents/append-plan-ledger.ps1

## [PLN-20260225-0002] Phase1 gap closure: v3 cleanup and runtime references
- createdUtc: 2026-02-25T03:00:23Z
- status: done
- scope: mixed
- summary: Aligned docs/examples to apiVersion 3, removed legacy action usage from examples, generated docs/catalogs.json actions.json events.json, and kept parser params v3-only.
- milestones:
  1. Normalize legacy examples to apiVersion 3
  2. Replace removed legacy action usage with applyStatus
  3. Add machine-readable runtime reference exports
- refs:
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/examples/external_mods
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260225-0003] Gap closure batch: wasm host runtime + content registry extraction
- createdUtc: 2026-02-25T04:17:40Z
- status: in_progress
- scope: engine
- summary: Close immediate v3 blockers by stabilizing executable WASM host bridge, adding runtime budgets/telemetry, and extracting catalog lookup logic from ExternalModManager into a dedicated content registry.
- milestones:
  1. fix build/runtime blocker in wasm runtime and manager bridge
  2. extract Find*ById lookups into ExternalModContentRegistry
  3. refresh exported runtime reference JSON and docs
- tags: external-mods, api-v3, wasm3, modularization
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md

## [PLN-20260225-0003][UPDATE] 2026-02-25T04:17:49Z
- status: done
- note: Completed: release build passes; executable WASM host imports wired with budgets and per-frame metrics; lookup helpers moved to ExternalModContentRegistry; runtime reference export/docs updated.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.h
  - soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.h
  - soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp
  - docs/actions.json
  - docs/catalogs.json
  - docs/events.json

## [PLN-20260225-0004] Skill soh-git-checkpoint-merge rollout
- createdUtc: 2026-02-25T04:26:48Z
- status: in_progress
- scope: tooling
- summary: Create and wire a Git checkpoint/merge skill with automatic commit triggers after feature completion and successful builds, plus end-of-plan local merge workflow.
- milestones:
  1. create skill files and scripts
  2. update manifest/index and AGENTS policy
  3. sync/install/validate skills
- tags: skills, git, automation, plans
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/SKILL.md
  - docs/agents/AGENTS.md
  - docs/agents/skills.manifest.json
  - docs/agents/skills.index.json

## [PLN-20260225-0004][UPDATE] 2026-02-25T04:31:36Z
- status: done
- note: Implemented soh-git-checkpoint-merge skill, updated manifest/index/AGENTS, and installed+validated mirror/global skills.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/SKILL.md
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/git-audit.ps1
  - docs/agents/skills.manifest.json
  - docs/agents/skills.index.json
  - docs/agents/AGENTS.md

## [PLN-20260225-0004][UPDATE] 2026-02-25T04:42:26Z
- status: done
- note: Hardened soh-git-checkpoint-merge scripts: fixed StrictMode-safe git output handling and validated dry-run flows (git-audit, feature/build checkpoints).
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/git-audit.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1

## [PLN-20260225-0005] Aim OTS reticle visibility + LMB fire flow fix
- createdUtc: 2026-02-25T12:42:55Z
- status: in_progress
- scope: engine
- summary: Fix reticle visibility for selected slingshot/pistol and route LMB through vanilla item-button input path for Aim OTS v3.
- milestones:
  1. Add reticleVisibility enum+parser field,Resolve effective profile for mouse-fire by context,Inject LMB into sControlInput cur/press with edge detection,Implement selected/aim_only/button_hold reticle gating,Update aim_ots_toggle_demo camera profile and sync runtime,Build validation and log/memory updates
- tags: external-mods,aim-ots,input,reticle,api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0006] Aim OTS reticle visibility + LMB fire flow fix
- createdUtc: 2026-02-25T12:43:12Z
- status: in_progress
- scope: engine
- summary: Fix reticle visibility for selected slingshot/pistol and route LMB through vanilla item-button input path for Aim OTS v3.
- milestones:
  1. Add reticleVisibility enum+parser field,Resolve effective profile for mouse-fire by context,Inject LMB into sControlInput cur/press with edge detection,Implement selected/aim_only/button_hold reticle gating,Update aim_ots_toggle_demo camera profile and sync runtime,Build validation and log/memory updates
- tags: external-mods,aim-ots,input,reticle,api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0006][UPDATE] 2026-02-25T12:52:38Z
- status: done
- note: Implemented reticleVisibility parsing+runtime, moved LMB fire injection into sControlInput cur/press path, relaxed slingshot reticle draw gate, updated aim_ots_toggle_demo profile to selected, and validated Release build.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/src/code/z_player_lib.c,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0007] AGENTS governance refresh aligned to current docs/tools
- createdUtc: 2026-02-25T13:02:38Z
- status: in_progress
- scope: docs
- summary: Refresh docs/agents/AGENTS.md and governance artifacts to match current docs/tools state, enforce plan/memory/validation gates, and keep doc-drift as soft gate.
- milestones:
  1. rewrite AGENTS protocol sections and command matrix,update project_state snapshot with current risks and skill-pack count,append governance decision and drift backlog entries to memory log,run validation commands and close plan with update
- tags: agents,governance,docs,memory
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/Plans.md,docs/agents/memory.log

## [PLN-20260225-0007][UPDATE] 2026-02-25T13:04:30Z
- status: done
- note: Completed governance refresh: AGENTS rewritten with execution/read/memory gates, project_state updated for 14-skill baseline and soft-gate drift policy, memory decision+incident logged, and validation commands executed.
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/Plans.md,docs/agents/memory.log,docs/agents/memory.index.json

## [PLN-20260225-0008] Aim OTS fix: no pitch lock + selected reticle visibility
- createdUtc: 2026-02-25T13:11:51Z
- status: in_progress
- scope: engine
- summary: Fix OTS LMB aim pitch lock and ensure reticleVisibility=selected works for pistol/slingshot by correcting profile resolution and removing forced focus rotation.
- milestones:
  1. fix profile resolution fallback order for context,remove forced focus rotation in bow/slingshot ready-to-fire path,validate aim_ots demo profile settings,build release and log validation
- tags: external-mods,aim-ots,input,reticle
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/overlays/actors/ovl_player_actor/z_player.c,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0008][UPDATE] 2026-02-25T13:15:00Z
- status: done
- note: Implemented aim OTS fix: removed forced focus pitch/yaw sync in ready-to-fire, corrected context profile resolution ordering (active explicit -> best mod profile -> core fallback), kept selected reticle behavior, validated config presence and successful Release build.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/examples/external_mods/aim_ots_toggle_demo/camera/camera_profiles.json,x64/Release/mods/aim_ots_toggle_demo/camera/camera_profiles.json

## [PLN-20260225-0009] Aim OTS reticle visible without pistol in hand
- createdUtc: 2026-02-25T13:30:42Z
- status: in_progress
- scope: engine
- summary: Fix slingshot/pistol reticle selected-mode gating so reticle only draws when slingshot/pistol is actually in hand, avoiding always-on overlay while other items are active.
- milestones:
  1. isolate root cause in DrawAimReticleIfActive selected gating,patch selected visibility gate to require slingshot-in-hand context,build release validation and runtime sanity check guidance,append memory decision and close plan update
- tags: external-mods,aim-ots,reticle,ui
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/code/z_player_lib.c

## [PLN-20260225-0009][UPDATE] 2026-02-25T13:33:21Z
- status: done
- note: Fixed reticle leakage: DrawAimReticleIfActive now requires slingshot in hand for aim_only/button_hold/selected modes, so crosshair no longer appears while pistol/slingshot is not held; release build succeeded.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0010] freeze_dome_staff_demo (hammer impact freeze dome)
- createdUtc: 2026-02-25T14:20:14Z
- status: in_progress
- scope: mixed
- summary: Implement API v3 data-driven hammerGroundImpact trigger, spawnShockwave effect, AoE targetScope, and new freeze_dome_staff_demo docs/runtime sync.
- milestones:
  1. Add trigger/effect/aoe type+parser changes
  2. Wire hammer impact interop and runtime execution
  3. Create freeze_dome_staff_demo and sync docs/runtime
  4. Update docs refs and validate build/smoke
- tags: external-mods, api-v3, demo
- refs:
  - docs/agents/project_state.md
  - docs/examples/external_mods

## [PLN-20260225-0010][UPDATE] 2026-02-25T14:40:36Z
- status: done
- note: Implemented hammerGroundImpact trigger, spawnShockwave effect, AoE targetScope filtering, z_player hammer impact hook, and freeze_dome_staff_demo with docs/runtime sync; Release build passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/freeze_dome_staff_demo
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/MODDING.md
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260225-0011] Pistol select-to-aim + slot toggle + LMB/B fire
- createdUtc: 2026-02-25T15:48:21Z
- status: in_progress
- scope: engine
- summary: Implement PLANpistol.md by adding item opt-in fields, slot press toggle state, BTN_B fire gating, and pistol demo camera profile/runtime integration.
- milestones:
  1. Add item fields+parser
  2. Add aim select runtime+interop
  3. Patch Player_ProcessItemButtons slot toggle and LMB/B fire
  4. Update pistol demo docs/runtime and docs refs
  5. Build + smoke + close plan
- tags: external-mods, aim, pistol, input, api-v3
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json

## [PLN-20260225-0011][UPDATE] 2026-02-25T16:01:19Z
- status: done
- note: Implemented PLANpistol: added aimSelectToggle/aimAttackButtonFire parsing, slot-select aim toggle runtime+interop, BTN_B virtual fire routing in Player_ProcessItemButtons, reticle gating tied to active select state, pistol demo camera profile/capability updates, docs updates, demo sync and Release build.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/items/items.json
  - docs/examples/external_mods/pistol_hitscan_demo/camera/camera_profiles.json
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/MODDING.md

## [PLN-20260225-0012] Sylian Foundry modloader docs refresh and governance alignment
- createdUtc: 2026-02-25T16:13:46Z
- status: in_progress
- scope: docs
- summary: Create canonical Sylian Foundry Modloader doc, archive legacy MVP history, reorganize modding references for API v3, run drift/runtime reference validation, and close governance records.
- milestones:
  1. Create canonical SYLIAN_FOUNDRY_MODLOADER.md and archive legacy MVP history
  2. Rewrite MOD_SDK_MVP.md and MODDING.md to v3-aligned structure
  3. Refresh EXTERNAL_MOD_* references and README modloader navigation
  4. Run drift guard and runtime reference export validations
  5. Append plan update + memory decision and run git checkpoints
- tags: docs, modloader, api-v3, governance
- refs:
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/MOD_SDK_MVP.md
  - docs/MODDING.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md
  - README.md

## [PLN-20260225-0012][UPDATE] 2026-02-25T16:20:55Z
- status: done
- note: Completed full docs refresh for Sylian Foundry Modloader: canonical doc added, MVP history archived, references reorganized, README apiVersion note added, runtime refs exported, and drift guard now passes.
- refs:
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/archive/MOD_SDK_MVP_LEGACY_HISTORY.md
  - docs/MOD_SDK_MVP.md
  - docs/MODDING.md
  - docs/EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - README.md
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json
  - docs/agents/project_state.md

## [PLN-20260225-0012][UPDATE] 2026-02-25T16:22:26Z
- status: done
- note: Post-completion finalize-plan-merge gate attempted and blocked because working tree is not clean (dirty libultraship submodule). Docs implementation remains complete.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/memory.log

## [PLN-20260225-0013] Fix pistol C-down toggle exit and restore reticle/custom model visibility
- createdUtc: 2026-02-25T16:53:43Z
- status: in_progress
- scope: engine
- summary: Prioritize slot-toggle deactivation over virtual fire injection and remove granted-only gating for pistol aim-select reticle/custom model paths.
- milestones:
  1. Patch Player_ProcessItemButtons to process slot-toggle press before virtual fire injection and consume deactivation frame
  2. Patch ExternalModManager aim-select candidate availability to not depend only on granted
  3. Patch reticle/model selection to use availability helper and add targeted debug logs
  4. Build Release and validate pistol toggle/reticle/model behavior in-game
- tags: external-mods, aim-select, pistol, input, reticle, model, api-v3
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - x64/Release/mods/pistol_hitscan_demo/items/items.json
  - x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0013][UPDATE] 2026-02-25T17:15:04Z
- status: done
- note: Patched Player_ProcessItemButtons to prioritize C/D slot-toggle deactivation before virtual fire injection, removed granted-only aim availability gating, restored slingshot reticle/model candidate resolution via availability helper, and validated Release build.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - x64/Release/logs/Ship of Harkinian.log

## [PLN-20260225-0013][UPDATE] 2026-02-25T17:16:56Z
- status: done
- note: finalize-plan-merge gate attempted and blocked because working tree is not clean (dirty libultraship submodule). Implementation is complete and committed locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0014] Sylian Foundry Modloader v4 content-only master rollout
- createdUtc: 2026-02-25T17:44:20Z
- status: in_progress
- scope: mixed
- summary: Start implementing the v4 hard-break foundation for content-only mods: manifest v4 contract fields, expanded capability map, runtime contract registries, v4 reference exports, and migration scaffolding for examples/tools.
- milestones:
  1. Implement M0 governance baseline + frozen gap matrix artifacts
  2. Implement M1 kernel: apiVersion 4 hard-break and manifest v4 fields/capability wiring
  3. Implement M2 foundations: public action/condition registries and mod validator tool scaffold
  4. Implement M10 scaffolding: foundry CLI-style scripts (validate/migrate/sync) and migrate official examples to v4
  5. Run build + reference exports + docs sync + ledger/memory updates
- tags: external-mods, sylian-foundry, api-v4, content-only, tooling, docs
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - tools/external_mods/export_runtime_reference.ps1
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260225-0014][UPDATE] 2026-02-25T18:10:17Z
- status: in_progress
- note: Completed v4 foundation checkpoint: apiVersion hard-break + manifest v4 fields/capability wiring, validator+migrator+CLI scripts, runtime contract registries export, docs/runtime mod manifests migrated to v4, and Release build/validation passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,tools/external_mods/validate_mod.ps1,tools/external_mods/migrate_mods_v3_to_v4.ps1,tools/external_mods/foundry-cli.ps1,tools/external_mods/export_runtime_reference.ps1,docs/runtime_contract/actions.registry.json,docs/runtime_contract/conditions.registry.json,plan.md

## [PLN-20260225-0015] Freeze ice-trap no-damage data-driven + freeze dome demo uplift
- createdUtc: 2026-02-25T18:34:43Z
- status: in_progress
- scope: mixed
- summary: Implement freezeProfile + AoE player-inclusive scopes + freeze_dome_staff_demo updates + status runtime callbacks/stacking groundwork for higher data-driven freedom in API v4.
- milestones:
  1. Add freezeProfile and AoE target scope parser/type support
  2. Implement runtime freeze no-damage mode for player/enemy with ice shell lifecycle
  3. Update freeze_dome_staff_demo to use statusDefinitions and player_enemies_bosses scope
  4. Add status callbacks/stacking execution + persistent AoE ticking runtime state
  5. Update validator/migrator/docs/runtime references and run build/sync validations
- tags: external-mods, freeze, api-v4, data-driven
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/freeze_dome_staff_demo
  - tools/external_mods/validate_mod.ps1

## [PLN-20260225-0015][UPDATE] 2026-02-25T19:02:56Z
- status: done
- note: Implemented freezeProfile no-damage mode, new AoE scopes with player inclusion, persistent AoE ticking, status callback/stacking execution, freeze_dome_staff_demo status catalog uplift, validator/migrator/runtime reference updates, and validated Release build + sync/validate scripts.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/freeze_dome_staff_demo
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/migrate_mods_v3_to_v4.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json

## [PLN-20260225-0015][UPDATE] 2026-02-25T19:04:34Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked because working tree is not clean (dirty libultraship submodule). Implementation commit is complete locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0016] Fix camera capability mismatch for aim/pistol demos
- createdUtc: 2026-02-25T19:32:31Z
- status: in_progress
- scope: mod
- summary: Resolve runtime disable error requiring camera.aim_profiles.v2 by updating demo manifest capabilities and syncing runtime mods.
- milestones:
  1. Update aim_ots_toggle_demo and pistol_hitscan_demo capabilities to camera.aim_profiles.v2
  2. Sync docs examples to runtime mods
  3. Validate mod manifests and confirm no capability mismatch warnings
- tags: external-mods, camera, demos, api-v4
- refs:
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/mod.json
  - x64/Release/mods/pistol_hitscan_demo/mod.json

## [PLN-20260225-0016][UPDATE] 2026-02-25T19:34:08Z
- status: done
- note: Fixed camera capability contract mismatch by updating aim_ots_toggle_demo and pistol_hitscan_demo to camera.aim_profiles.v2 (docs+runtime) and migrated runtime aim_ots entryScript to apiVersion 4; validator now passes for both mods.
- refs:
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/mod.json
  - x64/Release/mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/scripts/init.json
  - tools/external_mods/migrate_mods_v3_to_v4.ps1
  - tools/external_mods/validate_mod.ps1

## [PLN-20260225-0016][UPDATE] 2026-02-25T19:34:53Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked because working tree is not clean (dirty libultraship submodule). Fix commit is complete locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0017] Freeze dome staff should not affect player
- createdUtc: 2026-02-25T19:36:05Z
- status: in_progress
- scope: mod
- summary: Adjust freeze_dome_staff_demo AoE scope to exclude player and sync runtime/demo messaging.
- milestones:
  1. Update demo AoE targetScope to enemies_bosses
  2. Update demo notification text to reflect enemy-only freeze
  3. Sync docs examples to runtime and validate mod
- tags: external-mods, freeze, demo, aoe
- refs:
  - docs/examples/external_mods/freeze_dome_staff_demo/combat/aoe_profiles.json
  - docs/examples/external_mods/freeze_dome_staff_demo/scripts/init.json
  - x64/Release/mods/freeze_dome_staff_demo/combat/aoe_profiles.json

## [PLN-20260225-0017][UPDATE] 2026-02-25T19:37:14Z
- status: done
- note: Updated freeze_dome_staff_demo to enemy-only freeze by switching AoE targetScope to enemies_bosses and refreshed notification text; synced runtime mirror and validated both docs/runtime mods.
- refs:
  - docs/examples/external_mods/freeze_dome_staff_demo/combat/aoe_profiles.json
  - docs/examples/external_mods/freeze_dome_staff_demo/scripts/init.json
  - x64/Release/mods/freeze_dome_staff_demo/combat/aoe_profiles.json
  - x64/Release/mods/freeze_dome_staff_demo/scripts/init.json
  - tools/external_mods/sync_examples_to_runtime.ps1
  - tools/external_mods/validate_mod.ps1

## [PLN-20260225-0017][UPDATE] 2026-02-25T19:37:49Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked because working tree is not clean (dirty libultraship submodule). Fix commit is complete locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260225-0018] Fix cameraDefinitions v4 capability validation and load path
- createdUtc: 2026-02-25T20:18:53Z
- status: in_progress
- scope: engine
- summary: Eliminate duplicate cameraDefinitions parse gates (v1+v2), enforce v4 camera.aim_profiles.v2-only contract, align runtime camera loading, and validate aim/pistol demos.
- milestones:
  1. Patch TryParseManifest cameraDefinitions rules for apiVersion 4
  2. Align camera catalog runtime load path to v4 v2-only behavior
  3. Update validator/docs references and re-export runtime refs
  4. Sync and validate aim_ots_toggle_demo and pistol_hitscan_demo; build and drift checks
- tags: external-mods, camera, api-v4, parser, runtime
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json

## [PLN-20260225-0018][UPDATE] 2026-02-25T20:24:01Z
- status: done
- note: Fixed cameraDefinitions v4 contract: removed duplicate v1/v2 parser gate, enforced v4 camera.aim_profiles.v2-only with explicit legacy error for v1, aligned runtime camera loading to v2, updated validator/export/sync tooling, synced demos, and validated build+drift+mod checks.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/examples/external_mods/aim_ots_toggle_demo/mod.json
  - docs/examples/external_mods/pistol_hitscan_demo/mod.json
  - x64/Release/mods/aim_ots_toggle_demo/mod.json
  - x64/Release/mods/pistol_hitscan_demo/mod.json
  - docs/catalogs.json
  - docs/actions.json
  - docs/events.json

## [PLN-20260225-0018][UPDATE] 2026-02-25T20:24:58Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked because working tree is not clean (dirty libultraship submodule). Implementation is complete and committed locally.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - libultraship
  - docs/agents/Plans.md

## [PLN-20260303-0001] Implement SSS framework and SSS content pack for API v4
- createdUtc: 2026-03-03T00:18:02Z
- status: in_progress
- scope: mixed
- summary: Deliver SSS library capabilities (fx presets, states, spells), dependency enforcement, and dependent sss_content_pack demos with docs/tooling/runtime reference updates.
- milestones:
  1. Add manifest/parser/runtime contracts for fx.presets.v1, states.catalog.v1, spells.catalog.v1
  2. Implement runtime actions/hooks for FX presets, states API, spells cast, status/state lifecycle events
  3. Implement dependency resolver with semver range checks and deterministic load order
  4. Create sss and sss_content_pack example mods and sync runtime mirrors
  5. Update validator/exported references/docs and run build + smoke validations
- tags: external-mods, api-v4, sss, content-pack, tooling, docs
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260303-0001][UPDATE] 2026-03-03T02:20:25Z
- status: done
- note: Completed M1-M10 for SSS rollout: v4 parser/runtime features compiled, tooling/export updated for fx/states/spells, added sss + sss_content_pack examples, synced runtime mirrors, validated mods, and rebuilt Release.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,tools/external_mods/validate_mod.ps1,tools/external_mods/export_runtime_reference.ps1,tools/external_mods/sync_examples_to_runtime.ps1,docs/examples/external_mods/sss/mod.json,docs/examples/external_mods/sss_content_pack/mod.json,docs/catalogs.json,docs/actions.json,docs/events.json

## [PLN-20260303-0001][UPDATE] 2026-03-03T02:24:44Z
- status: done
- note: Post-completion finalize-plan-merge attempted and blocked: working tree not clean due dirty libultraship submodule marker.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,libultraship,docs/agents/Plans.md

## [PLN-20260303-0002] Fix finalize-plan-merge Root parameter parsing
- createdUtc: 2026-03-03T02:35:58Z
- status: in_progress
- scope: tooling
- summary: Patch finalize-plan-merge.ps1 Resolve-TargetBranch boolean expression so finalize merge runs without duplicate Root parameter binding errors.
- milestones:
  1. Patch script boolean expression and verify finalize command,Run finalize-plan-merge for PLN-20260303-0001 after clean tree,Record updates in ledger and memory
- tags: agents,git,tooling
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,docs/agents/Plans.md

## [PLN-20260303-0001][UPDATE] 2026-03-03T02:38:04Z
- status: done
- note: Local merge completed: 'codex/base-before-external-mod-wat-reload' -> 'develop' at 794003077.
- refs:
  - branch:codex/base-before-external-mod-wat-reload
  - branch:develop
  - commit:794003077

## [PLN-20260303-0002][UPDATE] 2026-03-03T02:39:50Z
- status: done
- note: Fixed finalize-plan-merge Root binding bug, stashed dirty libultraship submodule workspace, and successfully finalized PLN-20260303-0001 into develop.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,docs/agents/Plans.md,docs/agents/memory.index.json

## [PLN-20260303-0002][UPDATE] 2026-03-03T02:43:52Z
- status: done
- note: Finalize merge not rerun for PLN-20260303-0002 because checkpoint commits are already on develop after PLN-20260303-0001 merge; no separate source branch remained.
- refs:
  - docs/agents/Plans.md,branch:develop,commit:df927fa19

## [PLN-20260303-0003] Governance v4 skills refresh + critical skills + memory/index hardening
- createdUtc: 2026-03-03T03:12:51Z
- status: in_progress
- scope: mixed
- summary: Execute phased update: migrate agent skills/governance to API v4 baseline, add plan_tracks v5/v6 and 3 critical doc/memory skills, and harden memory index consistency workflows.
- milestones:
  1. Phase A: v4 updates + plan_tracks structure,Phase B: add docs-index-navigator, contract-delta-summarizer, plan-memory-reconciler skills,Phase C: harden memory/index scripts and reconcile backlog,Validation + checkpoints + finalize
- tags: agents,skills,governance,memory,api-v4
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/skills.manifest.json,tools/agents/rebuild-index.ps1

## [PLN-20260303-0003][UPDATE] 2026-03-03T03:46:43Z
- status: done
- note: Completed phased governance package: added plan_tracks v5/v6, migrated priority skills to API v4 baseline, created 3 critical navigation/reconciliation skills, and hardened memory/index tooling with consistency checks.
- refs:
  - docs/agents/AGENTS.md,docs/agents/project_state.md,docs/agents/plan_tracks/README.md,docs/agents/skills.manifest.json,docs/agents/skills.index.json,tools/agents/rebuild-index.ps1,tools/agents/validate-memory.ps1,tools/agents/memory-hardening.ps1

## [PLN-20260303-0003][UPDATE] 2026-03-03T03:50:02Z
- status: done
- note: finalize-plan-merge was run and correctly stopped because source and target are both develop; checkpoint commit already landed directly on target branch.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,branch:develop,commit:fa1035a82

## [PLN-20260303-0004] Patch v6 core API + v6 reference suite rollout
- createdUtc: 2026-03-03T07:29:15Z
- status: in_progress
- scope: mixed
- summary: Execute strict patch-v5 gate then implement v6 contract/runtime scaffolding (UI/container/actors/AI/nav/debug) and ship v6 framework/content reference suite under docs/examples/external_mods.
- milestones:
  1. Phase0 gate patch v5 and produce report,Phase1 add v6 capabilities/manifest fields/actions/events/docs exports,Phase2-6 add runtime modules and guard rails,Phase7-8 ship v6 framework and demo suites + sync,Phase9-10 tooling/docs/export/build/smoke and finalize
- tags: external-mods,api-v4,v6,reference-suite,frameworks
- refs:
  - docs/agents/plan_tracks/v6/index.md,docs/agents/plan_tracks/v6/roadmap.md,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/examples/external_mods

## [PLN-20260303-0004][UPDATE] 2026-03-03T08:55:47Z
- status: done
- note: Completed patch-v5 gate + v6 core contract/runtime scaffolding, exported references, added v6 framework/content reference suite, synced demos, and validated build/validate_mod/doc-drift.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,tools/external_mods/export_runtime_reference.ps1,tools/external_mods/validate_mod.ps1,tools/external_mods/sync_examples_to_runtime.ps1,docs/examples/external_mods/ui_kit/mod.json,docs/examples/external_mods/container_kit/mod.json,docs/examples/external_mods/ai_templates/mod.json,docs/agents/plan_tracks/v5/milestones/patch_v5_gate_report.md,docs/agents/plan_tracks/v6/milestones/patch_v6_core_api.md,docs/agents/plan_tracks/v6/milestones/patch_v6_reference_suite.md

## [PLN-20260303-0004][UPDATE] 2026-03-03T09:03:40Z
- status: done
- note: finalize-plan-merge attempted after clean build/validation and blocked because source and target are both develop; no merge action required.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,branch:develop

## [PLN-20260303-0005] Patch v5.1 closure for SSS status visuals and player/fx runtime
- createdUtc: 2026-03-03T09:30:24Z
- status: in_progress
- scope: engine
- summary: Close remaining v5 implementation gaps: execute status visuals lifecycle (start/loop/end), apply player boost/damage response controls, and honor fx.spawnActorFx overlayName mapping.
- milestones:
  1. Implement status visuals runtime hooks and deferred actor-context execution,Apply player.setBoostType/player.setDamageResponse in OnPlayerUpdate,Support overlay-name actor resolution in fx.spawnActorFx runtime,Validate build + validate_mod + drift and append plan update
- tags: external-mods,api-v4,sss,v5.1
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,docs/agents/Plans.md

## [PLN-20260303-0005][UPDATE] 2026-03-03T09:39:06Z
- status: done
- note: Closed v5.1 gaps by executing status visuals lifecycle presets (start/loop/end), binding deferred status callbacks to target actor FX context, applying player boost/damage response actions at runtime, and allowing fx.spawnActorFx overlay-only resolution.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/agents/Plans.md,build:x64/Release/soh.exe

## [PLN-20260303-0005][UPDATE] 2026-03-03T09:41:15Z
- status: done
- note: finalize-plan-merge executed and stopped with source==target on develop; checkpoints already committed directly on target branch.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,branch:develop

## [PLN-20260303-0006] Patch v7 world graphics core + reference suite rollout
- createdUtc: 2026-03-03T10:08:33Z
- status: in_progress
- scope: mixed
- summary: Implement v7 capability scaffolding for world/render contracts, parser/runtime action hooks, and deliver v7 framework/content reference suite with docs/tooling sync.
- milestones:
  1. Phase0: establish v7 governance track and prerequisite gate note
  2. Phase1-3: add v7 capabilities, manifest fields, parsers, actions, hooks, runtime blackboard scaffolding
  3. Phase4: ship v7 reference frameworks, packs metadata stubs, and demos under docs/examples
  4. Phase5: update tooling exports/validation/sync lists and run build+validation checks
  5. Phase6: checkpoint commits, plan update, memory append, and finalize merge attempt
- tags: external-mods, api-v4, v7, world-graphics, reference-suite
- refs:
  - docs/agents/plan_tracks/v7
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods

## [PLN-20260303-0006][UPDATE] 2026-03-03T10:48:27Z
- status: done
- note: Completed v7 scaffolding: capabilities/actions/hooks runtime plumbing, v7 track docs, v7 framework/demo suite, exports/sync/build/validation.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/agents/plan_tracks/v7/milestones/patch_v7_world_graphics_core.md
  - docs/examples/external_mods/lighting_kit/mod.json
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260303-0006][UPDATE] 2026-03-03T10:50:28Z
- status: done
- note: finalize-plan-merge was run and correctly blocked because source and target are both develop; checkpoints already landed directly on target branch.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1
  - branch:develop

## [PLN-20260303-0007] Runtime grafico real v7.1 (PBR + world graphics aplicado em jogo)
- createdUtc: 2026-03-03T11:39:41Z
- status: in_progress
- scope: mixed
- summary: Transformar scaffolding v7 em runtime visual real: aplicar scene/room/postfx/skylight/lights/material overrides em frame, habilitar path PBR GL+DX11 com fallback Metal, alinhar demos/tooling/docs.
- milestones:
  1. Modulo dedicado de world graphics runtime com resolucao cross-mod
  2. Aplicacao real em draw hooks (envCtx/lightCtx) + lifecycle de luzes
  3. Bridge PBR GL+DX11 + fallback Metal + debug inspector
  4. Atualizar demos v7, export/validate/drift/build
- tags: external-mods, v7.1, render, pbr, world-graphics
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - libultraship/src/fast/backends/gfx_opengl.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/examples/external_mods

## [PLN-20260303-0007][UPDATE] 2026-03-03T12:59:26Z
- status: in_progress
- note: Advanced v7.1 runtime graphics from scaffolding to real draw-time execution: added ExternalModWorldGraphicsRuntime module, draw begin/end hooks, deterministic scene/room/postfx/skylight resolution, dynamic light lifecycle (attach/follow/world), parser/action/hook extensions, and validated Release build + export + demo mod validation + drift check.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/agents/plan_tracks/v7/milestones/patch_v7_runtime_graphics_real.md
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260303-0008] Patch v8 core+suite + external mods tabs + furnace hotkey
- createdUtc: 2026-03-03T14:23:32Z
- status: in_progress
- scope: mixed
- summary: Deliver v8 contracts/runtime foundations and builder reference suite, plus 3-tab External Mods UI separation and direct-key furnace debug spawn toggle on key 7.
- milestones:
  1. Add v8 capabilities/manifest fields + direct hotkeys parser/runtime + actors.toggleArchetype
  2. Deliver furnace demo hotkey 7 toggle spawn/despawn and sync docs/runtime
  3. Implement External Mods 3-tab UI with resourcepacks/mods/core-api classification + pack controls
  4. Add v8 plan_tracks docs/milestones + update tooling/docs references + validate build
- tags: external-mods, api-v4, v8, ui, hotkeys
- refs:
  - docs/agents/plan_tracks/v8
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - docs/examples/external_mods/demo_furnace_system

## [PLN-20260303-0008][UPDATE] 2026-03-03T15:02:05Z
- status: in_progress
- note: Implemented v8 foundation slice: direct hotkeys runtime + actors.toggleArchetype with player_forward/world spawn override, External Mods 3-tab UI (Resourcepacks/Mods/Core+API) with classification badges, and demo_furnace_system key 7 toggle archetype spawn/despawn. Also added v8 plan track files, synced demos, regenerated runtime references, and passed Release build + mod validation.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/examples/external_mods/demo_furnace_system/config/input.json,docs/agents/plan_tracks/v8/index.md,docs/actions.json

## [PLN-20260303-0009] v6/v7 closure + firewall/levitation fixes
- createdUtc: 2026-03-03T15:30:58Z
- status: in_progress
- scope: mixed
- summary: Implement manual External Mods UI categorization, fix SSS content pack firewall and levitation behavior, remove legacy empty demos, and close v6/v7 remaining runtime checkpoints with validation.
- milestones:
  1. Phase A: UI category + SSS gameplay hotfixes + demo cleanup,Phase B: v6 runtime module/parity closure checklist updates + targeted code gaps,Phase C: v7 renderer/runtime closure tasks + validation/export
- tags: external-mods,api-v4,v6,v7,sss,ui
- refs:
  - docs/agents/plan_tracks/v6,docs/agents/plan_tracks/v7,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/examples/external_mods/sss_content_pack

## [PLN-20260303-0009][UPDATE] 2026-03-03T15:58:44Z
- status: in_progress
- note: Phase A hotfix delivered: External Mods tabs now use manual manifest uiCategory (core_api|mod), all example manifests tagged, legacy demo_firewall_staff/demo_purple_din_lev_glove removed, sss_content_pack Firewall switched to onUse cast path, levitationProfile lift_suspend parser/runtime added and wired in sss/sss_content_pack statuses, useProfile->spell->aoe debug trace added, docs/runtime refs refreshed, demo sync+validation+Release build passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/examples/external_mods/sss_content_pack/items/items.json,docs/examples/external_mods/sss_content_pack/statuses/statuses.json,docs/examples/external_mods/sss/statuses/statuses.json,tools/external_mods/sync_examples_to_runtime.ps1,docs/actions.json,docs/events.json,docs/catalogs.json

## [PLN-20260303-0009][UPDATE] 2026-03-03T16:51:56Z
- status: in_progress
- note: Starting Phase C fog fidelity patch: add data-driven postfx fog overlay controls to tint non-fog draw passes (sky/sprite-style textures), wire runtime apply/restore in OnPlayDrawBegin/End without breaking existing fill-screen effects, and calibrate world_profiles presets for stable green cinematic behavior.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,docs/examples/external_mods/world_profiles/render/postfx_presets.json

## [PLN-20260303-0009][UPDATE] 2026-03-03T17:07:27Z
- status: in_progress
- note: Implemented postfx fog overlay extension (forceFogOverlay/fogOverlayStrength), wired world graphics runtime fill-screen fallback for non-fog passes, and updated world_profiles green cinematic preset. Synced demos, built Release, validated world_profiles mod, and checked doc/runtime drift.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - docs/examples/external_mods/world_profiles/render/postfx_presets.json

## [PLN-20260303-0009][UPDATE] 2026-03-03T17:12:33Z
- status: in_progress
- note: Adjusted world_profiles overworld defaults back to vanilla-like low fog (overworld_clear tuned and reassigned in scene/room profiles), preserving green cinematic preset for optional use. Synced examples to runtime mods.
- refs:
  - docs/examples/external_mods/world_profiles/render/postfx_presets.json
  - docs/examples/external_mods/world_profiles/world/scene_profiles.json
  - docs/examples/external_mods/world_profiles/world/room_profiles.json
  - x64/Release/mods/world_profiles/render/postfx_presets.json

## [PLN-20260303-0009][UPDATE] 2026-03-03T17:26:18Z
- status: in_progress
- note: Implemented depth-aware fog mode for postfx presets: added forceDepthAwareFog parsing/runtime, wired world draw begin/end to toggle renderer fog forcing, and patched Fast3D interpreter to compute/apply fog even for non-fog display lists while world pass is active. Tuned green cinematic preset to use depth-aware fog with reduced overlay strength.
- refs:
  - libultraship/include/fast/interpreter.h
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - docs/examples/external_mods/world_profiles/render/postfx_presets.json

## [PLN-20260303-0010] Ambient Occlusion real SSAO (GL+DX11 with Metal fallback)
- createdUtc: 2026-03-03T18:09:36Z
- status: in_progress
- scope: engine
- summary: Implement runtime SSAO with quality tiers, pbr_profiles AO fields, CVars+UI overrides, and backend fallback observability without breaking fog/postfx world graphics.
- milestones:
  1. Add AO schema/types/parser+resolved state and C bridge,Implement SSAO pass in OpenGL and DX11 with quality modes,Add Metal fallback + fallback events/logs + UI CVars,Update docs/runtime references and validate release build
- tags: external-mods,render,ao,ssao,v7
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,libultraship/include/fast/interpreter.h,libultraship/src/fast/interpreter.cpp,libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/src/fast/backends/gfx_opengl.cpp,libultraship/src/fast/backends/gfx_direct3d11.cpp,libultraship/src/fast/backends/gfx_metal.cpp,soh/soh/SohGui/SohMenuSettings.cpp,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260303-0010][UPDATE] 2026-03-03T19:02:09Z
- status: done
- note: Implemented SSAO runtime patch: AO schema/parser in pbr profiles, GL+DX11 AO passes with blur/composite, Metal fallback telemetry, AO CVars+graphics menu, world graphics AO resolve bridge, docs/export/sync validation, and Release build success.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/include/fast/backends/gfx_rendering_api.h
  - libultraship/src/fast/backends/gfx_opengl.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/backends/gfx_metal.cpp
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/SohGui/SohMenuSettings.cpp
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260303-0011] SSAO crash hotfix triage and guard
- createdUtc: 2026-03-03T19:12:17Z
- status: in_progress
- scope: engine
- summary: Investigate crash when enabling AO/SSAO from graphics menu, identify failing backend path, and apply runtime guard/fallback so enabling AO never crashes.
- milestones:
  1. Collect crash evidence from log/dump and map first failing frame
  2. Patch AO runtime/backends with null/resource guards and fallback path
  3. Build Release and run mod/doc validation checks
- tags: render, ao, crash, hotfix
- refs:
  - x64/Release/logs/Ship of Harkinian.log
  - libultraship/src/fast/backends/gfx_opengl.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260303-0011][UPDATE] 2026-03-03T19:17:35Z
- status: done
- note: Triage found DX11 crash in SSAO setup path (EnsureAmbientOcclusionShaders/CreateBuffer). Applied non-throw AO resource creation guards with fallback code and fixed AO constant-buffer alignment (16-byte ByteWidth). Release build now succeeds.
- refs:
  - x64/Release/logs/Ship of Harkinian.log
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260303-0012] Hotkeys MOD_ACTION8/9 + placeholders cube for firewall/furnace
- createdUtc: 2026-03-03T19:29:25Z
- status: in_progress
- scope: mod
- summary: Add config-driven bindings for furnace and new inventory on MOD_ACTION8/9, and make firewall staff/furnace spawn visible placeholder cube actors for testing.
- milestones:
  1. Patch demo input/config bindings for MOD_ACTION8 and MOD_ACTION9
  2. Patch content demos so firewall and furnace spawn clear placeholder cube actors
  3. Sync examples to runtime and validate build/mod load
- tags: external-mods, demos, input, hotkeys, firewall, furnace
- refs:
  - docs/examples/external_mods/sss_content_pack
  - docs/examples/external_mods/demo_furnace_system
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260303-0012][UPDATE] 2026-03-03T20:35:27Z
- status: done
- note: Implemented MOD_ACTION8/9 aliases + UI labels, wired demo_furnace_system bindings and onInput actions, added actors.toggleArchetype onSpawn/onDestroy behavior dispatch, added cube overlay alias for placeholder spawns, patched sss_content_pack firewall to spawn visible cube placeholder, synced demos, validated mods, and built Release successfully.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/controls/SohInputEditorWindow.cpp
  - soh/soh/SohGui/UIWidgets.cpp
  - soh/soh/OTRGlobals.h
  - docs/examples/external_mods/demo_furnace_system/config/input.json
  - docs/examples/external_mods/demo_furnace_system/scripts/init.json
  - docs/examples/external_mods/demo_furnace_system/behaviors/behaviors.json
  - docs/examples/external_mods/sss_content_pack/items/use_profiles.json
  - docs/examples/external_mods/sss_content_pack/spells/spells.json

## [PLN-20260303-0012][UPDATE] 2026-03-03T21:14:02Z
- status: done
- note: Hotfix after field test: remapped MOD_ACTION8/9 to dedicated custom action bits (ocarina pitch up/down) to avoid ResetHotKey conflict with Modifier2, added ExecuteUseProfileEffects support for fx.spawnActorFx/fx.stopFx so firewall spell can spawn visible actor placeholders, and versioned furnace binding ids to *_v2 to bypass stale CVar masks from previous mappings. Synced demos, validated target mods, and rebuilt Release successfully.
- refs:
  - soh/soh/OTRGlobals.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/controls/SohInputEditorWindow.cpp
  - docs/examples/external_mods/demo_furnace_system/config/input.json
  - docs/examples/external_mods/demo_furnace_system/scripts/init.json
  - docs/examples/external_mods/sss_content_pack/items/use_profiles.json
  - docs/examples/external_mods/sss_content_pack/spells/spells.json

## [PLN-20260303-0012][UPDATE] 2026-03-03T21:21:07Z
- status: done
- note: Follow-up hotfix for field behavior: MOD_ACTION8/9 remapped away from modifier bits to dedicated custom action bits (BTN_CUSTOM_OCARINA_PITCH_UP/DOWN), default keyboard mapping now removes legacy key->modifier mappings for action8/action9 keys, and demo furnace bindings were versioned to *_v2 ids to bypass stale per-binding CVar remaps. Build+validate passed.
- refs:
  - soh/soh/OTRGlobals.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods/demo_furnace_system/config/input.json
  - docs/examples/external_mods/demo_furnace_system/scripts/init.json

## [PLN-20260304-0001] Patch v9 core persistence/narrative/dev + reference suite rollout
- createdUtc: 2026-03-04T00:27:02Z
- status: in_progress
- scope: mixed
- summary: Implement v9 capability-gated contracts and runtime scaffolding for persistence, narrative, and hot reload/wasm-sandbox-v2, then ship v9 framework+demos and sync docs/tooling exports.
- milestones:
  1. Phase 0 governance+track v9 and prerequisite gate report
  2. Phase 1 contract parser/types/capabilities/manifest fields for v9
  3. Phase 2-4 persistence runtime kernel + storage domains + spawn/time/weather actions/events
  4. Phase 5-7 narrative runtime + dev hot reload/console + wasm sandbox v2 budgets/permissions hooks
  5. Phase 8-9 v9 framework+demos under docs/examples with dependencies and runtime sync
  6. Phase 10-11 tooling/docs/export/build/validation and plan closure
- tags: external-mods, api-v4, v9, persistence, narrative, devtools
- refs:
  - docs/agents/plan_tracks/v9
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods

## [PLN-20260304-0001][UPDATE] 2026-03-04T02:38:59Z
- status: in_progress
- note: Implemented v9 core slice: persistence domain file-path support, lifecycle save/load flushing, room auto-save/load, timeline ticking, auto spawn-profile tick, hot-reload request processing, sandbox budget/permission hooks, wasm sandbox budget wiring, plus v9 framework+demos, guides, sync/export/validate.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/export_runtime_reference.ps1
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/examples/external_mods/persistence_kit
  - docs/examples/external_mods/narrative_kit
  - docs/examples/external_mods/devtools_kit
  - docs/examples/external_mods/demo_persistent_tents
  - docs/examples/external_mods/demo_loot_respawn
  - docs/examples/external_mods/demo_dialogue_npc
  - docs/examples/external_mods/demo_cutscene_intro
  - docs/examples/external_mods/demo_quest_chain
  - docs/examples/external_mods/demo_hot_reload_playground
  - docs/PERSISTENCE_GUIDE.md
  - docs/NARRATIVE_GUIDE.md
  - docs/HOT_RELOAD_GUIDE.md
  - docs/COOKBOOK.md

## [PLN-20260304-0001][UPDATE] 2026-03-04T02:59:47Z
- status: in_progress
- note: Hotfix for v9 suite load failures: added required itemDefinitions/inputDefinitions + empty items/config files to all v9 frameworks/demos, synced docs->runtime; updated sync_examples list for v9 packs; reran export/validate to confirm mods no longer fail for missing itemDefinitions.
- refs:
  - docs/examples/external_mods/persistence_kit/mod.json
  - docs/examples/external_mods/narrative_kit/mod.json
  - docs/examples/external_mods/devtools_kit/mod.json
  - docs/examples/external_mods/demo_persistent_tents/mod.json
  - docs/examples/external_mods/demo_loot_respawn/mod.json
  - docs/examples/external_mods/demo_dialogue_npc/mod.json
  - docs/examples/external_mods/demo_cutscene_intro/mod.json
  - docs/examples/external_mods/demo_quest_chain/mod.json
  - docs/examples/external_mods/demo_hot_reload_playground/mod.json
  - tools/external_mods/sync_examples_to_runtime.ps1
  - tools/external_mods/validate_mod.ps1

## [PLN-20260304-0002] Patch v10 ecosystem & tooling foundation
- createdUtc: 2026-03-04T03:29:59Z
- status: in_progress
- scope: mixed
- summary: Implement v10 foundation slice: modtool CLI, v2 packaging/lockfile schema docs, permissions contract scaffolding, deterministic pack pipeline, docs/wiki/templates, and contract export hooks while keeping modloader thin.
- milestones:
  1. register v10 track and milestones
  2. add modtool CLI with init/validate/pack/doctor/diff/report skeleton
  3. add v10 contracts/docs for packaging/permissions/tool reports
  4. add starter templates for framework/content/pack
  5. wire validation+export scripts and run smoke checks
- tags: external-mods, api-v4, v10, tooling, docs
- refs:
  - docs/agents/plan_tracks/v10/index.md
  - tools/external_mods
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T04:16:20Z
- status: in_progress
- note: Implemented v10 tooling foundation slice: added modtool CLI (init/validate/pack/sign/doctor/diff/report), added templates (framework/content/pack), created v10 track docs + milestone, added runtime contract schema docs for packaging/security/tool reports, updated foundry-cli bridge and validation checks for type/releaseChannels/capability rationales.
- refs:
  - tools/external_mods/modtool.ps1
  - tools/external_mods/templates
  - docs/runtime_contract/mod.manifest.v2.json
  - docs/runtime_contract/security.capabilities.v2.json
  - docs/framework_wiki/README.md
  - docs/agents/plan_tracks/v10/index.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T10:13:59Z
- status: in_progress
- note: Implemented v10 security/runtime slice: manifest fields (type/releaseChannels/capabilityRationales), stable-channel UI gate, per-mod permission store under Save/external_mods/permissions, action-level permission enforcement for persist.* and dev.reload/dev.console, plus modtool doctor conflict explainability/load-order report.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - tools/external_mods/modtool.ps1
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T10:24:18Z
- status: in_progress
- note: Extended v10 tooling/runtime hardening: action permission gates now active (persist.*=>filesystem, dev.reload*/dev.console.exec=>process), releaseChannels stable-only UI gate enforced at manifest discovery, per-mod permission grant store normalized to security.permissions.v1 with profileId, and modtool doctor now emits deterministic load-order plus registry/pack conflict explainers with legacy GetRelativePath fallback compatibility.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/modtool.ps1
  - docs/runtime_contract/security.permissions.v1.json
  - docs/examples/external_mods/persistence_kit/mod.json
  - docs/examples/external_mods/demo_hot_reload_playground/mod.json

## [PLN-20260304-0002][UPDATE] 2026-03-04T10:44:09Z
- status: in_progress
- note: Continuing v10 with M4+M5: add in-game permission prompt/grant UI (per-mod capabilities with rationale) and add deterministic contract docs generator/index pipeline integrated with modtool/foundry-cli.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/modtool.ps1
  - tools/external_mods/foundry-cli.ps1
  - docs/generated

## [PLN-20260304-0002][UPDATE] 2026-03-04T11:06:51Z
- status: in_progress
- note: Completed v10 M4+M5 slice: added External Mods permission toggles with persisted grants (security.permissions.v1), exposed manager APIs for permission get/set, added generate_contract_docs pipeline and wired modtool report + export-runtime-reference + foundry-cli command. Release build and tooling validation passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - tools/external_mods/generate_contract_docs.ps1
  - tools/external_mods/modtool.ps1
  - tools/external_mods/foundry-cli.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - docs/generated/contract_index.json
  - docs/runtime_contract/docs.contract_index.v1.json

## [PLN-20260304-0002][UPDATE] 2026-03-04T11:52:17Z
- status: in_progress
- note: Continued v10 M4+M5 hardening: manifest permissionRationales parse+validation, permission rationale UI render, and release-channel pack primitives in modtool/foundry-cli with validation/build/sync checks passing.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,tools/external_mods/modtool.ps1,tools/external_mods/foundry-cli.ps1,docs/SYLIAN_FOUNDRY_MODLOADER.md,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T12:04:14Z
- status: in_progress
- note: Hotfixed modtool report propagation bug: switched report parameter typing to IDictionary so Add-Error/Add-Warn mutations persist; beta channel pack now correctly fails when releaseChannels omits beta.
- refs:
  - tools/external_mods/modtool.ps1,docs/agents/Plans.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T12:52:48Z
- status: in_progress
- note: Starting next v10 block: add missing permissionRationales in remaining example frameworks/demos (persistence/devtools/hot-reload) to clear modtool report governance warnings.
- refs:
  - docs/examples/external_mods/persistence_kit/mod.json,docs/examples/external_mods/devtools_kit/mod.json,docs/examples/external_mods/demo_persistent_tents/mod.json,docs/examples/external_mods/demo_hot_reload_playground/mod.json

## [PLN-20260304-0002][UPDATE] 2026-03-04T12:56:00Z
- status: in_progress
- note: Completed next block: added permissionRationales to persistence/devtools kits and hot-reload/persistent-tents demos; synced runtime mirror and cleared modtool report warnings.
- refs:
  - docs/examples/external_mods/persistence_kit/mod.json,docs/examples/external_mods/devtools_kit/mod.json,docs/examples/external_mods/demo_persistent_tents/mod.json,docs/examples/external_mods/demo_hot_reload_playground/mod.json,tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260304-0002][UPDATE] 2026-03-04T13:36:46Z
- status: in_progress
- note: Completed next v10 UX hardening slice: External Mods permissions UI now has bulk presets (Allow Safe, Deny Risky, Allow All, Reset Defaults) with persisted per-mod grants and notification feedback.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/agents/Plans.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T14:19:20Z
- status: in_progress
- note: Starting next v10 block: implement runtime permission prompt notifications for risky denied grants so users get guided to External Mods UI without silent feature failures.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModManager.h,soh/soh/Enhancements/external-mods/ExternalModUi.cpp

## [PLN-20260304-0002][UPDATE] 2026-03-04T14:26:16Z
- status: in_progress
- note: Completed next v10 block: runtime now emits startup guidance notifications when risky permissions are denied, and docs updated for permission bulk presets + guidance behavior.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/SYLIAN_FOUNDRY_MODLOADER.md,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:03:16Z
- status: in_progress
- note: Completed v10 permission prompt block: added modal prompt for denied risky permissions (grant/keep denied/remind later) with per-mod Seen CVar and persisted grant updates.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/agents/Plans.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:28:50Z
- status: in_progress
- note: Completed v10 M8 hardening block: added runtime failure quarantine store (Save/external_mods/quarantine), manifest discovery gating for quarantined mods, External Mods UI Clear Quarantine action, and risky-permission modal prompt flow (grant/keep/remind) with persisted Seen CVar state.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModManager.h,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/SYLIAN_FOUNDRY_MODLOADER.md,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:33:23Z
- status: done
- note: Closed v10 remaining blocks: runtime quarantine + clear action + risky-permission modal prompt finished, build/validate/doctor/report/drift checks all green. v10 tooling/security hardening milestones M1-M8 now implemented for this patch scope.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModManager.h,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,tools/external_mods/modtool.ps1,docs/SYLIAN_FOUNDRY_MODLOADER.md,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:37:31Z
- status: in_progress
- note: Starting deterministic-pack hotfix: modtool pack currently rewrites manifest.lock.json with wall-clock generatedUtc, causing artifact hash drift between identical runs; patching lockfile timestamp policy to deterministic fixed value.
- refs:
  - tools/external_mods/modtool.ps1,docs/agents/Plans.md

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:40:04Z
- status: done
- note: Determinism hotfix complete: modtool pack now writes manifest.lock.json with deterministic fixed timestamp, eliminating artifact hash drift across identical runs; verified with two stable packs producing identical SHA256.
- refs:
  - tools/external_mods/modtool.ps1,docs/examples/external_mods/persistence_kit/manifest.lock.json

## [PLN-20260304-0002][UPDATE] 2026-03-04T15:42:34Z
- status: done
- note: Finalize-plan-merge dry-run remains blocked because working tree is not clean (expected during active local patch stack); feature/build checkpoint scripts were executed in dry-run and validation commands passed.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1,docs/agents/Plans.md

## [PLN-20260305-0001] Patch v5.2 Kokiri Polish closure
- createdUtc: 2026-03-05T00:03:39Z
- status: in_progress
- scope: mixed
- summary: Implement flexible contracts, severity diagnostics, settings kernel/schema, and framework compatibility polish so missing unused files no longer disable mods.
- milestones:
  1. Manifest/type additions (provides/uses/files.optional/settings)
  2. Registry contracts map + generator + shared consumption
  3. Reference scan + severity matrix in loader (disable only on fatal)
  4. Validator structured severity output and issue codes
  5. Mod Menu health/details/copy diagnostics
  6. Settings kernel/actions/events + schema UI auto-gen
  7. Framework wrapper compatibility + four v5.2 demos
  8. Sync/export/build/validation/drift and ledger closure
- tags: external-mods, v5.2, loader, validator, settings, ui, compat
- refs:
  - docs/agents/plan_tracks/v5/milestones/patch_v52_kokiri_polish.md
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260305-0001][UPDATE] 2026-03-08T16:48:27Z
- status: in_progress
- note: Implemented v5.2 core continuation: manifest fields (provides/uses/files.optional/settings), optional item/input loading, settings kernel/actions/schema parser, runtime issue summaries, UI diagnostics health, Release build pass, and validator no longer warns on missing itemDefinitions/inputDefinitions.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - tools/external_mods/validate_mod.ps1
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json
  - docs/runtime_contract/actions.registry.json

## [PLN-20260305-0001][UPDATE] 2026-03-08T17:28:02Z
- status: in_progress
- note: Continued v5.2: added settings schema UI auto-generation in External Mods details, rewrote validate_mod.ps1 to emit structured severity/issues JSON with fatal-only failure, added framework_no_items_demo and live_settings_realtime_demo, and synced examples. Validation passed; build now reaches link and is blocked by environment LNK1104 on soh.iobj.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/examples/external_mods/framework_no_items_demo/mod.json
  - docs/examples/external_mods/live_settings_realtime_demo/mod.json
  - build/modtool_reports/validate_mod_report.json

## [PLN-20260305-0001][UPDATE] 2026-03-08T17:36:45Z
- status: in_progress
- note: Starting next v5.2 block: reference scan + runtime severity matrix for unresolved refs, plus unresolved_reference_demo and compat_wrapper_demo coverage.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/unresolved_reference_demo/mod.json
  - docs/examples/external_mods/compat_wrapper_demo/mod.json

## [PLN-20260305-0001][UPDATE] 2026-03-08T17:58:17Z
- status: in_progress
- note: Implemented next v5.2 block: unresolved runtime refs now degrade to ERROR diagnostics instead of fatal disable for invalid onInput bindings and several catalog links (use/spell/projectile/aoe/item/scene/room). Added unresolved_reference_demo and compat_wrapper_demo, updated container_kit dependency on ui_kit, synced examples, and Release build now succeeds with /m:1.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods/unresolved_reference_demo/mod.json
  - docs/examples/external_mods/compat_wrapper_demo/mod.json
  - docs/examples/external_mods/container_kit/mod.json
  - tools/external_mods/sync_examples_to_runtime.ps1
  - build/modtool_reports/validate_mod_report.json

## [PLN-20260305-0001][UPDATE] 2026-03-08T18:02:26Z
- status: in_progress
- note: Starting next v5.2 block: docs/tooling sync for flexible contracts + settings, update example index, rerun exports/validation/build gate.
- refs:
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/examples/external_mods/README.md
  - tools/external_mods/export_runtime_reference.ps1

## [PLN-20260305-0001][UPDATE] 2026-03-08T18:23:07Z
- status: in_progress
- note: Completed next v5.2 block: synced docs/tooling for flexible contracts + settings (manifest/runtime contract docs, example index, exported actions/events/catalogs now include settings.* and settings.changed), added validator static onInput binding scan so unresolved_reference_demo reports ERROR without disabling, pruned unknown input bindings during runtime tick instead of disabling, and revalidated/build passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - docs/examples/external_mods/README.md
  - docs/runtime_contract/mod.manifest.v2.json
  - docs/runtime_contract/tool.validate_report.v1.json
  - docs/actions.json
  - docs/events.json
  - docs/catalogs.json

## [PLN-20260305-0001][UPDATE] 2026-03-08T18:48:49Z
- status: in_progress
- note: Starting next v5.2 block: broaden validator reference scan beyond onInput (useProfile/status/spell/postfx/light) and keep runtime/build validations green.
- refs:
  - tools/external_mods/validate_mod.ps1
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods/unresolved_reference_demo/README.md

## [PLN-20260305-0001][UPDATE] 2026-03-08T18:55:46Z
- status: in_progress
- note: Completed next v5.2 block: validate_mod now performs broader static reference scanning for useProfile/status/spell/postfx/skylight references across JSON files in a mod directory, allows built-in core/plain status tokens, deduplicates findings, and reports unresolved ids as ERROR without disabling. Revalidated unresolved_reference_demo plus representative good mods (world_profiles, status_catalog_demo, sss_content_pack).
- refs:
  - tools/external_mods/validate_mod.ps1
  - docs/examples/external_mods/unresolved_reference_demo/mod.json
  - docs/examples/external_mods/world_profiles/mod.json
  - docs/examples/external_mods/status_catalog_demo/mod.json
  - docs/examples/external_mods/sss_content_pack/mod.json
  - build/modtool_reports/validate_mod_report.json

## [PLN-20260310-0001] Volumetrics runtime real (GL+DX11+Metal) with v7 postfx/light integration
- createdUtc: 2026-03-10T17:01:01Z
- status: planned
- scope: engine
- summary: Implement volumetric fog/lighting runtime with postfx+lighting schema extensions, renderer bridge, backend passes, UI/CVars, docs/runtime export, and validation.
- milestones:
  1. Extend v7 postfx/light contracts and runtime resolve,Add renderer volumetrics bridge/interpreter integration and MSAA gating,Implement volumetrics passes for OpenGL, DX11, and Metal with fallback telemetry,Update UI/docs/examples/runtime exports and run Release validation
- tags: render,volumetrics,v7,external-mods,postfx,lighting
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/src/fast/interpreter.cpp

## [PLN-20260310-0001][UPDATE] 2026-03-10T17:36:09Z
- status: done
- note: Implemented volumetrics runtime/config/UI, OpenGL+DX11 backend passes, explicit Metal fallback reporting, updated examples/docs, Release build passed, exports/sync/drift/validate completed.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/src/fast/interpreter.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,docs/examples/external_mods/world_profiles/render/postfx_presets.json,docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260310-0001][UPDATE] 2026-03-10T17:37:14Z
- status: done
- note: Checkpoint scripts were reviewed in DryRun; finalize-plan-merge is blocked until the working tree is clean because the repo already contains unrelated pending changes.
- refs:
  - docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-feature.ps1,docs/agents/skills/soh-git-checkpoint-merge/scripts/commit-after-build.ps1,docs/agents/skills/soh-git-checkpoint-merge/scripts/finalize-plan-merge.ps1

## [PLN-20260310-0001][UPDATE] 2026-03-10T17:38:05Z
- status: in_progress
- note: Implemented volumetrics contract/runtime/UI/interpreter + OpenGL/DX11 passes, kept explicit Metal fallback, regenerated docs/exports/examples, Release build passed, and mod validators passed with rationale warnings only.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260310-0002] Aim OTS third-person battle locomotion + facing parity
- createdUtc: 2026-03-10T17:52:56Z
- status: in_progress
- scope: engine
- summary: Implement context-aware Aim OTS battle locomotion so projectile over-shoulder aim uses Z-lock movement/animation set with aim-facing, without real lock-on state.
- milestones:
  1. add contextual external-mods interop predicate for OTS battle locomotion
  2. patch z_player ground aim movement/actions to reuse Z-lock battle anim/move logic under OTS
  3. validate Release build and close with plan update/checkpoints
- tags: external-mods, aim-ots, player, animation, camera
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0003] Aim OTS third-person Z-lock locomotion parity
- createdUtc: 2026-03-10T17:53:00Z
- status: in_progress
- scope: engine
- summary: Add contextual Aim OTS locomotion/facing so projectile over-shoulder aim reuses Z-lock battle movement animations without creating real lock-on state.
- milestones:
  1. Add contextual external-mod interop predicate for projectile over-shoulder aim locomotion
  2. Integrate synthetic battle-aim helper in z_player battle movement/action branches and aim-facing yaw
  3. Build Release and record plan update/checkpoints
- tags: external-mods, aim-ots, z-player, animation, movement
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:12:55Z
- status: done
- note: Implemented Aim OTS projectile battle locomotion/facing parity via contextual external-mods interop and z_player battle-state reuse. MSBuild ClCompile passed for z_player.c and ExternalModManager.cpp, Link target passed for soh.vcxproj. Git checkpoint/finalize scripts were run in DryRun or blocked because the repo working tree is already dirty with unrelated changes.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - build/x64/soh/soh.vcxproj

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:16:37Z
- status: in_progress
- note: Follow-up on Aim OTS battle locomotion parity: investigating blocked WASD locomotion and slow camera drag caused by input consumption while OTS is active.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:20:36Z
- status: done
- note: Follow-up Aim OTS input fix applied: OTS battle movement no longer consumes left-stick/WASD as aim input in func_8084ABD8, leaving movement keys free for locomotion and removing the slow camera drag caused by movement-stick aim bleed. MSBuild ClCompile and Link passed for soh.vcxproj Release x64.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - build/x64/soh/soh.vcxproj

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:23:38Z
- status: in_progress
- note: Second follow-up on Aim OTS parity: tracing remaining first-person state/handlers that still block WASD and cause camera drag while OTS is active.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/controls/Mouse.cpp

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:26:48Z
- status: done
- note: Applied second Aim OTS follow-up: OTS battle movement now bypasses the unk_6AD first-person movement gate when resolving speed/yaw, and func_8083FD78 no longer uses the movement stick to drive focus pitch during OTS battle locomotion. This should restore WASD movement and stop camera drag from movement-stick aim bleed. MSBuild ClCompile and Link passed for soh.vcxproj Release x64.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - build/x64/soh/soh.vcxproj

## [PLN-20260310-0002][UPDATE] 2026-03-10T18:48:56Z
- status: in_progress
- note: Third follow-up on Aim OTS parity: fixing the remaining first-person state-machine coupling so over-shoulder projectile aim no longer sets PLAYER_STATE1_FIRST_PERSON or traps movement/camera in first-person behavior.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:01:51Z
- status: done
- note: Implemented the OTS third-person state-machine fix in z_player.c: over-shoulder projectile aim no longer stays in PLAYER_STATE1_FIRST_PERSON, movement/camera gates now honor third-person OTS aim, and first-person is restored only when OTS aim is exited/toggled off. Release build succeeded; git checkpoint scripts were run in DryRun and finalize-plan-merge remained blocked because the working tree is not clean.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - build/x64/soh/soh.vcxproj

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:10:21Z
- status: in_progress
- note: Fourth follow-up on Aim OTS parity: tracing a remaining re-entry loop in Player_ActionHandler_13 that may still be reapplying projectile aim state every frame and zeroing movement/camera while over-shoulder third-person aim is active.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:13:23Z
- status: done
- note: Applied a fourth Aim OTS follow-up in z_player.c: Player_ActionHandler_13 now stops re-entering over-shoulder projectile aim every frame once third-person OTS aim is active, and the main battle locomotion actions now refresh OTS aim facing each frame. Release build succeeded; git checkpoint scripts were run in DryRun and finalize-plan-merge remained blocked because the working tree is not clean.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - build/x64/soh/soh.vcxproj

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:34:39Z
- status: in_progress
- note: Fifth follow-up on Aim OTS parity: replacing the remaining bow-parallel strafe-only movement classification for third-person OTS so W/S map to forward/back and A/D map to strafe relative to aim yaw.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:38:52Z
- status: in_progress
- note: Sixth follow-up: fix OTS third-person movement vector mapping so W/S work and A/D strafe correctly without regressing camera aim.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:41:33Z
- status: done
- note: Fixed OTS third-person movement classification to preserve incoming movement yaw, route negative OTS input into backwalk, and verified Release build succeeds.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:48:34Z
- status: in_progress
- note: Seventh follow-up: fix remaining OTS movement frame-of-reference issues where A/D and S still collapse toward forward and camera/player can stall when rotating aim left.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:57:10Z
- status: done
- note: Fixed OTS movement/reference-frame mismatch by rebuilding OTS projectile movement yaw from actor.focus.rot.y plus local stick angle instead of camera input yaw, removed duplicate upper-body aim update in the OTS classifier, and verified the Release build succeeds.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T19:59:15Z
- status: in_progress
- note: Eighth follow-up: inspect raw stick-angle semantics and fix remaining OTS direction mapping so A/S/D map to left/back/right correctly.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:06:25Z
- status: done
- note: Fixed OTS stick-angle convention: pure OTS movement now converts local stick yaw using the real z_lib.c mapping (forward = 0x4000) before applying aim-relative movement, which should correct A/S/D direction mapping. Release build relinked soh.exe successfully.
- refs:
  - soh/src/code/z_lib.c
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:20:43Z
- status: in_progress
- note: Ninth follow-up: replace pure OTS stick-angle/yaw reconstruction with explicit local X/Y axis classification so A/S/D map deterministically to strafe/back/right without relying on sControlStickAngle trigonometry.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:25:18Z
- status: done
- note: Replaced the pure OTS movement classifier with explicit local stick-axis mapping: dominant Y now yields forward/back while dominant X yields left/right strafe with aim-relative yaw targets, removing the remaining sControlStickAngle-based direction reconstruction. Release build succeeded.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:32:26Z
- status: done
- note: Reworked the pure OTS movement classifier again to use the processed stick world yaw quantized relative to actor.focus.rot.y, preserving engine input conventions while removing the raw local-axis guesswork. Release build succeeded.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:42:13Z
- status: in_progress
- note: Tenth follow-up: fix remaining OTS state-direction mismatch where A/D still drift forward diagonally and S enters backward animation while translating forward.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:46:46Z
- status: in_progress
- note: Eleventh follow-up: correct remaining pure OTS movement mapping so A/D become pure left/right strafes and S becomes true backward relative to aim yaw instead of drifting forward/diagonal.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T20:59:21Z
- status: done
- note: Eleventh follow-up applied in z_player.c: pure OTS now preserves movement yaw separately from aim facing, snaps yaw to the aim-relative side/back target on pure OTS strafe/backwalk transitions, and keeps actor.shape.rot.y tracking the aim. Release build succeeded.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - x64/Release/soh.exe

## [PLN-20260310-0002][UPDATE] 2026-03-10T21:04:52Z
- status: in_progress
- note: Documenting the final Aim OTS debugging findings and movement-state fixes in a markdown file inside the OTS mod folder for future maintenance.
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0002][UPDATE] 2026-03-10T21:08:39Z
- status: done
- note: Saved the Aim OTS debugging history and final movement-state fix notes to OTS_MOVEMENT_FIX_NOTES.md inside docs/examples/external_mods/aim_ots_toggle_demo and synchronized the updated example mod to x64/Release/mods.
- refs:
  - docs/examples/external_mods/aim_ots_toggle_demo/OTS_MOVEMENT_FIX_NOTES.md
  - x64/Release/mods/aim_ots_toggle_demo/OTS_MOVEMENT_FIX_NOTES.md

## [PLN-20260310-0004] Framework data-driven de player resources + stamina BOTW
- createdUtc: 2026-03-10T21:57:12Z
- status: in_progress
- scope: mixed
- summary: Criar framework genérico de player resources com stamina persistente, HUD circular reutilizável, bridge de gameplay e mod exemplo data-driven estilo BOTW.
- milestones:
  1. Definir capability player.resources.v1 e runtime/persistência genéricos
  2. Implementar UI resource_rings reutilizável e bridge HUD/interface
  3. Integrar consumo/regeneração de stamina no gameplay do player
  4. Entregar mod exemplo, docs, exports, sync e validação final
- refs:
  - docs/agents/project_state.md
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/src/code/z_parameter.c

## [PLN-20260310-0005] Framework data-driven de player resources + mod exemplo de stamina BOTW
- createdUtc: 2026-03-10T21:57:28Z
- status: in_progress
- scope: mixed
- summary: Criar framework reutilizável de player resources para external-mods API v4, HUD circular de resource rings e mod exemplo completo de stamina com persistência, sprint e integração no player.
- milestones:
  1. Definir capability/contracts player.resources.v1 e ui.resource_rings.v1 com parser/runtime/export
  2. Implementar runtime genérico de player resources com persistência, ações/condições/hooks e bridge de gameplay
  3. Adicionar renderer reutilizável de resource ring contextual/fixo e integrá-lo ao loop de HUD
  4. Integrar stamina ao player para roll/shield/climb-hang/swim/sprint via bridge genérica
  5. Criar demo stamina BOTW, atualizar docs/exports, sincronizar mods e validar build
- tags: external-mods, stamina, hud, player-resource, data-driven
- refs:
  - docs/agents/project_state.md
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/src/code/z_parameter.c

## [PLN-20260310-0006] Framework data-driven de player resources + mod completo de stamina BOTW
- createdUtc: 2026-03-10T22:00:32Z
- status: in_progress
- scope: engine
- summary: Criar capability player.resources.v1, HUD circular reutilizável, bridge de gameplay no player e mod exemplo completo com stamina persistente e sprint via inputDefinitions.
- milestones:
  1. Adicionar contrato/runtime player.resources.v1 com parser, estado ativo, ações/condições/hooks e persistência por mod.
  2. Implementar HUD circular reutilizável com ring contextual perto do Link e fallback fixo na interface.
  3. Integrar roll/shield/climb-hang/swim/sprint ao runtime de recursos via bridge modular em z_player.
  4. Entregar mod exemplo de stamina BOTW com settings, input binding, upgrades persistentes, docs/exports e sync de demo.
- tags: external-mods, stamina, data-driven, hud, gameplay
- refs:
  - docs/agents/project_state.md
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0007] Docs/example/export slice for player resources stamina demo
- createdUtc: 2026-03-10T22:54:18Z
- status: in_progress
- scope: mixed
- summary: Scaffold API v4 example mod docs/examples/external_mods/player_resources_stamina_botw_demo and update docs/export tooling for planned player resource/resource ring contract without touching engine/runtime gameplay files.
- milestones:
  1. Inspect existing docs/example/export patterns and current contract outputs
  2. Create stamina example mod files (manifest/settings/input/resource defs/readme)
  3. Patch docs/export scripts and concise reference docs sections for planned contract
  4. Sync docs example to runtime mirror and validate targeted docs/export flows
- tags: external-mods, api-v4, docs, examples, stamina
- refs:
  - docs/examples/external_mods
  - tools/external_mods/export_runtime_reference.ps1
  - docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md

## [PLN-20260310-0007][UPDATE] 2026-03-10T23:02:14Z
- status: done
- note: Scaffolded player_resources_stamina_botw_demo under docs/examples, added planned player-resource/resource-ring contract entries to export_runtime_reference, regenerated docs runtime-reference outputs, updated concise authoring docs, and synchronized the demo to the runtime mods mirror.
- refs:
  - docs/examples/external_mods/player_resources_stamina_botw_demo
  - docs/examples/external_mods/README.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md
  - tools/external_mods/export_runtime_reference.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/catalogs.json
  - docs/actions.json
  - docs/events.json
  - docs/runtime_contract/actions.registry.json
  - docs/runtime_contract/conditions.registry.json

## [PLN-20260310-0004][UPDATE] 2026-03-11T00:13:16Z
- status: canceled
- note: Superseded by PLN-20260310-0006, which carried the framework-first player-resources/stamina implementation through build, docs, sync, and validation.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/code/z_parameter.c,soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0005][UPDATE] 2026-03-11T00:13:16Z
- status: canceled
- note: Superseded by PLN-20260310-0006 after consolidating the same player-resources/stamina scope into one surviving implementation plan.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/src/code/z_parameter.c,soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260310-0006][UPDATE] 2026-03-11T00:13:17Z
- status: done
- note: Implemented generic player.resources.v1 and ui.resource_rings.v1 runtime/HUD/gameplay bridges, delivered the BOTW stamina demo, regenerated runtime docs, synced demos, passed validate_mod, passed doc drift check, and built Release soh successfully.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModPlayerResourcesRuntime.cpp,soh/src/code/z_parameter.c,soh/src/overlays/actors/ovl_player_actor/z_player.c,docs/examples/external_mods/player_resources_stamina_botw_demo,tools/external_mods/validate_mod.ps1

## [PLN-20260311-0001] Fix stamina demo manifest runtime budgets
- createdUtc: 2026-03-11T00:17:14Z
- status: in_progress
- scope: mod
- summary: Corrigir player_resources_stamina_botw_demo para usar runtime.maxActorInstances/maxActiveStatuses válidos, sincronizar o mirror e validar o mod.
- milestones:
  1. Ajustar mod.json do demo de stamina,Sincronizar docs/examples para x64/Release/mods,Validar o demo corrigido
- tags: external-mods,stamina,manifest,validation
- refs:
  - docs/examples/external_mods/player_resources_stamina_botw_demo/mod.json,tools/external_mods/sync_examples_to_runtime.ps1,tools/external_mods/validate_mod.ps1

## [PLN-20260311-0001][UPDATE] 2026-03-11T00:18:05Z
- status: done
- note: Fixed player_resources_stamina_botw_demo runtime budgets by changing maxActorInstances/maxActiveStatuses to valid values, synchronized the runtime mods mirror, and revalidated the demo successfully.
- refs:
  - docs/examples/external_mods/player_resources_stamina_botw_demo/mod.json,x64/Release/mods/player_resources_stamina_botw_demo/mod.json,tools/external_mods/validate_mod.ps1

## [PLN-20260311-0002] Fix input key alias parsing for stamina sprint binding
- createdUtc: 2026-03-11T00:21:21Z
- status: in_progress
- scope: engine
- summary: Adicionar suporte a aliases de Shift no parser de defaultKeyboardKeys para remover o fatal LSHIFT do demo de stamina e validar com build incremental.
- milestones:
  1. Expandir aliases de teclado aceitos pelo parser v4,Rebuild do soh com a correção,Confirmar que o demo de stamina não falha mais por LSHIFT
- tags: external-mods,input,stamina,parser
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/examples/external_mods/player_resources_stamina_botw_demo/config/input.json

## [PLN-20260311-0002][UPDATE] 2026-03-11T00:23:00Z
- status: blocked
- note: Patched the external-mod keyboard alias parser to accept Shift/control/alt common tokens including LSHIFT, and validate_mod passed. Final Release link is blocked because x64/Release/soh.exe is currently locked by a running process (LNK1104).
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,docs/examples/external_mods/player_resources_stamina_botw_demo/config/input.json,x64/Release/soh.exe

## [PLN-20260311-0003] Fix white stamina ring rendering
- createdUtc: 2026-03-11T00:32:33Z
- status: in_progress
- scope: engine
- summary: Corrigir o renderer de ui.resource_rings.v1 para respeitar as cores do anel de stamina em vez de desenhar o fill/background em branco.
- milestones:
  1. Inspecionar o setup de combine/render do ring HUD,Ajustar z_parameter.c para usar cores corretas no fill/background,Rebuild do soh para validar a correção visual
- tags: external-mods,stamina,hud,render
- refs:
  - soh/src/code/z_parameter.c,docs/examples/external_mods/player_resources_stamina_botw_demo/ui/resource_rings.json

## [PLN-20260311-0003][UPDATE] 2026-03-11T00:34:00Z
- status: blocked
- note: Patched the stamina ring HUD renderer to use vertex shading (G_CC_SHADE) instead of primitive white, which should restore normalColor/lowColor/exhaustedColor rendering. Incremental compile reached z_parameter.c, but final relink is blocked because x64/Release/soh.exe is still locked by the running game process.
- refs:
  - soh/src/code/z_parameter.c,docs/examples/external_mods/player_resources_stamina_botw_demo/ui/resource_rings.json,x64/Release/soh.exe

## [PLN-20260311-0004] Add mana-style yellow stamina bar option to player resources HUD
- createdUtc: 2026-03-11T00:39:29Z
- status: planned
- scope: tooling
- summary: Extend ui.resource_rings.v1 with a mana-style yellow stamina presentation below the magic meter, configurable in the stamina demo with both views enabled by default.
- milestones:
  1. define milestones
- tags: ui,external-mods,player-resources,stamina
- refs:
  - docs/agents/project_state.md

## [PLN-20260311-0004][UPDATE] 2026-03-11T00:51:42Z
- status: done
- note: Implemented the stamina HUD mana-style bar option below the magic meter, kept the contextual ring path, synced the demo, validated the mod, and built Release soh; finalize-plan-merge remained blocked because the working tree is dirty.
- refs:
  - soh/src/code/z_parameter.c
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/soh/Enhancements/external-mods/ExternalModPlayerResourcesRuntime.cpp
  - docs/examples/external_mods/player_resources_stamina_botw_demo/ui/resource_rings.json
  - docs/examples/external_mods/player_resources_stamina_botw_demo/settings/settings.schema.json

## [PLN-20260311-0005] Execute roadmap phase 1 for runtime graphics base consolidation
- createdUtc: 2026-03-11T03:05:59Z
- status: planned
- scope: engine
- summary: Start the practical graphics roadmap by closing the current base: PBR parity/material-override propagation and render inspector expansion for PBR/SSAO/volumetrics/fallback telemetry.
- milestones:
  1. define milestones
- tags: render,pbr,ssao,volumetrics,inspector
- refs:
  - docs/agents/project_state.md

## [PLN-20260311-0005][UPDATE] 2026-03-11T03:28:15Z
- status: done
- note: Implemented roadmap phase 1 groundwork: resolved active PBR profile state in world graphics runtime, expanded render inspector telemetry for backend/source/fallback/material stats, and added a real material albedo override bridge from ExternalModManager into the fast interpreter texture load path. Release build succeeded.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/src/fast/interpreter.cpp
  - build/x64/soh/soh.vcxproj

## [PLN-20260311-0006] Implement auto-generated material fallback maps and organize runtime mods folders
- createdUtc: 2026-03-11T03:30:03Z
- status: in_progress
- scope: engine
- summary: Add deterministic automatic fallback generation for missing normal/PBR material maps from existing textures, and reorganize the runtime mods mirror into frameworks/resourcepacks/mods with sync/runtime support.
- milestones:
  1. Inspect current material override/runtime texture load path and mods mirror assumptions
  2. Implement deterministic fallback-map generation plus material/runtime wiring
  3. Reorganize runtime mods folder structure and update sync/discovery/build validation
  4. Run Release build plus mod sync/validation and append plan update
- tags: render, pbr, external-mods, tooling, mods-folder
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - libultraship/src/fast/interpreter.cpp
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/agents/project_state.md

## [PLN-20260311-0006][UPDATE] 2026-03-11T04:00:20Z
- status: done
- note: Implemented bucketed mods folders plus automatic runtime fallback generation for missing normal/ORM maps on active materials; sync/validate passed, full Release link remains blocked if soh.exe is open.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModManager.h
  - libultraship/include/fast/interpreter.h
  - libultraship/src/fast/interpreter.cpp
  - tools/external_mods/sync_examples_to_runtime.ps1
  - mods/mods
  - x64/Release/mods/mods

## [PLN-20260311-0007] Fix Windows MSB3191 invalid object directory paths in soh Release build
- createdUtc: 2026-03-11T04:06:04Z
- status: in_progress
- scope: tooling
- summary: Diagnose and fix malformed generated object directory paths (Release\\/soh/...) that break MSBuild directory creation during soh Release rebuild.
- milestones:
  1. 1. reproduce MSB3191 and inspect generated vcxproj entries
  2. 2. trace malformed path back to CMake/source list
  3. 3. patch path normalization, regenerate, rebuild Release
  4. 4. record validation and plan update
- tags: build,windows,cmake,msbuild
- refs:
  - build/x64/soh/soh.vcxproj
  - CMakeLists.txt
  - soh/CMakeLists.txt

## [PLN-20260311-0007][UPDATE] 2026-03-11T04:07:57Z
- status: done
- note: Reproduced the rebuild attempt on 2026-03-11 and both `cmake --build build/x64 --config Release --target soh -- /m:1` and direct MSBuild of build/x64/soh/soh.vcxproj succeeded without MSB3191; no code patch was required in the current generated tree.
- refs:
  - build/x64/soh/soh.vcxproj
  - x64/Release/soh.exe

## [PLN-20260311-0008] Exclude resourcepacks from External Mods discovery/UI
- createdUtc: 2026-03-11T04:11:04Z
- status: in_progress
- scope: engine
- summary: Stop treating mods/resourcepacks entries as external mods so non-mod resource packs no longer appear as fatal packages in the External Mods UI.
- milestones:
  1. 1. trace package discovery and tab bucketing for resourcepacks
  2. 2. patch discovery/UI to ignore resourcepacks as external mods while preserving frameworks and mods buckets
  3. 3. sync runtime folders and validate rebuild/runtime behavior
- tags: external-mods,ui,mods-layout,resourcepacks
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260311-0008][UPDATE] 2026-03-11T04:13:23Z
- status: done
- note: Stopped External Mods discovery from scanning mods/resourcepacks as package roots, so standalone resourcepacks are no longer loaded as fatal external-mod entries in the Mods tab. ClCompile validation passed; full Release link remains blocked if soh.exe is open.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp

## [PLN-20260311-0009] Fix invalid demo runtime budgets and loot respawn actor limit
- createdUtc: 2026-03-11T04:15:40Z
- status: in_progress
- scope: mod
- summary: Correct API v4 demo manifests with invalid runtime budgets and resolve demo_loot_respawn failing actor instance limits, then sync examples to runtime mods.
- milestones:
  1. 1. inspect affected demo manifests and actor definitions
  2. 2. patch invalid runtime budgets and actor instance limits
  3. 3. sync examples to runtime and validate affected demos
  4. 4. document residual quarantine reset guidance if needed
- tags: external-mods,demos,manifest,quarantine
- refs:
  - docs/examples/external_mods
  - x64/Release/mods/mods
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260311-0009][UPDATE] 2026-03-11T04:19:04Z
- status: done
- note: Fixed invalid 0/0 runtime budgets in compat_wrapper_demo, live_settings_realtime_demo, framework_no_items_demo, and unresolved_reference_demo; raised demo_loot_respawn crate_anchor maxInstances from 1 to 2 so the placed actor and spawn-profile instance can coexist; synced runtime demos and validate_mod passed for the affected runtime mirrors (loot respawn only retains capability-rationale warnings).
- refs:
  - docs/examples/external_mods/compat_wrapper_demo/mod.json
  - docs/examples/external_mods/live_settings_realtime_demo/mod.json
  - docs/examples/external_mods/framework_no_items_demo/mod.json
  - docs/examples/external_mods/unresolved_reference_demo/mod.json
  - docs/examples/external_mods/demo_loot_respawn/actors/actors.json
  - x64/Release/mods/mods/demo_loot_respawn

## [PLN-20260311-0010] Add survival needs framework and demo with hunger, thirst, forage, and bottle water
- createdUtc: 2026-03-11T04:41:43Z
- status: in_progress
- scope: mixed
- summary: Implement data-driven survival systems by extending player resources and resource ring HUD, adding consumables and forage capabilities, integrating bottle water/tall grass hooks, and shipping a compatible demo mod.
- milestones:
  1. Inspect current resource/hud/item/grass hooks and choose hook points
  2. Implement player resource tick/depletion extensions plus stacked HUD support
  3. Add player.consumables.v1 runtime, conditions/actions, and bottle-water integration
  4. Add world.forage.v1 runtime and tall-grass Odd Mushroom drops
  5. Create and sync player_survival_needs_demo, rebuild, validate docs/runtime drift, and finalize governance updates
- tags: external-mods, survival, player-resources, ui, forage, consumables
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModPlayerResourcesRuntime.cpp
  - soh/src/code/z_parameter.c
  - soh/src/overlays/actors/ovl_En_Kusa/z_en_kusa.c
  - docs/examples/external_mods

## [PLN-20260311-0010][UPDATE] 2026-03-11T05:53:59Z
- status: done
- note: Implemented survival needs framework and demo: passive hunger/thirst resources, stacked magic-bar HUD, custom consumables, bottle water, tall-grass forage, docs/export sync, build and validator pass.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModPlayerResourcesRuntime.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/src/overlays/actors/ovl_En_Kusa/z_en_kusa.c
  - docs/examples/external_mods/player_survival_needs_demo
  - tools/external_mods/export_runtime_reference.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260311-0011] Add two-use bottle content support for survival water
- createdUtc: 2026-03-11T11:21:31Z
- status: in_progress
- scope: mixed
- summary: Extend player.consumables.v1 bottleContent to support multi-use fills, persist remaining uses per bottle slot, and update player_survival_needs_demo fresh water to behave like Lon Lon Milk with two uses.
- milestones:
  1. extend consumable runtime state and persistence for remaining bottle uses
  2. update bottle consume/fill logic and placeholder items for full/half states
  3. update survival demo docs and validate build/runtime sync
- tags: external-mods, consumables, survival, bottles
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/player_survival_needs_demo/player/player_consumables.json

## [PLN-20260311-0011][UPDATE] 2026-03-11T11:30:14Z
- status: done
- note: Implemented two-use bottle content support for player.consumables.v1 (usesPerFill + placeholderPartialItemId + persisted remainingUses), updated player_survival_needs_demo fresh_water to behave like Lon Lon Milk with 2 uses, exported docs, synced demos, and validated compile + mod shape. Full Release relink remained blocked because soh.exe was running.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/player_survival_needs_demo/player/player_consumables.json
  - docs/examples/external_mods/player_survival_needs_demo/README.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260311-0012] Add survival water placeholder assets and inventory-backed Odd Mushroom consumable
- createdUtc: 2026-03-11T11:35:49Z
- status: in_progress
- scope: mixed
- summary: Add placeholder bottle icon assets to player_survival_needs_demo and extend player.consumables.v1 so stack consumables can expose an inventory-backed vanilla slot placeholder (Odd Mushroom) while preserving mod-side stack counts and bottle/food use behavior.
- milestones:
  1. inspect current consumable/inventory hook points and mod asset paths
  2. implement inventory-backed stack consumable placeholder flow plus bottle icon placeholder asset resolution
  3. update survival demo assets/config/docs then build/sync/validate
- tags: external-mods, survival, consumables, inventory, assets
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/player_survival_needs_demo

## [PLN-20260311-0012][UPDATE] 2026-03-11T11:55:54Z
- status: done
- note: Implemented inventory-backed Odd Mushroom consumption via the vanilla slot placeholder, added fresh_water full/half placeholder PNGs under the survival demo, rebuilt Release, exported runtime references, synced examples, and validated source/runtime demo packages.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - soh/soh/Enhancements/external-mods/ExternalModInterop.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - docs/examples/external_mods/player_survival_needs_demo/player/player_consumables.json
  - docs/examples/external_mods/player_survival_needs_demo/ui/assets/fresh_water_full.png
  - docs/examples/external_mods/player_survival_needs_demo/ui/assets/fresh_water_half.png
  - docs/examples/external_mods/player_survival_needs_demo/README.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260311-0013] Route survival water placeholder PNGs to bottle C-slot icons
- createdUtc: 2026-03-11T12:02:05Z
- status: in_progress
- scope: mixed
- summary: Move the new fresh water full/half PNGs from the thirst HUD role into actual bottle placeholder icon overrides for custom bottle content on equipped buttons, while removing the accidental thirst-bar icon usage.
- milestones:
  1. inspect current bottle icon draw path and consumable asset fields
  2. add consumable placeholder icon asset parsing plus button icon override support for custom bottle content
  3. update survival demo config/docs then rebuild sync validate and record governance
- tags: external-mods, survival, consumables, ui, inventory, assets
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/player_survival_needs_demo

## [PLN-20260311-0013][UPDATE] 2026-03-11T12:07:15Z
- status: done
- note: Re-targeted fresh water full/half PNGs into bottle placeholderIconAsset fields for player.consumables.v1, removed the accidental thirst-bar icon usage, rebuilt Release, exported runtime references, synced demos, and validated the survival mod in source/runtime mirrors.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/player_survival_needs_demo/player/player_consumables.json
  - docs/examples/external_mods/player_survival_needs_demo/ui/resource_rings.json
  - docs/examples/external_mods/player_survival_needs_demo/ui/assets/fresh_water_full.png
  - docs/examples/external_mods/player_survival_needs_demo/ui/assets/fresh_water_half.png
  - docs/examples/external_mods/player_survival_needs_demo/README.md
  - docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
  - docs/SYLIAN_FOUNDRY_MODLOADER.md

## [PLN-20260311-0014] Bypass vanilla Odd Mushroom trade-item dialog for inventory-backed survival food
- createdUtc: 2026-03-11T12:11:54Z
- status: in_progress
- scope: mixed
- summary: Fix the survival demo so the inventory-backed Odd Mushroom placeholder consumes through the custom drink-demo path on C-slot use instead of falling into the vanilla trade-item "does not work here" message flow.
- milestones:
  1. trace the actual player/item action path used when pressing the C-slot Odd Mushroom placeholder
  2. patch the player/runtime hook so inventory-backed stack consumables short-circuit the vanilla trade-item fallback
  3. rebuild sync validate and record governance updates
- tags: external-mods, survival, consumables, inventory, player
- refs:
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods/player_survival_needs_demo/player/player_consumables.json

## [PLN-20260311-0014][UPDATE] 2026-03-11T12:18:30Z
- status: done
- note: Adjusted inventory-backed consumable detection so it no longer requires heldItemId to already equal the placeholder item during the pending-use window, and added a fallback redirect in Player_Action_ExchangeItem so the survival Odd Mushroom escapes the vanilla trade-item dialog path. ClCompile passed; full Release link remained blocked because soh.exe was running.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/overlays/actors/ovl_player_actor/z_player.c

## [PLN-20260311-0015] Fix volumetric fog debug view rendering
- createdUtc: 2026-03-11T14:22:59Z
- status: in_progress
- scope: mixed
- summary: Investigate why the volumetric fog debug view shows nothing, patch the GL/DX11 debug composite path so the volumetric buffer can be visualized correctly, validate with a Release build, and update docs/governance if contract-facing behavior changes.
- milestones:
  1. inspect current volumetrics debug/config flow in runtime interpreter and backend passes
  2. patch backend/interpreter debug-view logic so volumetric fog debug output is visible and deterministic
  3. rebuild Release and run drift/governance updates
- tags: render, volumetrics, debug, external-mods
- refs:
  - libultraship/src/fast/interpreter.cpp
  - libultraship/src/fast/backends/gfx_opengl.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0015][UPDATE] 2026-03-11T14:50:24Z
- status: done
- note: Patched GL/DX11 volumetrics debug shaders to emit explicit density/lighting visualization and preserved volumetrics enable state across Play frames so StartFrame can arm the offscreen path required by ApplyVolumetrics. Release build passed after clearing stale soh.iobj/soh.ipdb incremental link artifacts.
- refs:
  - libultraship/src/fast/backends/gfx_opengl.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0016] Rebucket external-mod frameworks into mods/frameworks
- createdUtc: 2026-03-11T15:13:23Z
- status: in_progress
- scope: tooling
- summary: Route framework/core_api example packages to the frameworks bucket instead of mods/mods and resync the runtime mirror so framework packs no longer live under the mods bucket.
- milestones:
  1. Inspect current sync/discovery assumptions
  2. Patch sync bucketing using manifest metadata
  3. Resync runtime folders and validate framework placement
- tags: external-mods, mods-layout, frameworks, sync
- refs:
  - tools/external_mods/sync_examples_to_runtime.ps1
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - x64/Release/mods/frameworks
  - x64/Release/mods/mods

## [PLN-20260311-0016][UPDATE] 2026-03-11T15:15:37Z
- status: done
- note: Patched sync_examples_to_runtime.ps1 to route framework/core_api packages into x64/Release/mods/frameworks using mod.json metadata, preserved user resourcepacks during -Clean, resynced runtime mirror, and verified framework packs are no longer present under x64/Release/mods/mods.
- refs:
  - tools/external_mods/sync_examples_to_runtime.ps1
  - x64/Release/mods/frameworks
  - x64/Release/mods/mods

## [PLN-20260311-0017] Fix survival HUD numeric setting application
- createdUtc: 2026-03-11T15:26:20Z
- status: in_progress
- scope: mod
- summary: Investigate why player_survival_needs_demo HUD scale/opacity sliders fail with requires numeric value and patch the schema/runtime path so the settings apply correctly at runtime.
- milestones:
  1. Inspect survival settings schema and runtime setting definitions
  2. Patch numeric setting type/default/range handling
  3. Validate setting changes in source and runtime mirror
- tags: external-mods, survival, settings, ui
- refs:
  - docs/examples/external_mods/player_survival_needs_demo/settings/settings.schema.json
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/src/code/z_parameter.c

## [PLN-20260311-0017][UPDATE] 2026-03-11T15:31:03Z
- status: blocked
- note: Patched locale-invariant float parsing/formatting for External Mods settings in ExternalModManager and ExternalModUi to accept comma decimals and always serialize with dot. ClCompile passed, but final Release link is blocked because soh.exe is still running (LNK1104 on x64/Release/soh.exe).
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - x64/Release/soh.exe

## [PLN-20260311-0018] Fix demo actor instance limits for furnace and loot respawn
- createdUtc: 2026-03-11T15:32:29Z
- status: in_progress
- scope: mod
- summary: Raise data-driven actor definition instance limits in demo_furnace_system and demo_loot_respawn so toggle/spawn flows stop quarantining the demos, then sync runtime and clear stale quarantine files.
- milestones:
  1. Inspect actor defs and spawn/toggle flows
  2. Patch insufficient instance limits in source demos
  3. Sync runtime mirror and remove stale quarantine files
- tags: external-mods, demos, actors, quarantine
- refs:
  - docs/examples/external_mods/demo_furnace_system
  - docs/examples/external_mods/demo_loot_respawn
  - x64/Release/mods/mods/demo_furnace_system
  - x64/Release/mods/mods/demo_loot_respawn

## [PLN-20260311-0018][UPDATE] 2026-03-11T15:35:28Z
- status: done
- note: Raised demo_furnace_system furnace_anchor maxInstances from 1 to 2 so the scene-spawned anchor plus toggleArchetype instance can coexist without hitting the actor definition limit. Synced runtime mirror and validated both demo_furnace_system and demo_loot_respawn; demo_loot_respawn already had maxInstances=2 in source/runtime, so any remaining fatal state is stale quarantine that must be cleared/reloaded in the running game.
- refs:
  - docs/examples/external_mods/demo_furnace_system/actors/actors.json
  - x64/Release/mods/mods/demo_furnace_system/actors/actors.json
  - docs/examples/external_mods/demo_loot_respawn/actors/actors.json
  - x64/Release/mods/mods/demo_loot_respawn/actors/actors.json

## [PLN-20260311-0019] DX11 volumetrics visibility + advanced graphics controls
- createdUtc: 2026-03-11T15:44:15Z
- status: in_progress
- scope: engine
- summary: Make DX11 volumetrics visibly render as real fog, extend render.postfx.v1 with volumetrics color/ambient intensity, add advanced menu overrides, and expand runtime inspector telemetry.
- milestones:
  1. Fix DX11 volumetrics medium/scattering composition and debug modes,Extend postfx/parser/runtime config for volumetrics color and ambient intensity,Add advanced graphics menu overrides for volumetrics and base postfx,Update examples/docs/runtime exports and validate build+drift
- tags: render,volumetrics,dx11,external-mods,graphics-ui
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,soh/soh/SohGui/SohMenuSettings.cpp,docs/examples/external_mods/world_profiles/render/postfx_presets.json

## [PLN-20260311-0019][UPDATE] 2026-03-11T16:16:26Z
- status: done
- note: Implemented DX11 volumetric medium fog + debug modes, advanced graphics overrides/menu, inspector runtime stats, and updated world_profiles/docs; build/sync/validate/drift passed.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260311-0020] Fix DX11 volumetrics depth-space mismatch and debug output
- createdUtc: 2026-03-11T16:25:47Z
- status: in_progress
- scope: engine
- summary: Patch DX11 volumetrics so world reconstruction/projection matches engine NDC space, restoring visible fog and working debug view; then validate Release build.
- milestones:
  1. inspect DX11 shader depth/NDC path against GL reference
  2. patch DX11 volumetrics reconstruction/projection and any dependent debug/composite plumbing
  3. build Release and summarize in-game validation steps
- tags: volumetrics,dx11,graphics,debug
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/src/fast/interpreter.cpp

## [PLN-20260311-0020][UPDATE] 2026-03-11T16:29:00Z
- status: done
- note: Patched DX11 volumetrics depth-space reconstruction/projection to match engine NDC space (-1..1), restoring valid world/depth mapping for fog accumulation and debug view; Release build passed.
- refs:
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe

## [PLN-20260311-0021] Fix stamina/survival demo numeric setting load blocker
- createdUtc: 2026-03-11T16:34:01Z
- status: in_progress
- scope: mod
- summary: Investigate why stamina and survival demo float defaults fail schema normalization under runtime load, patch the smallest locale-safe settings parsing path, sync mirrors, and validate the affected demos.
- milestones:
  1. Inspect the runtime settings normalization path and confirm the first blocker from logs
  2. Patch locale-invariant float parsing and numeric normalization used by settings schemas
  3. Sync docs examples to runtime mirrors and validate the stamina/survival demos
- tags: external-mods, demos, settings, stamina, survival
- refs:
  - docs/examples/external_mods/player_resources_stamina_botw_demo/settings/settings.schema.json
  - docs/examples/external_mods/player_survival_needs_demo/settings/settings.schema.json
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260311-0021][UPDATE] 2026-03-11T16:37:15Z
- status: done
- note: Identified locale-sensitive float parsing as the first blocker behind the numeric default load failures, patched TryParseFloatString to trim input and parse with the classic locale after comma-to-dot normalization, rebuilt Release, synced demos, and validated the stamina/survival/live-settings demos in both docs and runtime mirrors.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - docs/examples/external_mods/player_resources_stamina_botw_demo/settings/settings.schema.json
  - docs/examples/external_mods/player_survival_needs_demo/settings/settings.schema.json
  - x64/Release/mods/mods/player_resources_stamina_botw_demo/settings/settings.schema.json
  - x64/Release/mods/mods/player_survival_needs_demo/settings/settings.schema.json

## [PLN-20260311-0022] Fix DX11 volumetrics execution/debug blocker
- createdUtc: 2026-03-11T16:38:38Z
- status: in_progress
- scope: engine
- summary: Investigate why DX11 volumetrics still do not appear or show debug output in-game, patch the first execution blocker in the runtime/backend path, rebuild Release, and provide a focused validation checklist.
- milestones:
  1. Inspect the DX11 volumetrics execution path, gates, and debug-mode plumbing to isolate the first blocker
  2. Patch the smallest DX11/runtime issue preventing volumetrics and debug output from appearing
  3. Rebuild Release and report concrete validation steps for DirectX11
- tags: graphics, volumetrics, dx11, external-mods
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0022][UPDATE] 2026-03-11T16:52:48Z
- status: done
- note: Auto quality now promotes when presets, custom overrides, debug modes, or active volumetric lights demand the pass; OpenGL now consumes fog color, ambient intensity, debug mode, and reports runtime stats like DX11.
- refs:
  - docs/agents/project_state.md

## [PLN-20260311-0023] Normalize external resource HUD bar sizing and add shared stack positioning controls
- createdUtc: 2026-03-11T17:07:33Z
- status: planned
- scope: engine
- summary: Make external magic-style resource bars match the vanilla magic bar size, tighten stack spacing, and add shared centered position controls for the stack so hunger/thirst/stamina bars can be relocated deterministically.
- milestones:
  1. Inspect current resource ring layout/render path and settings resolution
  2. Patch shared magic-style bar renderer to use vanilla magic bar sizing and shared position/spacing overrides
  3. Add menu CVars for centered stack X/Y positioning and spacing, update example defaults, rebuild and validate
- tags: external-mods,hud,player-resources,survival,stamina
- refs:
  - soh/src/code/z_parameter.c
  - soh/soh/SohGui/SohMenuSettings.cpp
  - docs/examples/external_mods/player_resources_stamina_botw_demo/ui/resource_rings.json
  - docs/examples/external_mods/player_survival_needs_demo/ui/resource_rings.json

## [PLN-20260311-0023][UPDATE] 2026-03-11T17:12:09Z
- status: blocked
- note: z_parameter.c now matches magic-bar width, uses tighter stack spacing, and supports shared centered stack X/Y controls via gEnhancements.Graphics.ExternalResourceBars.*. Graphics menu widgets were added and stamina/survival resource_rings defaults were tightened to 17px. Compile-only validation and demo validation passed; final Release relink is blocked by LNK1104 until soh.exe is closed.
- refs:
  - docs/agents/project_state.md

## [PLN-20260311-0024] Render inspector visibility + overlay for volumetrics debug
- createdUtc: 2026-03-11T17:23:14Z
- status: in_progress
- scope: engine
- summary: Make render inspector discoverable and usable by auto-enabling it with volumetrics debug demand, exposing per-mod toggles in External Mods UI, and drawing a fixed on-screen summary overlay.
- milestones:
  1. Inspect current render inspector visibility flow and UI entry points
  2. Wire default/runtime activation so inspector can follow active volumetrics debug state
  3. Expose render inspector overlay toggles in External Mods UI
  4. Add fixed on-screen render inspector summary overlay and validate with Release build
- tags: external-mods, render-inspector, volumetrics, ui
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModUi.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/SohGui/SohGui.cpp

## [PLN-20260311-0024][UPDATE] 2026-03-11T17:44:15Z
- status: done
- note: Implementado gate por volumetrics debug, toggles de render inspector na External Mods UI, overlay fixo na tela e default-on para lighting_kit; build/sync/validate concluídos.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,soh/soh/SohGui/SohGui.cpp,docs/examples/external_mods/lighting_kit/debug/render_inspector.json

## [PLN-20260311-0025] Volumetrics DX11 activation gate fix from inspector evidence
- createdUtc: 2026-03-11T18:06:40Z
- status: in_progress
- scope: engine
- summary: Use inspector evidence showing volumetrics=off/executed=no under DX11 custom debug to fix activation gating so custom overrides/debug can force the pass on and expose the reason more clearly.
- milestones:
  1. Inspect DX11 volumetrics gating and inspector summary path,Patch runtime enable logic for custom/debug demand and improve summary,Build Release and validate sync/checkpoints
- tags: volumetrics,dx11,render-inspector,external-mods
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,libultraship/src/fast/interpreter.cpp,soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260311-0025][UPDATE] 2026-03-11T18:10:09Z
- status: done
- note: Inspector evidence showed volumetrics was resolving debug/custom values but staying disabled. Runtime now force-enables volumetrics whenever custom overrides or volumetrics debug are active, and the summary now prints master/custom flags for faster diagnosis. Release build passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,x64/Release/soh.exe

## [PLN-20260311-0025][UPDATE] 2026-03-11T18:20:46Z
- status: done
- note: Inspector still showed volumetrics=off/executed=no under DX11. Hardened both runtime and interpreter so debug/custom demand can force the pass to run when quality>0, even if the base enable bit falls out of sync. Release build passed again.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,libultraship/src/fast/interpreter.cpp,x64/Release/soh.exe

## [PLN-20260311-0026] Render inspector overlay copy button
- createdUtc: 2026-03-11T18:26:52Z
- status: in_progress
- scope: engine
- summary: Add a copy-to-clipboard button directly in the fixed Render Inspector overlay so users can grab the current summary without opening External Mods UI.
- milestones:
  1. Inspect overlay window implementation,Add copy button and optional feedback notification,Build Release and record plan update
- tags: render-inspector,ui,external-mods
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModRenderInspectorWindow.cpp,soh/soh/Enhancements/external-mods/ExternalModUi.cpp

## [PLN-20260311-0026][UPDATE] 2026-03-11T18:28:10Z
- status: blocked
- note: Adicionado botão Copy no overlay do Render Inspector com cópia para clipboard e notificação curta. Compilação passou, mas o relink de soh.exe bloqueou com LNK1104 porque o executável está aberto.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModRenderInspectorWindow.cpp,x64/Release/soh.exe

## [PLN-20260311-0027] DX11 volumetrics debug mode and stale fallback fix
- createdUtc: 2026-03-11T18:46:00Z
- status: in_progress
- scope: engine
- summary: Fix DirectX11 volumetrics debug visualization so Light Energy and Final Volume show meaningful output and clear stale runtime-unsupported fallback when the pass executed successfully.
- milestones:
  1. Inspect DX11 volumetrics shader debug branches and fallback reporting,Patch debug output and stale fallback clearing,Build Release and validate via inspector summary
- tags: volumetrics,dx11,debug,render-inspector
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,libultraship/src/fast/interpreter.cpp

## [PLN-20260311-0027][UPDATE] 2026-03-11T18:52:17Z
- status: done
- note: DX11 volumetrics debug modes revised: mode 1 now uses a neutral density heatmap, mode 2 uses an energy heatmap, mode 3 boosts final volume visibility with alpha-guided neutral compositing, and stale runtime-unsupported fallback is cleared each supported frame. Release build passed.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,libultraship/src/fast/interpreter.cpp,x64/Release/soh.exe

## [PLN-20260311-0028] DX11 volumetrics truthful debug telemetry and raw preview
- createdUtc: 2026-03-11T19:00:02Z
- status: in_progress
- scope: engine
- summary: Replace false-positive volumetrics debug telemetry with real dispatched/produced/debugPreviewValid stats and rework DX11 debug modes to preview raw density, light energy, and final volume instead of misleading composite output.
- milestones:
  1. Inspect current runtime stats structs and DX11 volumetrics debug path,Patch DX11 raw debug outputs and truthful volumetrics runtime stats,Build Release and update ledger/checkpoints
- tags: volumetrics,dx11,debug,telemetry,render-inspector
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/include/fast/backends/gfx_direct3d_common.h,libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0029] Advanced light and skylight config pass before next Release build
- createdUtc: 2026-03-11T19:01:07Z
- status: planned
- scope: engine
- summary: Add configurable light and skylight controls/data-driven overrides before the next Release build so volumetrics and graphics tuning can be validated with proper lighting inputs.
- milestones:
  1. Inspect current light/skylight profile fields and menu exposure,Add missing global/custom controls for light and skylight,Sync docs/examples and validate with Release build
- tags: lighting,skylight,graphics,external-mods
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,soh/soh/SohGui/SohMenuSettings.cpp,docs/examples/external_mods/lighting_kit

## [PLN-20260311-0028][UPDATE] 2026-03-11T19:05:43Z
- status: in_progress
- note: Started implementation of truthful DX11 volumetrics debug telemetry: split dispatched vs produced/debug-valid stats, raw debug preview path, and fallback cleanup in renderer/runtime summary.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/src/fast/backends/gfx_direct3d11.cpp,libultraship/src/fast/interpreter.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0028][UPDATE] 2026-03-11T19:23:20Z
- status: in_progress
- note: Patched DX11 volumetrics to write raw metrics and preview raw debug buffers, split dispatched/produced/debugPreviewValid runtime stats, and cleared stale fallback when the pass dispatches. Release build succeeded and soh.exe was relinked; next step is in-game retest of debug modes 1/2/3 and produced-volume telemetry.
- refs:
  - docs/agents/project_state.md

## [PLN-20260311-0030] DX11 volumetrics truthful debug + light/skylight validation overrides
- createdUtc: 2026-03-11T19:43:11Z
- status: in_progress
- scope: engine
- summary: Combine truthful DX11 volumetrics debug overhaul with global skylight/test-light overrides for validation.
- milestones:
  1. Inspect current DX11 volumetrics debug preview and lighting override hooks;Patch truthful debug telemetry/previews and add custom skylight/test-light menu/runtime overrides;Build Release and validate inspector/output
- tags: volumetrics,dx11,debug,telemetry,lighting,skylight,graphics
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260311-0030][UPDATE] 2026-03-11T21:52:56Z
- status: done
- note: Implemented DX11 volumetrics truthful debug telemetry/preview, added custom skylight and custom volumetric test light graphics overrides, and completed a successful Release build.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/SohGui/SohMenuSettings.cpp
  - x64/Release/soh.exe

## [PLN-20260311-0031] DX11 volumetrics final output and truthful debug fix
- createdUtc: 2026-03-11T22:06:11Z
- status: in_progress
- scope: engine
- summary: Fix DirectX11 volumetrics so producedVolume reflects real final contribution, ensure skylight/test-light reach shader payloads, and make FogDensity/LightEnergy/FinalVolume debug modes preview real buffers instead of proxy composites.
- milestones:
  1. inspect DX11 raymarch/composite outputs and light payload bridge
  2. patch producedMedium/producedFinal stats and real final-volume generation
  3. patch debug previews and validate with Release build plus inspector output
- tags: volumetrics, dx11, debug, telemetry, lighting
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0031][UPDATE] 2026-03-11T22:26:10Z
- status: done
- note: Implemented DX11 volumetrics telemetry semantics, raw debug preview fixes, and directional-light scattering restore; Release build succeeded.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,x64/Release/soh.exe

## [PLN-20260311-0032] Triage no-launch regression after DX11 volumetrics patch
- createdUtc: 2026-03-11T22:52:58Z
- status: in_progress
- scope: engine
- summary: Reproduce and fix the startup blocker where soh.exe no longer opens after recent DX11 volumetrics changes.
- milestones:
  1. 1. collect recent startup/crash evidence and reproduce locally;2. isolate first failing module or initialization path;3. patch blocker and rebuild Release
- tags: volumetrics,dx11,startup,triage
- refs:
  - x64/Release/soh.exe,libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260311-0032][UPDATE] 2026-03-11T23:09:22Z
- status: done
- note: Patched GUI color-picker reset and hardened ConsoleVariable color-block parsing; Release build succeeded and local smoke launch kept soh.exe alive for 5 seconds without new crash logs.
- refs:
  - soh/soh/SohGui/UIWidgets.cpp,libultraship/src/ship/config/ConsoleVariable.cpp,x64/Release/soh.exe,x64/Release/logs/Ship of Harkinian.log

## [PLN-20260312-0001] DX11 volumetrics payload bridge and debug preview fix
- createdUtc: 2026-03-12T14:39:06Z
- status: in_progress
- scope: engine
- summary: Fix DirectX11 volumetrics so runtime light/skylight payloads reach the backend, debug modes preview real buffers, and inspector telemetry reports medium/final output truthfully.
- milestones:
  1. inspect runtime-to-DX11 volumetric light payload bridge and stats thresholds
  2. patch DX11 volumetric light upload/scattering and rewrite debug preview modes
  3. build Release and validate inspector/debug output with runtime docs/checkpoints
- tags: volumetrics, dx11, debug, telemetry, lighting
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0001][UPDATE] 2026-03-12T15:06:20Z
- status: done
- note: Implemented DX11 volumetrics payload bridge fix, debug-preview telemetry cleanup, and built Release soh.exe successfully.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h

## [PLN-20260312-0001][UPDATE] 2026-03-12T15:23:56Z
- status: done
- note: Follow-up fix: UI-selected volumetrics debug mode now overrides preset/custom gating even when custom volumetrics is off; rebuilt Release successfully.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0001][UPDATE] 2026-03-12T17:39:40Z
- status: in_progress
- note: Patched DX11 volumetrics constant-buffer layout by adding 16-byte alignment padding before depthNdcParams; rebuilt Release for retest.
- refs:
  - libultraship/include/fast/backends/gfx_direct3d_common.h

## [PLN-20260312-0002] DX11 volumetrics physical composite and directional shadow map
- createdUtc: 2026-03-12T18:20:52Z
- status: in_progress
- scope: engine
- summary: Implement transmittance-based DX11 volumetrics compositing together with a single directional volumetric shadow caster so fog stops reading as an additive mask and gains real depth/occlusion.
- milestones:
  1. Patch DX11 volumetrics composite/output semantics and runtime stats thresholds
  2. Add one directional volumetric shadow-map path for custom test light/custom skylight/runtime directional light
  3. Validate debug modes, rebuild Release, run docs/runtime checks, and append governance updates
- tags: volumetrics, dx11, lighting, graphics, debug
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0002][UPDATE] 2026-03-12T18:43:35Z
- status: done
- note: Implemented DX11 transmittance volumetrics composite plus directional camera-depth occlusion path, then rebuilt soh Release successfully.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/agents/memory.log

## [PLN-20260312-0003] DX11 volumetrics directional shadow proxy and debug normalization
- createdUtc: 2026-03-12T21:56:51Z
- status: in_progress
- scope: engine
- summary: Implement a DX11 light-space directional shadow proxy from camera depth for volumetrics, normalize debug previews, and validate that final volume gains real depth beyond overlay-like fog.
- milestones:
  1. add DX11 directional shadow-proxy resources/shader and feed the primary directional volumetric caster into light space
  2. wire the proxy into directional volumetric shadowing plus debug mode normalization/telemetry updates
  3. build Release, validate inspector metrics/debug output, and append plan progress
- tags: volumetrics, dx11, lighting, debug, graphics
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0003][UPDATE] 2026-03-12T22:22:21Z
- status: in_progress
- note: Implemented DX11 directional shadow-proxy scaffolding, patched volumetrics shader cbuffers/debug paths, and completed a Release build; awaiting runtime visual validation.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - x64/Release/soh.exe

## [PLN-20260312-0004] DX11 volumetrics depth reconstruction and spatial depth fix
- createdUtc: 2026-03-12T22:44:10Z
- status: in_progress
- scope: engine
- summary: Make DirectX 11 volumetrics look spatial instead of like a 2D overlay by fixing remaining raymarch/composite/shadow depth issues, strengthening medium/final contribution, and validating with Release telemetry.
- milestones:
  1. Inspect remaining DX11 volumetrics visual blockers in raymarch/composite/debug path
  2. Patch backend/runtime so volumetrics gains real spatial depth and stronger final contribution
  3. Rebuild Release, validate inspector/debug output, and record governance updates
- tags: volumetrics, dx11, rendering, graphics
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0005] DX11 volumetrics depth restoration and non-overlay composition
- createdUtc: 2026-03-12T22:45:29Z
- status: in_progress
- scope: engine
- summary: Make DX11 volumetrics look spatial instead of like a 2D filter by restoring physically meaningful depth separation, strengthening final-volume composition, and correcting debug/telemetry to track the real final contribution.
- milestones:
  1. inspect remaining DX11 volumetrics visual limitations in raymarch/composite/debug path
  2. patch shader/backend/runtime so final volume derives from spatial medium and directional lighting with stronger depth separation
  3. rebuild Release, validate inspector/debug output, and append governance updates
- tags: render, volumetrics, dx11, graphics
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/include/fast/backends/gfx_rendering_api.h

## [PLN-20260312-0004][UPDATE] 2026-03-12T22:54:51Z
- status: in_progress
- note: Patched DX11 volumetrics raymarch to keep marching across sky/no-depth pixels out to maxDistance instead of returning zero immediately, then rebuilt Release soh.exe successfully; awaiting in-game validation that the fog now reads as spatial volume rather than a 2D overlay.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe

## [PLN-20260312-0004][UPDATE] 2026-03-12T23:05:33Z
- status: in_progress
- note: Runtime validation exposed a DX11 HLSL compile failure (undeclared ReconstructViewRayDirection) after the no-depth sky-march patch; next step is to fix symbol placement/visibility inside the embedded raymarch shader string, rebuild Release, and revalidate inspector fallback/dispatched state.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/logs/Ship of Harkinian.log

## [PLN-20260312-0004][UPDATE] 2026-03-12T23:11:08Z
- status: in_progress
- note: Patched the DX11 volumetrics raymarch HLSL string so ReconstructViewRayDirection is declared inside the same embedded shader block before PSMain, then rebuilt Release soh.exe successfully. Runtime revalidation in-game is still needed to confirm fallback=none and spatial depth improvement.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe

## [PLN-20260312-0004][UPDATE] 2026-03-12T23:27:34Z
- status: in_progress
- note: Inspector now shows the DX11 volumetrics pass dispatching again, but depth cues remain poor because custom skylight/test-light overrides default to non-shadowed payloads and the medium noise field still includes a camera-relative offset. Next patch will enable directional shadow participation for the custom overrides and world-lock the medium noise field to reduce the 2D veil look.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260312-0004][UPDATE] 2026-03-12T23:31:14Z
- status: in_progress
- note: Enabled directional shadow participation by default for custom skylight and custom test light overrides, removed the camera-relative offset from DX11 volumetric density noise, added a second world-space detail octave, and softened the distance/medium weighting to reduce the uniform 2D veil look; rebuilt Release soh.exe successfully for retest.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe

## [PLN-20260312-0004][UPDATE] 2026-03-13T13:34:20Z
- status: in_progress
- note: DX11 volumetrics now compiles and dispatches, but user validation still shows a veil-like result with weak depth cues. Next patch will strengthen spatial depth using distance-weighted medium contribution, layered world-space variation, and forced directional shadow participation for custom validation lights before another Release rebuild.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T13:53:35Z
- status: in_progress
- note: User validation still shows DX11 volumetrics as a screen-space veil with weak depth. Next patch will inspect raymarch accumulation, transmittance/composite, and debug outputs, then tune spatial falloff and directional shadowing before another Release build.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:10:41Z
- status: in_progress
- note: Patched DX11 volumetrics no-depth handling to estimate sky proxy distance from nearby valid depth samples, clamp sky march distance, and reduce ambient/scattering overhead for upward empty-space rays. Release rebuild succeeded via soh-build-test-windows.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:16:21Z
- status: in_progress
- note: Adjusted DX11 volumetrics no-depth sky handling again: sky proxy distance now samples nearby valid depth more conservatively, horizon fallback distance is shorter, and empty-space rays reduce ambient/scattering much more aggressively. Release rebuild succeeded.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:39:34Z
- status: in_progress
- note: Applying a more conservative confidence-weighted sky proxy distance and much stronger no-depth coverage suppression so FinalVolume stops reading as a full-screen veil in open-sky views.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:42:31Z
- status: in_progress
- note: Tightened DX11 no-depth sky handling again by broadening nearby depth sampling directions, shortening the sky fallback march distance, and sharply suppressing no-depth ambient/scattering coverage. Release rebuild succeeded for another in-game depth validation pass.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:48:24Z
- status: in_progress
- note: User validation still shows FinalVolume dominated by uniform no-depth medium in open-sky views. Next patch will suppress no-depth density earlier in the ray, bias accumulation toward anchored nearby depth, and further reduce sky-only ambient/scattering to restore spatial depth.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T14:54:17Z
- status: in_progress
- note: Further tightened DX11 no-depth volumetrics by capping no-depth march distance, scaling no-depth density much harder by ray progress/horizon/confidence, and gating distance visibility for sky rays. Release rebuild succeeded for another user validation pass.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T15:05:44Z
- status: in_progress
- note: Applying another DX11 volumetrics tuning pass focused on stronger far-only accumulation for no-depth rays, harsher zenith suppression, and reduced no-light sky fill so the normal render reads as spatial fog instead of a screen veil.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T15:10:13Z
- status: in_progress
- note: Further reduced the no-depth veil by shortening no-depth max march distance, delaying no-depth density accumulation until late ray progress, hardening zenith suppression, and cutting ambient/scattering when no light payload exists. Release rebuild succeeded for retest.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T15:26:14Z
- status: in_progress
- note: User retest still shows FogDensity dominated by broad no-depth sky fill. Next patch will anchor no-depth contribution much tighter to the horizon, require stronger nearby-depth confidence, and slash no-light sky medium so open-sky views stop reading as a full-screen veil.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T15:44:37Z
- status: in_progress
- note: Tightened no-depth contribution again: no-depth march distance now tops out sooner, horizon/confidence gating is much stricter, no-depth density only ramps very late in the ray, and no-light sky fill is heavily reduced. Release rebuild succeeded for another in-game validation pass.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T16:40:20Z
- status: in_progress
- note: User retest still shows FogDensity covering nearly the full screen when payloadLights=0 and no-depth sky dominates. Next patch will both suppress no-depth accumulation harder when no light payload exists and make the DX11 debug normalization use avg-vs-max contrast instead of bright full-screen fill.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T17:19:29Z
- status: in_progress
- note: Patched DX11 volumetrics to use harsher no-depth horizon/confidence gating, much lower no-light sky suppression, and avg-vs-max debug normalization so FogDensity/FinalVolume stop reading as full-screen bright fill. libultraship compiled successfully, but final Release relink was blocked because x64/Release/soh.exe was locked by a running game process.
- refs:
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe

## [PLN-20260312-0004][UPDATE] 2026-03-13T17:40:51Z
- status: in_progress
- note: Next DX11 volumetrics pass rejects sky-like far depth as no-depth input so horizon suppression can actually apply before another Release rebuild.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/agents/Plans.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T17:46:31Z
- status: in_progress
- note: Patched DX11 volumetrics raymarch to treat far-plane sky-like depth as no-depth, so horizon/no-depth suppression can apply to empty-space pixels before the next rebuild.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/agents/Plans.md

## [PLN-20260312-0004][UPDATE] 2026-03-13T17:57:17Z
- status: in_progress
- note: Patched DX11 volumetrics to classify sky-like far-plane depth as no-depth before raymarch, then rebuilt Release successfully for retest.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - x64/Release/soh.exe
  - docs/agents/Plans.md

## [PLN-20260312-0004][UPDATE] 2026-03-14T12:16:52Z
- status: in_progress
- note: User validation still shows no real distance falloff: signs/houses/tree remain too readable while FogDensity stays broad and uniform. Next patch will add scene-depth-driven aerial perspective/extinction for valid surface-depth pixels and further demote no-depth sky fill so far geometry fades with distance even when light payload is absent.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-14T12:29:55Z
- status: planned
- note: User validation still shows little real aerial perspective: distant houses/tree/sign remain readable while FogDensity fills broad areas. Next patch will strengthen valid-surface depth extinction/transmittance and add stronger depth-driven distance haze so normal render loses contrast with distance, not only debug.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-14T13:31:57Z
- status: planned
- note: User asked to continue volumetric VFX implementation. Next patch will add more world-space layered noise, ground-hugging pockets, macro billows, and horizon banding so the fog reads as volume in the world instead of uniform screen haze.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-14T14:02:39Z
- status: planned
- note: User asked for more graphics implementations. Next patch will add new graphics-side volumetric tuning controls and stronger renderer-side support for aerial perspective / world-space fog VFX so the feature set expands beyond baseline fog debug fixes.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-03-14T14:13:27Z
- status: in_progress
- note: Continuei a implementação dos gráficos DX11 com foco em profundidade espacial do volumetric fog e em novos controles de tuning no renderer/menu.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_rendering_api.h
  - soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260312-0004][UPDATE] 2026-03-14T16:21:41Z
- status: in_progress
- note: Continuei a implementação dos gráficos DX11 reforçando o composite full-resolution de aerial perspective para que geometria distante perca contraste e legibilidade com a distância, reduzindo a aparência de filtro 2D no volumetric fog.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260312-0004][UPDATE] 2026-03-14T16:30:18Z
- status: in_progress
- note: Added stronger full-resolution depth haze in the DX11 volumetrics composite so distant geometry should lose contrast faster based on scene depth, then rebuilt Release successfully.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,x64/Release/soh.exe

## [PLN-20260312-0004][UPDATE] 2026-03-14T16:37:07Z
- status: in_progress
- note: Continuei a implementação do volumetric fog DX11 reforçando a extinção por distância no raymarch e no composite para que geometria distante fique significativamente menos legível e o mapa ganhe mais profundidade atmosférica.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260312-0004][UPDATE] 2026-03-14T17:07:56Z
- status: in_progress
- note: Continuando a rodada DX11 de fog volumetrico com mais implementacoes de graficos: novos controles de extincao/sombra, debug modes de transmittance e shadow occlusion, e mais telemetria do inspector.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h,libultraship/include/fast/backends/gfx_direct3d_common.h,libultraship/src/fast/backends/gfx_direct3d11.cpp,soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp,soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260312-0004][UPDATE] 2026-03-14T17:39:57Z
- status: in_progress
- note: Starting DX11 volumetrics quality batch: add transmittance/shadow debug modes, new depth-extinction controls, directional shadow tuning, and runtime/inspector stats for depth-first fog validation.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/interpreter.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260321-0001] Player modern collision framework sidecar
- createdUtc: 2026-03-21T01:46:38Z
- status: in_progress
- scope: engine
- summary: Add a player-only modern collision sidecar framework behind a CVar while preserving the vanilla bgcheck backend intact and dormant when disabled.
- milestones:
  1. add sidecar collision framework module and types
  2. dispatch player bgcheck through CVar-gated framework or vanilla path
  3. add ModernPlayerCollision enhancement toggle and validate build
- tags: collision, player, framework, enhancements
- refs:
  - soh/src/code/z_player_collision_framework.c
  - soh/include/z64player_collision_framework.h
  - soh/src/overlays/actors/ovl_player_actor/z_player.c
  - soh/soh/SohGui/SohMenuEnhancements.cpp

## [PLN-20260407-0001] API v4 native runtime foundation
- createdUtc: 2026-04-07T16:25:24Z
- status: in_progress
- scope: mixed
- summary: Add the first Windows-first native runtime foundation for API v4 external mods with manifest/runtime contract extensions, permission-gated DLL loading, tooling/template support, and UI/runtime diagnostics while keeping wasm mods compatible.
- milestones:
  1. Define native runtime ABI/runtime contract and parse native-cpp-v1|hybrid-v1 manifests,Load native DLL runtimes with permission checks and lifecycle hooks,Ship tooling/docs/template coverage and verify with target soh build plus validator smoke
- tags: external-mods,native-runtime,api-v4,tooling
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModNativeRuntime.cpp,tools/external_mods/modtool.ps1,docs/runtime_contract/mod.manifest.v2.json

## [PLN-20260407-0001][UPDATE] 2026-04-07T16:54:21Z
- status: in_progress
- note: Implemented the assets.raw.v1 tranche for API v4 external mods: added manifest/runtime parsing for assetSourceDefinitions, prepared host-access loose assets for native/hybrid runtimes, extended the native host API with getAssetPath/getAssetInfo, refreshed the native template with a sample payload, regenerated runtime reference docs, validated the generated sample mod, and rebuilt Release soh.exe successfully.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModNativeRuntime.cpp,tools/external_mods/templates/native_mod/mod.json,docs/runtime_contract/security.capabilities.v2.json,x64/Release/soh.exe

## [PLN-20260407-0001][UPDATE] 2026-04-07T17:05:02Z
- status: in_progress
- note: Starting next implementation tranche: add render.meshes.v1 plus actors.prefabs.v1 foundations on top of assets.raw.v1 so external mods can register reusable meshes and spawn simple visual prefabs/props from JSON instead of only resolving host-access file paths.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,docs/runtime_contract/security.capabilities.v2.json

## [PLN-20260407-0001][UPDATE] 2026-04-07T17:20:35Z
- status: done
- note: Completed the next external-mod tranche: added render.meshes.v1, actors.prefabs.v1, and world.instances.v1 with manifest/runtime parsing, mesh model asset loading via the existing OBJ/Fast64/archive pipeline, in-game drawing for prefab-backed actor/world visuals, tooling/docs updates, and a lanterna_test demo exercising assets.raw + meshes + prefabs + placed visuals.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,docs/examples/external_mods/lanterna_test/mod.json,docs/runtime_contract/security.capabilities.v2.json

## [PLN-20260407-0002] External Mods v4 follow-up: world authoring/editor runtime plus backlog for animation and Workbench
- createdUtc: 2026-04-07T21:18:56Z
- status: in_progress
- scope: mixed
- summary: Implement the next shippable tranche on top of meshes/prefabs: make world.authoring.v1 and editor.placement.v1 execute real prefab placement/library behavior, then capture the remaining larger gaps as a follow-up plan for skeletons, animations, advanced prefabs, Workbench, and in-game editor tooling.
- milestones:
  1. Implement real schemas/runtime for world.authoring.v1 and editor.placement.v1
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,docs/runtime_contract/mod.manifest.v2.json,docs/runtime_contract/security.capabilities.v2.json

## [PLN-20260407-0002][UPDATE] 2026-04-07T21:28:57Z
- status: done
- note: Implemented runtime-backed editor.placement.v1 and world.authoring.v1 on top of the mesh/prefab/world instance slice: added schema fields, parsers, package load paths, in-game draw resolution via placement libraries, UI counts, docs updates, and a validated lanterna_test demo with placement and authoring JSON. Build and validate both passed.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,soh/soh/Enhancements/external-mods/ExternalModUi.cpp,docs/examples/external_mods/lanterna_test/mod.json

## [PLN-20260407-0003] External Mods v4 backlog: animation pipeline, rich prefabs, world tooling, and editors
- createdUtc: 2026-04-07T21:29:15Z
- status: planned
- scope: mixed
- summary: Deliver the remaining large pieces of the API v4 external-mod plan after meshes/prefabs/world authoring: render.skeletons.v1, render.animations.v1, animation graphs, richer actor prefabs/components/collision, expanded world authoring/triggers/encounters, Foundry Workbench, Live Link, and advanced in-game editor tooling.
- milestones:
  1. Ship render.skeletons.v1 plus render.animations.v1 with external clips and runtime playback hooks.
  2. Expand actors.prefabs.v1 into richer prefab/component data with collider, hitbox, sensor, and state-machine support.
  3. Extend world authoring into triggers, encounters, patchsets, and richer placement/edit metadata.
  4. Create Foundry Workbench Windows app skeleton with project explorer, schema forms, asset import, and pack/doctor flows.
  5. Add Live Link and advanced in-game editor overlays/gizmos/selection for round-trip authoring.
- tags: external-mods, api-v4, animation, editor, workbench
- refs:
  - docs/runtime_contract/mod.manifest.v2.json
  - docs/runtime_contract/security.capabilities.v2.json
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260407-0004] External mods catalog consolidation to framework-first v4 topology
- createdUtc: 2026-04-07T22:27:49Z
- status: in_progress
- scope: mixed
- summary: Consolidate the examples catalog, templates, sync tooling, validation, and framework wiki around six canonical framework-first API v4 packages with hybrid/native support, explicit manifest metadata, and capability coverage across canonical consumer demos.
- milestones:
  1. Create six canonical framework packages and canonical consumer demos under docs/examples/external_mods.
  2. Normalize manifests/templates/tooling to framework-first metadata, hybrid defaults, and dependency id/versionRange shape.
  3. Auto-discover examples in sync tooling and enforce structural checks in doctor/validate/report flows.
  4. Rewrite examples README and framework wiki to the consolidated topology and remove legacy catalog references.
  5. Run validate/doctor/report/report-native and Release build until the consolidated suite passes.
- tags: external-mods, framework-first, native, tooling, docs
- refs:
  - docs/examples/external_mods
  - tools/external_mods/modtool.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1
  - docs/framework_wiki/README.md

## [PLN-20260407-0004][UPDATE] 2026-04-07T22:53:58Z
- status: done
- note: Canonical framework-first catalog finalized: 6 hybrid frameworks, 6 consumer demos, 5 reference packs, tooling/docs migration, hybrid DLL builds, and zero-warning report-native/doctor on docs/examples/external_mods.
- refs:
  - docs/examples/external_mods/README.md
  - tools/external_mods/modtool.ps1
  - tools/external_mods/validate_mod.ps1
  - tools/external_mods/sync_examples_to_runtime.ps1

## [PLN-20260410-0001] External mods v4 foundation for split data/runtime layout and manifest v3
- createdUtc: 2026-04-10T17:54:39Z
- status: in_progress
- scope: mixed
- summary: Implement the first executable tranche of the free data-driven external-mod plan: add mod.manifest.v3, split mods_data/mods_runtime discovery, sharePolicy/imports/exports/serviceEndpoints parsing, validation/tooling support, and example catalog migration seeds without changing apiVersion 4.
- milestones:
  1. add manifest v3 contract plus parser/types for split layout and share policy,teach loader and sync tooling to discover paired mods_data/mods_runtime roots,extend modtool/validate to scaffold, validate, doctor, and pack the split layout,add one migrated example seed and regenerate runtime docs/contracts
- tags: external-mods,api-v4,data-driven,tooling,runtime
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModTypes.h,tools/external_mods/modtool.ps1,tools/external_mods/validate_mod.ps1,tools/external_mods/sync_examples_to_runtime.ps1,docs/runtime_contract/mod.manifest.v2.json

## [PLN-20260410-0001][UPDATE] 2026-04-10T18:22:55Z
- status: done
- note: Implemented the foundation tranche for the API v4 free-form mod ecosystem: manifest v3 parsing, split mods_data/mods_runtime discovery, data-only runtime kind, convention-based capability binding, generic DSL document loading, split-aware validation/pack/sync tooling, updated capability docs, and a runnable free_mod_v3_seed split example. Verified validate_mod, modtool validate/doctor/pack, sync_examples_to_runtime, and a successful Release build of soh.exe.
- refs:
  - docs/runtime_contract/mod.manifest.v3.json,tools/external_mods/validate_mod.ps1,tools/external_mods/modtool.ps1,tools/external_mods/sync_examples_to_runtime.ps1,docs/examples/external_mods/free_mod_v3_seed/data/mod.json,soh/soh/Enhancements/external-mods/ExternalModManager.cpp

## [PLN-20260410-0001][UPDATE] 2026-04-10T22:49:43Z
- status: done
- note: Implemented the next shared-surface tranche on top of manifest v3: sharePolicy-backed cross-mod read enforcement in ExternalModManager, public JSON query/service APIs for both native SDK and WASM runtimes, synced SDK headers across canonical framework/demo copies, updated the native smoke sample to consume the new query/service bus, and refreshed the split seed/docs to describe the service lookup contract. Verified the sample native plugin rebuild, modtool validate/doctor, sync_examples_to_runtime, and a successful Release build of soh.exe.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModNativeApi.h,soh/soh/Enhancements/external-mods/ExternalModNativeRuntime.cpp,soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp,docs/examples/external_mods/demo_devtools_native_smoke/native/plugin/src/plugin.cpp,docs/examples/external_mods/free_mod_v3_seed/data/services/lookup.json

## [PLN-20260410-0001][UPDATE] 2026-04-10T23:16:00Z
- status: in_progress
- note: Starting the next host-surface tranche on top of manifest v3: expose own-mod `invokeActionJson` to WASM and native SDK, expand `queryPublicJson` beyond metadata into runtime/settings/world/narrative/storage/inventory state, and update the canonical native smoke sample/docs so data-only, WASM, and native mods all consume the same public query/action layer.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModNativeApi.h,soh/soh/Enhancements/external-mods/ExternalModNativeRuntime.cpp,soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp,docs/examples/external_mods/demo_devtools_native_smoke/native/plugin/src/plugin.cpp,docs/examples/external_mods/README.md

## [PLN-20260410-0001][UPDATE] 2026-04-10T23:37:07Z
- status: done
- note: Completed the shared gameplay host-surface tranche for the split v3 ecosystem: `queryPublicJson` now exposes runtime/settings/world/narrative/storage/inventory state under sharePolicy read rules, `invokeActionJson` executes own-mod action payloads through the existing action parser/executor, both surfaces are bound in WASM and native SDK runtimes, and the canonical native smoke sample now exercises runtime query, public service lookup, and host action invocation. Verified native sample rebuild, validate_mod, catalog-wide modtool validate/doctor, sync_examples_to_runtime, and a successful Release rebuild of `soh.exe`.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp,soh/soh/Enhancements/external-mods/ExternalModManager.h,soh/soh/Enhancements/external-mods/ExternalModNativeApi.h,soh/soh/Enhancements/external-mods/ExternalModNativeRuntime.cpp,soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp,docs/examples/external_mods/demo_devtools_native_smoke/native/plugin/src/plugin.cpp,docs/examples/external_mods/free_mod_v3_seed/data/services/lookup.json

## [PLN-20260411-0001] External mods v4 entity-driven DSL composition for world placement
- createdUtc: 2026-04-11T13:48:54Z
- status: in_progress
- scope: mixed
- summary: Add the first executable DSL composition slice on top of split manifest v3 by allowing `components` and `entities` documents to drive real visual placement through `entityId` in world instances, editor placements, and world authoring, synthesizing prefab bindings during load instead of requiring every placement to hardcode prefab ids.
- milestones:
  1. Extend world/placement parsers and runtime types to accept `entityId` alongside `prefabId`.
  2. Resolve `component.visual -> entity -> prefab` during package load, including synthesized prefab definitions from component mesh-backed visuals.
  3. Update `free_mod_v3_seed` and docs to use entity-driven placement end-to-end, then validate and rebuild.
- tags: external-mods,api-v4,data-driven,dsl,world,render
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/free_mod_v3_seed/data
  - docs/runtime_contract/mod.manifest.v3.json

## [PLN-20260411-0001][UPDATE] 2026-04-11T13:55:14Z
- status: done
- note: Implemented the first executable entity-driven DSL composition slice for the split v3 mod ecosystem: `world.instances`, `editor.placement`, and `world.authoring` now accept `entityId`, the loader resolves `component.visual -> entity -> prefab` bindings during package load, visual components can synthesize prefab definitions directly from `meshId` plus transform/lod/billboard, and `free_mod_v3_seed` now places its lantern through entity references instead of hard-coded prefab ids at each placement site. Verified seed validate/validate_mod, catalog-wide validate/doctor, sync_examples_to_runtime, and a successful Release rebuild of `soh.exe`.
- refs:
  - soh/soh/Enhancements/external-mods/ExternalModManager.cpp
  - soh/soh/Enhancements/external-mods/ExternalModTypes.h
  - docs/examples/external_mods/free_mod_v3_seed/data/components/visual_lantern.json
  - docs/examples/external_mods/free_mod_v3_seed/data/world/instances.json
  - docs/examples/external_mods/free_mod_v3_seed/data/world/placement.json
  - docs/runtime_contract/mod.manifest.v3.json

## [PLN-20260312-0004][UPDATE] 2026-05-11T03:34:00Z
- status: in_progress
- note: Continuing DX11 volumetrics depth batch: implement light-space directional shadow map, depth-driven transmittance/composite, and new DX11 debug/inspector controls for spatial fog validation.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/src/fast/interpreter.cpp
  - libultraship/include/fast/interpreter.h
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260312-0004][UPDATE] 2026-05-11T11:20:27Z
- status: in_progress
- note: Release build passed after adding stronger depth-driven composite extinction and optical-depth-informed haze for distant geometry in DX11 volumetrics.
- refs:
  - docs/agents/project_state.md

## [PLN-20260312-0004][UPDATE] 2026-05-11T16:45:01Z
- status: in_progress
- note: User reported Custom Volumetrics and Volumetrics Debug View dropping the game to around 1 FPS. Prioritizing removal of synchronous metrics readback/stalls and then continuing graphics improvements.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260312-0004][UPDATE] 2026-05-11T17:05:56Z
- status: in_progress
- note: Metrics are now consumed with DO_NOT_WAIT from a pending staging copy and refreshed on a throttle; inspector/debug scales reuse the last valid stats instead of blocking the GPU every frame. Release build succeeded.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp,libultraship/include/fast/backends/gfx_direct3d_common.h

## [PLN-20260312-0004][UPDATE] 2026-05-11T17:13:28Z
- status: in_progress
- note: Quality 3 now clamps the internal low-res buffer by pixel budget on large windows, and debug preview quality 3 uses a lower step count; Release build succeeded again.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp

## [PLN-20260312-0004][UPDATE] 2026-05-11T19:08:06Z
- status: in_progress
- note: Starting performance-mode DX11 volumetrics patch: split cheap depth fog/aerial perspective from expensive raymarch, cap debug/internal budgets, expose perf/shadow telemetry, and keep debug from dropping to 1 FPS.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - libultraship/include/fast/backends/gfx_rendering_api.h
  - soh/soh/SohGui/SohMenuSettings.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp

## [PLN-20260312-0004][UPDATE] 2026-05-11T21:09:56Z
- status: done
- note: Implemented DX11 volumetrics performance mode, capped raymarch/debug buffers, depth-fog-only path when no light payload, inspector fields, menu performance presets, and validated Release build of soh.exe.
- refs:
  - libultraship/include/fast/backends/gfx_rendering_api.h
  - libultraship/include/fast/backends/gfx_direct3d_common.h
  - libultraship/include/fast/interpreter.h
  - libultraship/src/fast/interpreter.cpp
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
  - soh/soh/SohGui/SohMenuSettings.cpp

## [PLN-20260312-0004][UPDATE] 2026-05-11T21:23:19Z
- status: done
- note: Post-build cleanup: removed DX11 depth conversion warning, reran diff checks, and rebuilt Release soh.exe successfully.
- refs:
  - libultraship/src/fast/backends/gfx_direct3d11.cpp
  - docs/agents/Plans.md

## [PLN-20260225-0014][UPDATE] 2026-07-11T18:16:35Z
- status: in_progress
- note: Retomada integral do master plan apos trabalho paralelo: auditar M3-M12 contra a arvore viva, preservar mudancas existentes, completar lacunas reais e fechar validacao/build.
- refs:
  - plan.md,soh/soh/Enhancements/external-mods/ExternalModManager.cpp,tools/external_mods/validate_mod.ps1,docs/examples/external_mods

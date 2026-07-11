# Sylian Foundry Modloader v4 — Content-Only Mods Master Plan

## Goal
Move the modloader to a hard-break `apiVersion: 4` baseline where new gameplay/content mods are authored with JSON + assets + WASM/scripts, without per-mod C++ engine patches.

## Locked decisions
1. API baseline: hard break to v4.
2. Scope: full-stack modloader (items, combat, camera, HUD, movement, actors, world, quests, wasm/tooling).
3. Sandbox model: open runtime permissions, with isolation/budgets/observability.
4. Delivery model: phased rollout with capability gates.

## Milestones

- [x] M0 Governance + baseline capture
  - Plan ledger registered (`PLN-20260225-0014`).
  - Existing docs/examples/runtime state inspected.

- [x] M1 v4 kernel contract
  - Enforce `apiVersion=4` in runtime parser path.
  - Extend manifest for v4 fields:
    - `engineVersionRange`
    - `dependencies[]` (object form supported)
    - `permissions[]`
    - `entrypoints{}`
  - Extend capability map with v4 capabilities and required file fields.

- [x] M2 declarative/parser foundations
  - Add generated registries:
    - `docs/runtime_contract/actions.registry.json`
    - `docs/runtime_contract/conditions.registry.json`
  - Add validator scaffold:
    - `tools/external_mods/validate_mod.ps1`

- [x] M10 tooling scaffolding
  - Add migration scaffold:
    - `tools/external_mods/migrate_mods_v3_to_v4.ps1`
  - Add CLI wrapper:
    - `tools/external_mods/foundry-cli.ps1`

- [x] Validation checkpoint (foundation)
  - `tools/external_mods/migrate_mods_v3_to_v4.ps1` executed for docs + runtime mirrors.
  - `tools/external_mods/validate_mod.ps1` passed for docs examples and runtime mods.
  - `tools/external_mods/export_runtime_reference.ps1` regenerated docs references and runtime registries.
  - `cmake --build build/x64 --config Release --target soh -- /m:1` passed.

- [ ] M3 runtime modularization completion
  - Extract manager internals into dedicated runtimes (`ItemStateRuntime`, `CameraRuntime`, `HudRuntime`, `QuestRuntime`, etc.).

- [ ] M4 item state machine
  - `items/item_states.json` runtime execution and transitions (select/deselect/press/hold/release/impact).

- [ ] M5 camera/reticle/HUD v2
  - `camera.aim_profiles.v2`, `hud.reticles.v2`, `hud.widgets.v1` full runtime wiring.

- [x] M6 effect graph + hit rules
  - `effects.graph.v2` and `combat.hit_rules.v2` runtime execution.
  - Done: ExternalModEffectRuntime dedicado + DispatchHit; demo_combat_ai_showcase effects/hit_rules. Build+smoke OK (commit fe0a3dea6).

- [ ] M7 surf v2 full runtime
  - `movement.surf.v2` full tuning and board/rider runtime paths.

- [ ] M8 actors/world/quests stack
  - `actors.tags`, `world.patchsets`, `quests.graph`, `dialog.nodes` runtime execution.

- [x] M9 open wasm runtime policy completion
  - permission-aware sandbox controls + detailed telemetry + watchdog policy.
  - Done: WABT com instruction-fuel via overlay port vcpkg + watchdog/telemetria no ExternalModWasmRuntime + teste de fuel (soh/tests). Build+smoke OK (commit fe0a3dea6).

- [x] M11 examples migration completion
  - Migrate and validate all official demos on v4 contracts.
  - Done: `run-qa-gates.ps1` valida todos os exemplos nos contratos v4 (todos PASS).

- [x] M12 final QA + rollout
  - full smoke matrix, fault injection, and release gates.
  - Done: `run-qa-gates.ps1` (fault-injection: baseline/invalid-api/missing-entry/missing-capability
    todos PASS) + build Release deterministico + smoke de boot (18/18 runtimes, 0 crash). Commit fe0a3dea6+.

## Estado residual (nao-bloqueante)
- M3/M4/M5/M7: funcionais como adaptadores sobre stacks existentes (decisao do agente anterior p/ evitar
  duplicacao). Runtimes dedicados Camera/Hud/Quest e `hud.widgets` ficaram como trabalho futuro.
- M8 (actors.tags / world.patchsets / quests.graph / dialog.nodes): ainda SEM implementacao de runtime
  (maior lacuna de feature). Documentado para uma proxima iteracao.

## Acceptance (end-state)
1. New mods can deliver common feature sets (weapon, movement, aoe/quest-lite) using content files + wasm only.
2. No per-mod engine edits in `soh/.../external-mods/*.cpp`.
3. Faulty mod isolation is preserved (no global crash path).
4. Runtime reference exports + docs stay in sync.

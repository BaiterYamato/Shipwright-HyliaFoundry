# Context Pack
generated=2026-03-10T19:54:36.2404447-03:00
maxFiles=8 maxSnippetsPerFile=3 snippetLines=220

## file: docs/agents/project_state.md
```text
# Project State Snapshot

Last updated: 2026-03-03T13:45:00Z

## Current Governance and Architecture State

- External Mods baseline for active development is API v4 (`apiVersion: 4`).
- Sylian Foundry Modloader docs are now organized around `docs/SYLIAN_FOUNDRY_MODLOADER.md`.
- Runtime reference exports remain:
  - `docs/catalogs.json`
  - `docs/actions.json`
  - `docs/events.json`
- Runtime contract registries are exported under:
  - `docs/runtime_contract/actions.registry.json`
  - `docs/runtime_contract/conditions.registry.json`
- Example mods source of truth is `docs/examples/external_mods` with runtime mirror in `x64/Release/mods`.
- Agent governance is repository-local under `docs/agents`.
- Skill Pack mirror under `docs/agents/skills` currently has **17 skills**.
- Plans ledger is append-only at `docs/agents/Plans.md`.
- Future-version planning tracks live under `docs/agents/plan_tracks` (`v5`..`v10`) and are linked from execution plans.

## Active Decisions

1. Every implementation must register a plan in `Plans.md` before code changes.
2. Plan progress/completion is append-only via `[UPDATE]`; existing entries are immutable.
3. `plan_tracks/v*` files are companion roadmaps only; they never replace execution entries in `Plans.md`.
4. Memory policy is append-only with `summary <= 180` and compaction threshold `200 entries / 60 days`.
5. Commit discipline follows `soh-git-checkpoint-merge` checkpoints (feature/build/finalize).
6. Doc/runtime drift checks are mandatory; current policy remains soft gate for unrelated urgent fixes.
7. Contract changes must regenerate runtime reference exports with `tools/external_mods/export_runtime_reference.ps1`.
8. API v4 migration scaffolding is active via `tools/external_mods/migrate_mods_v3_to_v4.ps1` and `tools/external_mods/validate_mod.ps1`.

## Open Risks

1. Local finalize merge can remain blocked when repository cleanliness fails (recently observed with dirty `libultraship` submodule state).
2. Duplicate/overlapping plan ids for same scope can cause governance confusion if not superseded explicitly.
3. Demo/runtime sync can drift if `sync_examples_to_runtime.ps1` is skipped after demo edits.
4. API v4 implementation is foundational (M0-M2/tooling); runtime modularization milestones M3+ remain open.
5. Some gameplay behaviors still require in-game validation beyond build/static checks.

## Near-Term Milestones

1. Keep AGENTS protocol aligned with real scripts and current skill inventory.
2. Maintain canonical Sylian Foundry Modloader docs and references without drift.
3. Keep demo sync and runtime reference exports up to date after contract-affecting changes.
4. Preserve memory/index integrity (`rebuild-index` + `validate-memory`) after memory operations.
5. Preserve strict plan-first workflow for all new implementation scopes.
6. Advance v4 milestones M3+ (runtime modularization, item state machine, camera/hud/effects graphs).
7. Execute v7 world-graphics contract rollout with reference suite frameworks/demos.
8. Harden docs retrieval and plan-memory reconciliation via new critical skills and memory pipeline tooling.

## Out of Scope (Current Snapshot)

- NPC/quest/story runtime expansion remains outside current phase baseline.
```

## file: docs/agents/memory.index.json
```text
{
    "version":  2,
    "schemaVersion":  2,
    "lastUpdatedUtc":  "2026-03-10T22:54:19Z",
    "stats":  {
                  "entryCount":  172,
                  "openCount":  72,
                  "doneCount":  99,
                  "deprecatedCount":  1,
                  "topicCount":  80,
                  "tagCount":  72
              },
    "consistency":  {
                        "duplicateIds":  [

                                         ],
                        "danglingSupersedes":  [

                                               ],
                        "danglingPlanTopics":  [

                                               ]
                    },
    "byId":  {
                 "mem-20260304-000013":  {
                                             "topic":  "tooling:modtool-report-mutation-loss",
                                             "status":  "done",
                                             "line":  130,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000015":  {
                                             "topic":  "external-mods-pistol-reticle-axe-prop",
                                             "status":  "done",
                                             "line":  15,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000024":  {
                                             "topic":  "plan:PLN-20260225-0007",
                                             "status":  "done",
                                             "line":  24,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000005":  {
                                             "topic":  "docs-consistency",
                                             "status":  "open",
                                             "line":  5,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000039":  {
                                             "topic":  "plan:PLN-20260303-0007",
                                             "status":  "open",
                                             "line":  104,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000019":  {
                                             "topic":  "plan:PLN-20260225-0009",
                                             "status":  "done",
                                             "line":  84,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000026":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  166,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000028":  {
                                             "topic":  "plan:PLN-20260303-0004",
                                             "status":  "open",
                                             "line":  93,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000043":  {
                                             "topic":  "plan:PLN-20260225-0013",
                                             "status":  "open",
                                             "line":  43,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000025":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  165,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000035":  {
                                             "topic":  "plan:PLN-20260225-0011",
                                             "status":  "done",
                                             "line":  35,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000014":  {
                                             "topic":  "plan:PLN-20260225-0004",
                                             "status":  "done",
                                             "line":  14,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000047":  {
                                             "topic":  "git-finalize-blocked",
                                             "status":  "open",
                                             "line":  47,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000014":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  154,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000030":  {
                                             "topic":  "external-mods:v6-core-reference-suite",
                                             "status":  "done",
                                             "line":  95,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000008":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  125,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000006":  {
                                             "topic":  "plan:PLN-20260303-0003",
                                             "status":  "open",
                                             "line":  71,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000018":  {
                                             "topic":  "plan:PLN-20260225-0010",
                                             "status":  "done",
                                             "line":  83,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000027":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  167,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000001":  {
                                             "topic":  "external-mods-phase1",
                                             "status":  "done",
                                             "line":  1,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000034":  {
                                             "topic":  "plan:PLN-20260225-0011",
                                             "status":  "open",
                                             "line":  34,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000009":  {
                                             "topic":  "plan:PLN-20260225-0002",
                                             "status":  "done",
                                             "line":  9,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000027":  {
                                             "topic":  "plan:PLN-20260303-0003",
                                             "status":  "done",
                                             "line":  92,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000038":  {
                                             "topic":  "plan:PLN-20260303-0007",
                                             "status":  "open",
                                             "line":  103,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000015":  {
                                             "topic":  "plan:PLN-20260225-0012",
                                             "status":  "done",
                                             "line":  80,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000023":  {
                                             "topic":  "docs-runtime-drift-backlog",
                                             "status":  "open",
                                             "line":  23,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000011":  {
                                             "topic":  "external-mods-v3-gap-closure",
                                             "status":  "done",
                                             "line":  11,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000004":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  121,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000014":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  131,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000051":  {
                                             "topic":  "render:ssao-dx11-crash-on-enable",
                                             "status":  "done",
                                             "line":  116,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000005":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  145,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000019":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  159,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000031":  {
                                             "topic":  "plan:PLN-20260225-0010",
                                             "status":  "open",
                                             "line":  31,
```
```text
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000029":  {
                                             "topic":  "external-mods:reticle-selected-in-hand-gate",
                                             "status":  "done",
                                             "line":  29,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000053":  {
                                             "topic":  "external-mods:freeze-ice-trap-no-damage-runtime",
                                             "status":  "done",
                                             "line":  53,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000013":  {
                                             "topic":  "plan:PLN-20260225-0004",
                                             "status":  "done",
                                             "line":  13,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000005":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  122,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000047":  {
                                             "topic":  "plan:PLN-20260303-0010",
                                             "status":  "done",
                                             "line":  112,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000032":  {
                                             "topic":  "plan:PLN-20260310-0007",
                                             "status":  "open",
                                             "line":  172,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000038":  {
                                             "topic":  "docs:sylian-foundry-modloader-refresh",
                                             "status":  "done",
                                             "line":  38,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000013":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  153,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000018":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  158,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000018":  {
                                             "topic":  "external-mods:v10-permission-modal-prompt",
                                             "status":  "done",
                                             "line":  135,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000009":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  149,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000037":  {
                                             "topic":  "plan:PLN-20260303-0006",
                                             "status":  "done",
                                             "line":  102,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000025":  {
                                             "topic":  "plan:PLN-20260225-0008",
                                             "status":  "open",
                                             "line":  25,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000004":  {
                                             "topic":  "memory-operations",
                                             "status":  "open",
                                             "line":  4,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000020":  {
                                             "topic":  "plan:PLN-20260225-0008",
                                             "status":  "done",
                                             "line":  85,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000007":  {
                                             "topic":  "plan:PLN-20260225-0001",
                                             "status":  "open",
                                             "line":  7,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000011":  {
                                             "topic":  "external-mods:v10-permission-ui-and-contract-docs",
                                             "status":  "done",
                                             "line":  128,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000052":  {
                                             "topic":  "plan:PLN-20260303-0012",
                                             "status":  "open",
                                             "line":  117,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000019":  {
                                             "topic":  "external-mods:v10-runtime-failure-quarantine",
                                             "status":  "done",
                                             "line":  136,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000006":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  123,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000021":  {
                                             "topic":  "plan:PLN-20260225-0007",
                                             "status":  "done",
                                             "line":  86,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000045":  {
                                             "topic":  "plan:PLN-20260303-0009",
                                             "status":  "open",
                                             "line":  110,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000013":  {
                                             "topic":  "plan:PLN-20260225-0015",
                                             "status":  "done",
                                             "line":  78,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000001":  {
                                             "topic":  "plan:PLN-20260304-0001",
                                             "status":  "open",
                                             "line":  118,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000015":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  155,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000030":  {
                                             "topic":  "plan:PLN-20260225-0009",
                                             "status":  "done",
                                             "line":  30,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000048":  {
                                             "topic":  "render:ssao-v71",
                                             "status":  "done",
                                             "line":  113,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000003":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  143,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000012":  {
                                             "topic":  "plan:PLN-20260225-0016",
                                             "status":  "done",
                                             "line":  77,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000041":  {
                                             "topic":  "plan:PLN-20260303-0008",
                                             "status":  "open",
                                             "line":  106,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000059":  {
                                             "topic":  "plan:PLN-20260225-0017",
                                             "status":  "done",
                                             "line":  59,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000011":  {
                                             "topic":  "plan:PLN-20260225-0017",
                                             "status":  "done",
                                             "line":  76,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000021":  {
                                             "topic":  "tooling:modtool-pack-nondeterministic-lockfile",
                                             "status":  "done",
                                             "line":  138,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000036":  {
                                             "topic":  "plan:PLN-20260225-0011-finalize",
                                             "status":  "open",
                                             "line":  36,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000049":  {
                                             "topic":  "plan:PLN-20260303-0011",
                                             "status":  "open",
                                             "line":  114,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000044":  {
                                             "topic":  "plan:PLN-20260225-0013",
                                             "status":  "done",
                                             "line":  44,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000004":  {
                                             "topic":  "plan:PLN-20260310-0003",
```
```text
                                             "status":  "open",
                                             "line":  144,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000030":  {
                                             "topic":  "plan:PLN-20260310-0005",
                                             "status":  "open",
                                             "line":  170,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000002":  {
                                             "topic":  "plan:PLN-20260310-0001",
                                             "status":  "done",
                                             "line":  142,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000052":  {
                                             "topic":  "plan:PLN-20260225-0015",
                                             "status":  "done",
                                             "line":  52,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000001":  {
                                             "topic":  "render:volumetrics-v1",
                                             "status":  "done",
                                             "line":  141,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000016":  {
                                             "topic":  "plan:PLN-20260225-0011-finalize",
                                             "status":  "done",
                                             "line":  81,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000029":  {
                                             "topic":  "plan:PLN-20260303-0004",
                                             "status":  "done",
                                             "line":  94,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000031":  {
                                             "topic":  "plan:PLN-20260310-0006",
                                             "status":  "open",
                                             "line":  171,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000064":  {
                                             "topic":  "plan:PLN-20260225-0018",
                                             "status":  "done",
                                             "line":  64,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000045":  {
                                             "topic":  "external-mods:pistol-aim-toggle-reticle-model-fix",
                                             "status":  "done",
                                             "line":  45,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000012":  {
                                             "topic":  "plan:PLN-20260225-0004",
                                             "status":  "open",
                                             "line":  12,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000010":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  150,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000033":  {
                                             "topic":  "git-finalize-blocked",
                                             "status":  "open",
                                             "line":  33,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000012":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  152,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000010":  {
                                             "topic":  "external-mod-v3-gap-closure",
                                             "status":  "done",
                                             "line":  10,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000029":  {
                                             "topic":  "plan:PLN-20260310-0004",
                                             "status":  "open",
                                             "line":  169,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000011":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  151,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000065":  {
                                             "topic":  "plan:PLN-20260225-0019",
                                             "status":  "open",
                                             "line":  65,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000017":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "open",
                                             "line":  157,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000048":  {
                                             "topic":  "plan:PLN-20260225-0014",
                                             "status":  "open",
                                             "line":  48,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000002":  {
                                             "topic":  "external-mods:v4-sss-rollout",
                                             "status":  "done",
                                             "line":  67,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000016":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "open",
                                             "line":  133,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000046":  {
                                             "topic":  "plan:PLN-20260303-0010",
                                             "status":  "open",
                                             "line":  111,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000035":  {
                                             "topic":  "plan:PLN-20260303-0006",
                                             "status":  "done",
                                             "line":  100,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000014":  {
                                             "topic":  "plan:PLN-20260225-0013",
                                             "status":  "done",
                                             "line":  79,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000023":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  163,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000022":  {
                                             "topic":  "plan:PLN-20260225-0006",
                                             "status":  "done",
                                             "line":  87,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000050":  {
                                             "topic":  "plan:PLN-20260303-0011",
                                             "status":  "done",
                                             "line":  115,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000020":  {
                                             "topic":  "external-mods:aim-ots-reticle-lmb",
                                             "status":  "done",
                                             "line":  20,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000034":  {
                                             "topic":  "plan:PLN-20260303-0006",
                                             "status":  "open",
                                             "line":  99,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000002":  {
                                             "topic":  "plan:PLN-20260304-0001",
                                             "status":  "open",
                                             "line":  119,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000062":  {
                                             "topic":  "plan:PLN-20260225-0018",
                                             "status":  "done",
                                             "line":  62,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000026":  {
                                             "topic":  "agents:governance-v4-skills-hardening",
                                             "status":  "done",
                                             "line":  91,
                                             "source":  "memory.log"
                                         },
                 "mem-20260304-000003":  {
                                             "topic":  "plan:PLN-20260304-0001",
                                             "status":  "open",
                                             "line":  120,
                                             "source":  "memory.log"
                                         },
                 "mem-20260303-000032":  {
                                             "topic":  "plan:PLN-20260303-0005",
                                             "status":  "done",
                                             "line":  97,
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000022":  {
                                             "topic":  "agents-governance-protocol",
                                             "status":  "done",
                                             "line":  22,
                                             "source":  "memory.log"
                                         },
                 "mem-20260305-000001":  {
                                             "topic":  "plan:PLN-20260305-0001",
                                             "status":  "open",
                                             "line":  140,
                                             "source":  "memory.log"
                                         },
```

## file: tools/external_mods/export_runtime_reference.ps1
```text
[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$docsDir = Join-Path $resolvedRoot "docs"

if (-not (Test-Path $docsDir)) {
    throw "docs directory not found: $docsDir"
}

$generatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")

$catalogs = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    runtime = @{
        type = "wasm3-v1"
        budgetDefaults = @{
            maxCallMs = 2
            maxFrameBudgetMs = 2
            maxHookCallsPerFrame = 256
            maxActorInstances = 64
            maxActiveStatuses = 256
        }
        settingsDomains = @("global", "save", "session")
        settingsApplyModes = @("realtime", "scene_reload", "restart")
    }
    capabilities = @(
        @{ id = "hooks.extended.v1"; fileField = "hookDefinitions"; fileDefault = "hooks/hooks.json" },
        @{ id = "actors.vm.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "actors.generic.v1"; fileField = "actorDefinitions"; fileDefault = "actors/actors.json" },
        @{ id = "behaviors.graph.v1"; fileField = "behaviorDefinitions"; fileDefault = "behaviors/behaviors.json" },
        @{ id = "scenes.bundle.v1"; fileField = "sceneDefinitions"; fileDefault = "scenes/scenes.json" },
        @{ id = "statuses.catalog.v1"; fileField = "statusDefinitions"; fileDefault = "statuses/statuses.json"; schemaVersion = 1 },
        @{ id = "combat.damage.v1"; fileField = "damageDefinitions"; fileDefault = "combat/damage_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.targeting.v1"; fileField = "targetingDefinitions"; fileDefault = "combat/targeting_profiles.json"; schemaVersion = 1 },
        @{ id = "combat.projectiles.v1"; fileField = "projectileDefinitions"; fileDefault = "combat/projectiles.json"; schemaVersion = 1 },
        @{ id = "combat.aoe.v1"; fileField = "aoeDefinitions"; fileDefault = "combat/aoe_profiles.json"; schemaVersion = 1 },
        @{ id = "movement.profiles.v1"; fileField = "movementDefinitions"; fileDefault = "movement/movement_profiles.json"; schemaVersion = 1 },
        @{ id = "items.use_profiles.v1"; fileField = "itemUseProfiles"; fileDefault = "items/use_profiles.json"; schemaVersion = 1 },
        @{ id = "patches.vanilla_items.v1"; fileField = "vanillaItemPatches"; fileDefault = "patches/vanilla_items.patch.json" },
        @{ id = "world.queries.v1"; fileField = ""; fileDefault = ""; note = "No file required; enables world query actions." },
        @{ id = "input.bindings.v2"; fileField = "inputDefinitions"; fileDefault = "config/input.json"; schemaVersion = 1 },
        @{ id = "ui.runtime.v1"; fileField = "uiScreenDefinitions"; fileDefault = "ui/screens.json"; schemaVersion = 1 },
        @{ id = "ui.hud.v1"; fileField = "uiHudDefinitions"; fileDefault = "ui/hud_layouts.json"; schemaVersion = 1 },
        @{ id = "ui.inventory_ext.v1"; fileField = "inventoryExtensionDefinitions"; fileDefault = "inventory_ext/pages.json"; schemaVersion = 1 },
        @{ id = "containers.v1"; fileField = "containerDefinitions"; fileDefault = "containers/containers.json"; schemaVersion = 1 },
        @{ id = "recipes.processing.v1"; fileField = "recipeDefinitions"; fileDefault = "recipes/processing_recipes.json"; schemaVersion = 1 },
        @{ id = "interactions.v1"; fileField = "interactionDefinitions"; fileDefault = "interactions/interactions.json"; schemaVersion = 1 },
        @{ id = "actors.archetypes.v1"; fileField = "actorArchetypeDefinitions"; fileDefault = "actors/archetypes.json"; schemaVersion = 1 },
        @{ id = "actors.adapters.v1"; fileField = "actorAdapterDefinitions"; fileDefault = "actors/adapters.json"; schemaVersion = 1 },
        @{ id = "ai.behavior_trees.v1"; fileField = "behaviorTreeDefinitions"; fileDefault = "ai/behavior_trees.json"; schemaVersion = 1 },
        @{ id = "ai.sensors.v1"; fileField = "sensorDefinitions"; fileDefault = "ai/sensors.json"; schemaVersion = 1 },
        @{ id = "nav.routes.v1"; fileField = "routeDefinitions"; fileDefault = "nav/routes.json"; schemaVersion = 1 },
        @{ id = "nav.navmesh_bridge.v1"; fileField = "navBridgeDefinitions"; fileDefault = "nav/nav_bridge.json"; schemaVersion = 1 },
        @{ id = "debug.overlay.v1"; fileField = "debugOverlayDefinitions"; fileDefault = "debug/overlays.json"; schemaVersion = 1 },
        @{ id = "items.state_machine.v1"; fileField = "itemStateDefinitions"; fileDefault = "items/item_states.json"; schemaVersion = 1 },
        @{ id = "render.equipped_models.v1"; fileField = "equippedModelDefinitions"; fileDefault = "render/equipped_models.json"; schemaVersion = 1 },
        @{ id = "hud.widgets.v1"; fileField = "hudWidgetDefinitions"; fileDefault = "ui/widgets.json"; schemaVersion = 1 },
        @{ id = "hud.reticles.v2"; fileField = "hudReticleDefinitions"; fileDefault = "ui/reticles.json"; schemaVersion = 1 },
        @{ id = "camera.aim_profiles.v2"; fileField = "cameraDefinitions"; fileDefault = "camera/camera_profiles.json"; schemaVersion = 1 },
        @{ id = "effects.graph.v2"; fileField = "effectGraphDefinitions"; fileDefault = "effects/effect_graphs.json"; schemaVersion = 1 },
        @{ id = "combat.hit_rules.v2"; fileField = "combatHitRuleDefinitions"; fileDefault = "combat/hit_rules.json"; schemaVersion = 1 },
        @{ id = "movement.surf.v2"; fileField = "surfDefinitions"; fileDefault = "movement/surf_profiles.json"; schemaVersion = 1 },
        @{ id = "actors.tags.v1"; fileField = "actorTagDefinitions"; fileDefault = "actors/actor_tags.json"; schemaVersion = 1 },
        @{ id = "world.patchsets.v1"; fileField = "worldPatchDefinitions"; fileDefault = "world/patchsets.json"; schemaVersion = 1 },
        @{ id = "quests.graph.v1"; fileField = "questDefinitions"; fileDefault = "quests/quests.json"; schemaVersion = 1 },
        @{ id = "dialog.nodes.v1"; fileField = "dialogDefinitions"; fileDefault = "dialog/dialogs.json"; schemaVersion = 1 },
        @{ id = "sdk.generators.v1"; fileField = "sdkGeneratorDefinitions"; fileDefault = "sdk/generators.json"; schemaVersion = 1 },
        @{ id = "fx.presets.v1"; fileField = "fxPresetDefinitions"; fileDefault = "fx/fx_presets.json"; schemaVersion = 1 },
        @{ id = "states.catalog.v1"; fileField = "stateDefinitions"; fileDefault = "states/states.json"; schemaVersion = 1 },
        @{ id = "spells.catalog.v1"; fileField = "spellDefinitions"; fileDefault = "spells/spells.json"; schemaVersion = 1 },
        @{ id = "render.materials.v1"; fileField = "materialDefinitions"; fileDefault = "render/materials.json"; schemaVersion = 1 },
        @{ id = "render.pbr.v1"; fileField = "pbrDefinitions"; fileDefault = "render/pbr_profiles.json"; schemaVersion = 1 },
        @{ id = "render.lighting.v1"; fileField = "lightingDefinitions"; fileDefault = "render/light_profiles.json"; schemaVersion = 1 },
        @{ id = "render.postfx.v1"; fileField = "postFxDefinitions"; fileDefault = "render/postfx_presets.json"; schemaVersion = 1 },
        @{ id = "world.scenes.v1"; fileField = "sceneProfileDefinitions"; fileDefault = "world/scene_profiles.json"; schemaVersion = 1 },
        @{ id = "world.rooms.v1"; fileField = "roomProfileDefinitions"; fileDefault = "world/room_profiles.json"; schemaVersion = 1 },
        @{ id = "assets.packs.v2"; fileField = "assetPackDefinitions"; fileDefault = "assets/packs.json"; schemaVersion = 1 },
        @{ id = "debug.render_inspector.v1"; fileField = "renderInspectorDefinitions"; fileDefault = "debug/render_inspector.json"; schemaVersion = 1 },
        @{ id = "world.persistence.v1"; fileField = "worldPersistenceDefinitions"; fileDefault = "world/persistence.json"; schemaVersion = 1 },
        @{ id = "world.storage.v1"; fileField = "worldStorageDefinitions"; fileDefault = "world/storage_domains.json"; schemaVersion = 1 },
        @{ id = "world.spawn_profiles.v1"; fileField = "worldSpawnProfileDefinitions"; fileDefault = "world/spawn_profiles.json"; schemaVersion = 1 },
        @{ id = "world.time_weather.v1"; fileField = "worldTimeWeatherDefinitions"; fileDefault = "world/time_weather.json"; schemaVersion = 1 },
        @{ id = "world.seeding.v1"; fileField = "worldSeedingDefinitions"; fileDefault = "world/seeding.json"; schemaVersion = 1 },
        @{ id = "world.migrations.v1"; fileField = "worldMigrationDefinitions"; fileDefault = "world/migrations.json"; schemaVersion = 1 },
        @{ id = "debug.persistence_inspector.v1"; fileField = "persistenceInspectorDefinitions"; fileDefault = "debug/persistence_inspector.json"; schemaVersion = 1 },
        @{ id = "narrative.timeline.v1"; fileField = "narrativeTimelineDefinitions"; fileDefault = "narrative/timelines.json"; schemaVersion = 1 },
        @{ id = "narrative.dialogue.v1"; fileField = "narrativeDialogueDefinitions"; fileDefault = "narrative/dialogues.json"; schemaVersion = 1 },
        @{ id = "narrative.quests.v1"; fileField = "narrativeQuestDefinitions"; fileDefault = "narrative/quests.json"; schemaVersion = 1 },
        @{ id = "narrative.flags.v1"; fileField = "narrativeFlagDefinitions"; fileDefault = "narrative/flags.json"; schemaVersion = 1 },
        @{ id = "debug.narrative_inspector.v1"; fileField = "narrativeInspectorDefinitions"; fileDefault = "debug/narrative_inspector.json"; schemaVersion = 1 },
        @{ id = "dev.hot_reload.v1"; fileField = "devHotReloadDefinitions"; fileDefault = "dev/hot_reload.json"; schemaVersion = 1 },
        @{ id = "dev.console.v1"; fileField = "devConsoleDefinitions"; fileDefault = "dev/console_commands.json"; schemaVersion = 1 },
        @{ id = "dev.watchers.v1"; fileField = "devWatcherDefinitions"; fileDefault = "dev/watchers.json"; schemaVersion = 1 },
        @{ id = "wasm.sandbox.v2"; fileField = "wasmSandboxDefinitions"; fileDefault = "runtime/wasm_sandbox.json"; schemaVersion = 1 },
        @{ id = "debug.reload_inspector.v1"; fileField = "reloadInspectorDefinitions"; fileDefault = "debug/reload_inspector.json"; schemaVersion = 1 }
    )
    contracts = @{
        statuses = @{
            freezeProfileMode = @("legacy_timer", "ice_trap_no_damage")
            coreStatusPresets = @("core:freeze_ice_trap_no_damage")
        }
        aoe = @{
            targetScope = @("all_non_player", "enemies_bosses", "enemies_bosses_props", "player_enemies_bosses", "all_with_player")
        }
        worldGraphics = @{
            sceneHooks = @("onWorldSceneLoaded", "onWorldRoomEntered", "onWorldRoomExited")
            tickHooks = @("onWorldOverworldTick", "onWorldTimeOfDayChanged", "onWorldSkyboxChanged")
            renderActions = @("render.setPostFxPreset", "render.spawnLight", "render.setSkylight", "render.overrideMaterial")
        }
        settings = @{
            schemaFile = "settings/settings.schema.json"
            actions = @("settings.get", "settings.set", "settings.reset", "settings.list")
            event = "settings.changed"
        }
        persistence = @{
            rootPath = "<save_root>/external_mods/persist/<worldSlotId>/<modId>/"
            domainFiles = @("manifest.json", "entities/<scene>_<room>.json", "domains/*.json")
        }
        narrative = @{
            keyActions = @("narrative.startDialogue", "narrative.chooseOption", "narrative.startQuest", "narrative.startTimeline")
        }
    }
}

$actions = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    actions = @(
        @{ name = "showNotification"; category = "core" },
        @{ name = "teleportToEntrance"; category = "core" },
        @{ name = "loadModScene"; category = "scene" },
        @{ name = "pressButton"; category = "core" },
        @{ name = "showEquippedItemGet"; category = "core" },
        @{ name = "spawnSmoke"; category = "fx" },
        @{ name = "spawnKusa"; category = "fx" },
        @{ name = "lanternLight"; category = "fx" },
        @{ name = "applyStatus"; category = "status" },
        @{ name = "clearStatus"; category = "status" },
        @{ name = "clearAllStatuses"; category = "status" },
        @{ name = "useItemProfile"; category = "items" },
        @{ name = "dealDamage"; category = "combat" },
        @{ name = "spawnProjectile"; category = "combat" },
        @{ name = "spawnAoE"; category = "combat" },
        @{ name = "applyMovementProfile"; category = "movement" },
        @{ name = "applyImpulse"; category = "movement" },
        @{ name = "getGroundInfo"; category = "world_query" },
        @{ name = "raycast"; category = "world_query" },
        @{ name = "raycastAll"; category = "world_query" },
        @{ name = "spawnActor"; category = "actor" },
        @{ name = "despawnActor"; category = "actor" },
        @{ name = "setActorState"; category = "actor" },
        @{ name = "moveActorToPathNode"; category = "actor" },
        @{ name = "openDialog"; category = "dialog" },
        @{ name = "setSwitchFlag"; category = "flags" },
        @{ name = "clearSwitchFlag"; category = "flags" },
        @{ name = "setEventChkInf"; category = "flags" },
        @{ name = "clearEventChkInf"; category = "flags" },
        @{ name = "setInfTable"; category = "flags" },
        @{ name = "clearInfTable"; category = "flags" },
        @{ name = "giveRupees"; category = "economy" },
        @{ name = "takeRupees"; category = "economy" },
        @{ name = "grantModItem"; category = "items" },
        @{ name = "revokeModItem"; category = "items" },
        @{ name = "setVar"; category = "behavior" },
        @{ name = "addVar"; category = "behavior" },
        @{ name = "clampVar"; category = "behavior" },
        @{ name = "emitSignal"; category = "behavior" },
        @{ name = "callBehavior"; category = "behavior" },
        @{ name = "toggleAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraMode"; category = "camera" },
        @{ name = "setAimCameraProfile"; category = "camera" },
        @{ name = "fx.spawnEffectSs"; category = "fx" },
        @{ name = "fx.spawnActorFx"; category = "fx" },
        @{ name = "fx.spawnPreset"; category = "fx" },
        @{ name = "fx.stopFx"; category = "fx" },
        @{ name = "states.applyState"; category = "state" },
        @{ name = "states.clearState"; category = "state" },
        @{ name = "states.hasState"; category = "state" },
        @{ name = "player.getStateFlags"; category = "player" },
        @{ name = "player.setStateFlag"; category = "player" },
        @{ name = "player.clearStateFlag"; category = "player" },
        @{ name = "player.setControlLock"; category = "player" },
        @{ name = "player.setGravityScale"; category = "player" },
        @{ name = "player.setBoostType"; category = "player" },
        @{ name = "player.setDamageResponse"; category = "player" },
        @{ name = "settings.get"; category = "settings" },
        @{ name = "settings.set"; category = "settings" },
        @{ name = "settings.reset"; category = "settings" },
        @{ name = "settings.list"; category = "settings" },
        @{ name = "spells.castSpell"; category = "spells" },
        @{ name = "ui.openScreen"; category = "ui" },
        @{ name = "ui.closeScreen"; category = "ui" },
        @{ name = "ui.toggleScreen"; category = "ui" },
        @{ name = "ui.focusNext"; category = "ui" },
        @{ name = "ui.focusPrev"; category = "ui" },
        @{ name = "inventoryExt.createPage"; category = "inventory_ext" },
        @{ name = "inventoryExt.moveItem"; category = "inventory_ext" },
        @{ name = "inventoryExt.save"; category = "inventory_ext" },
        @{ name = "inventoryExt.load"; category = "inventory_ext" },
        @{ name = "container.open"; category = "container" },
        @{ name = "container.moveItem"; category = "container" },
        @{ name = "container.startProcess"; category = "container" },
        @{ name = "container.cancelProcess"; category = "container" },
        @{ name = "container.getProgress"; category = "container" },
        @{ name = "actors.spawnArchetype"; category = "actors" },
        @{ name = "actors.despawnArchetype"; category = "actors" },
        @{ name = "interactions.invoke"; category = "interactions" },
        @{ name = "ai.runBehavior"; category = "ai" },
        @{ name = "ai.setBlackboard"; category = "ai" },
        @{ name = "ai.clearBlackboard"; category = "ai" },
        @{ name = "sense.findTargets"; category = "sensors" },
        @{ name = "sense.lineOfSight"; category = "sensors" },
```
```text
        @{ name = "sense.distance"; category = "sensors" },
        @{ name = "nav.requestPath"; category = "nav" },
        @{ name = "nav.getPathPoints"; category = "nav" },
        @{ name = "nav.releasePath"; category = "nav" },
        @{ name = "debug.showOverlay"; category = "debug" },
        @{ name = "debug.hideOverlay"; category = "debug" },
        @{ name = "world.setSceneProfile"; category = "world" },
        @{ name = "world.setRoomProfile"; category = "world" },
        @{ name = "render.setPostFxPreset"; category = "render" },
        @{ name = "render.spawnLight"; category = "render" },
        @{ name = "render.setSkylight"; category = "render" },
        @{ name = "render.overrideMaterial"; category = "render" },
        @{ name = "persist.ensureEntityGuid"; category = "persistence" },
        @{ name = "persist.saveEntityState"; category = "persistence" },
        @{ name = "persist.loadEntityState"; category = "persistence" },
        @{ name = "persist.deleteEntityState"; category = "persistence" },
        @{ name = "persist.setDomainValue"; category = "persistence" },
        @{ name = "persist.getDomainValue"; category = "persistence" },
        @{ name = "persist.runMigrations"; category = "persistence" },
        @{ name = "world.spawnFromProfile"; category = "world" },
        @{ name = "world.time.setOverride"; category = "world" },
        @{ name = "world.weather.setOverride"; category = "world" },
        @{ name = "narrative.startDialogue"; category = "narrative" },
        @{ name = "narrative.chooseOption"; category = "narrative" },
        @{ name = "narrative.advanceDialogue"; category = "narrative" },
        @{ name = "narrative.setFlag"; category = "narrative" },
        @{ name = "narrative.clearFlag"; category = "narrative" },
        @{ name = "narrative.startQuest"; category = "narrative" },
        @{ name = "narrative.updateObjective"; category = "narrative" },
        @{ name = "narrative.startTimeline"; category = "narrative" },
        @{ name = "narrative.skipTimeline"; category = "narrative" },
        @{ name = "dev.reloadAll"; category = "dev" },
        @{ name = "dev.reloadTarget"; category = "dev" },
        @{ name = "dev.console.exec"; category = "dev" },
        @{ name = "invokeWasm"; category = "wasm" }
    )
    removedInApiV4 = @(
        "igniteFrontTarget",
        "freezeFrontTarget",
        "items.params.freezeOnMeleeHit",
        "items.params.freezeOnHitDuration",
        "items.params.freezeOnHitShake",
        "items.params.freezeOnHitIntensity"
    )
    wasmHostImports = @(
        "host_useItemProfile",
        "host_resolveTarget",
        "host_dealDamage",
        "host_applyStatus",
        "host_spawnProjectile",
        "host_spawnAoE",
        "host_applyMovementProfile",
        "host_applyImpulse",
        "host_getGroundInfo",
        "host_raycast",
        "host_raycastAll"
    )
    disableReasonCodes = @(
        "BUDGET_HOOK",
        "BUDGET_WASM_CALL",
        "BUDGET_WASM_FRAME",
        "WASM_IMPORT_FAIL",
        "WASM_EXPORT_FAIL",
        "WASM_RUNTIME_ERROR"
    )
}

$events = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    hooks = @(
        "onLoadGame",
        "onExitGame",
        "onSceneInit",
        "afterSceneCommands",
        "onTransitionEnd",
        "onFlagSet",
        "onFlagUnset",
        "onSceneFlagSet",
        "onSceneFlagUnset",
        "onPlayerUpdate",
        "onPlayerUseItem",
        "onPlayerHealthChange",
        "onItemReceive",
        "onActorInit",
        "onActorSpawn",
        "onActorUpdate",
        "onActorKill",
        "onActorDestroy",
        "onEnemyDefeat",
        "onBossDefeat",
        "onStatusApplied",
        "onStatusTick",
        "onStatusExpired",
        "onStateApplied",
        "onStateRemoved",
        "onUiScreenOpened",
        "onUiScreenClosed",
        "onUiAction",
        "onContainerSlotChanged",
        "onProcessStart",
        "onProcessTick",
        "onProcessComplete",
        "onArchetypeSpawn",
        "onArchetypeDespawn",
        "onInteraction",
        "onBehaviorNodeChanged",
        "onPathRequested",
        "onPathFailed",
        "onWorldSceneLoaded",
        "onWorldRoomEntered",
        "onWorldRoomExited",
        "onWorldOverworldTick",
        "onWorldTimeOfDayChanged",
        "onWorldSkyboxChanged",
        "onPersistentEntityLoaded",
        "onPersistentEntitySaved",
        "onPersistentDomainMigrated",
        "onWorldSpawnProfileTick",
        "world.time.segmentChanged",
        "world.weather.changed",
        "onDialogueStarted",
        "onDialogueChoiceCommitted",
        "onQuestStateChanged",
        "onTimelineStarted",
        "onTimelineCompleted",
        "onTimelineSkipped",
        "onHotReloadApplied",
        "onHotReloadFailed",
        "onSandboxBudgetExceeded",
        "onSandboxPermissionDenied",
        "onPlayDestroy",
        "onGameFrameUpdate"
    )
    behaviorEvents = @(
        "manual",
        "onInit",
        "onSpawn",
        "onDespawn",
        "onDestroy",
        "onUpdate",
        "onRandomTick",
        "onRoomEnter",
        "onTimeOfDayChanged",
        "onSwitchFlagChanged",
        "onPlayerNear",
        "onPlayerFar",
        "onTimer",
        "onInteract",
        "onSignal",
        "onSceneEnter",
        "onItemUsed",
        "onItemGranted",
        "onItemEquipped",
        "onCooldownReady",
        "settings.changed"
    )
    aliases = @(
        @{ from = "oninit"; to = "onspawn" },
        @{ from = "ondespawn"; to = "ondestroy" },
        @{ from = "onitemequipped"; to = "onitemgranted" }
    )
}

$actionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for action validation/dispatch contracts."
    actions = @(
        @{ name = "applyStatus"; params = @("status", "target?", "actorHandle?", "durationFrames?") },
        @{ name = "useItemProfile"; params = @("profile|useProfile|profileId") },
        @{ name = "dealDamage"; params = @("profile|damageProfileId", "target?", "actorHandle?") },
        @{ name = "spawnProjectile"; params = @("projectile|profile|projectileProfileId") },
        @{ name = "spawnAoE"; params = @("aoe|profile|aoeProfileId") },
        @{ name = "applyMovementProfile"; params = @("movement|profile|movementProfileId", "durationFrames?") },
        @{ name = "applyImpulse"; params = @("mode?", "strength?") },
        @{ name = "getGroundInfo"; params = @() },
        @{ name = "raycast"; params = @("range?") },
        @{ name = "raycastAll"; params = @("range?") },
        @{ name = "toggleAimCameraMode"; params = @("profileId?", "itemId?") },
        @{ name = "setAimCameraMode"; params = @("mode", "profileId?", "itemId?") },
        @{ name = "setAimCameraProfile"; params = @("profileId") },
        @{ name = "fx.spawnEffectSs"; params = @("name|effect|effectId", "scale?", "lifeFrames?", "attachFollow?", "storeKey?") },
        @{ name = "fx.spawnActorFx"; params = @("actorId", "overlay?", "scale?", "lifeFrames?", "attachFollow?", "storeKey?") },
        @{ name = "fx.spawnPreset"; params = @("presetId|preset", "storeKey?") },
        @{ name = "fx.stopFx"; params = @("handleKey|storeKey|key") },
        @{ name = "states.applyState"; params = @("stateId|state", "durationFrames?", "domain?") },
        @{ name = "states.clearState"; params = @("stateId|state", "domain?") },
        @{ name = "states.hasState"; params = @("stateId|state", "storeKey?") },
        @{ name = "player.getStateFlags"; params = @("storeKey?") },
        @{ name = "player.setStateFlag"; params = @("flag") },
        @{ name = "player.clearStateFlag"; params = @("flag") },
        @{ name = "player.setControlLock"; params = @("enabled") },
        @{ name = "player.setGravityScale"; params = @("scale") },
        @{ name = "player.setBoostType"; params = @("mode") },
        @{ name = "player.setDamageResponse"; params = @("mode") },
        @{ name = "settings.get"; params = @("key", "domain?", "storeKey?") },
        @{ name = "settings.set"; params = @("key", "value", "domain?") },
        @{ name = "settings.reset"; params = @("key?", "domain?") },
        @{ name = "settings.list"; params = @("storeKey?", "domain?") },
        @{ name = "spells.castSpell"; params = @("spellId|spell") },
        @{ name = "ui.openScreen"; params = @("screen|screenId") },
        @{ name = "ui.closeScreen"; params = @("screen|screenId") },
        @{ name = "ui.toggleScreen"; params = @("screen|screenId") },
        @{ name = "ui.focusNext"; params = @() },
        @{ name = "ui.focusPrev"; params = @() },
        @{ name = "inventoryExt.createPage"; params = @("pageId", "slots") },
        @{ name = "inventoryExt.moveItem"; params = @("srcBinding|src", "dstBinding|dst", "count?") },
        @{ name = "inventoryExt.save"; params = @() },
        @{ name = "inventoryExt.load"; params = @() },
        @{ name = "container.open"; params = @("containerId|container") },
        @{ name = "container.moveItem"; params = @("srcBinding|src", "dstBinding|dst", "count?") },
        @{ name = "container.startProcess"; params = @("containerId|container", "recipeId?") },
        @{ name = "container.cancelProcess"; params = @("containerId|container") },
        @{ name = "container.getProgress"; params = @("containerId|container", "storeKey?") },
        @{ name = "actors.spawnArchetype"; params = @("archetypeId|archetype") },
        @{ name = "actors.despawnArchetype"; params = @("actorHandle|handle|id") },
        @{ name = "interactions.invoke"; params = @("interactionId|interaction") },
        @{ name = "ai.runBehavior"; params = @("behaviorId|behavior") },
        @{ name = "ai.setBlackboard"; params = @("key", "value") },
```
```text
        @{ name = "ai.clearBlackboard"; params = @("key") },
        @{ name = "sense.findTargets"; params = @("storeKey?") },
        @{ name = "sense.lineOfSight"; params = @("range?", "storeKey?") },
        @{ name = "sense.distance"; params = @("storeKey?") },
        @{ name = "nav.requestPath"; params = @("routeId?", "storeKey?") },
        @{ name = "nav.getPathPoints"; params = @("handleKey|storeKey") },
        @{ name = "nav.releasePath"; params = @("handleKey|storeKey") },
        @{ name = "debug.showOverlay"; params = @("overlayId|overlay") },
        @{ name = "debug.hideOverlay"; params = @("overlayId|overlay") },
        @{ name = "world.setSceneProfile"; params = @("profileId|profile", "sceneId?") },
        @{ name = "world.setRoomProfile"; params = @("profileId|profile", "roomId", "sceneId?") },
        @{ name = "render.setPostFxPreset"; params = @("presetId|profileId|preset", "durationMs?", "blend?") },
        @{ name = "render.spawnLight"; params = @("profileId|profile", "actorHandle?", "lifetimeMs?|durationMs?", "storeKey?") },
        @{ name = "render.setSkylight"; params = @("profileId|profile") },
        @{ name = "render.overrideMaterial"; params = @("materialId|material", "match?", "scope?", "durationFrames?") },
        @{ name = "persist.ensureEntityGuid"; params = @("entityGuid?", "scope?") },
        @{ name = "persist.saveEntityState"; params = @("entityGuid?", "key?", "value?") },
        @{ name = "persist.loadEntityState"; params = @("entityGuid?", "key?") },
        @{ name = "persist.deleteEntityState"; params = @("entityGuid?") },
        @{ name = "persist.setDomainValue"; params = @("domainId|domain", "key", "value") },
        @{ name = "persist.getDomainValue"; params = @("domainId|domain", "key") },
        @{ name = "persist.runMigrations"; params = @("migrationId?") },
        @{ name = "world.spawnFromProfile"; params = @("profileId|profile") },
        @{ name = "world.time.setOverride"; params = @("segment|value") },
        @{ name = "world.weather.setOverride"; params = @("weather|value") },
        @{ name = "narrative.startDialogue"; params = @("dialogueId|id") },
        @{ name = "narrative.chooseOption"; params = @("optionId|id") },
        @{ name = "narrative.advanceDialogue"; params = @("nodeId?") },
        @{ name = "narrative.setFlag"; params = @("flagId|key", "value?") },
        @{ name = "narrative.clearFlag"; params = @("flagId|key") },
        @{ name = "narrative.startQuest"; params = @("questId|id") },
        @{ name = "narrative.updateObjective"; params = @("questId", "objectiveId", "state?") },
        @{ name = "narrative.startTimeline"; params = @("timelineId|id") },
        @{ name = "narrative.skipTimeline"; params = @("timelineId?") },
        @{ name = "dev.reloadAll"; params = @() },
        @{ name = "dev.reloadTarget"; params = @("target|registry") },
        @{ name = "dev.console.exec"; params = @("commandId|id") },
        @{ name = "invokeWasm"; params = @("export", "args?") }
    )
}

$conditionsRegistry = [ordered]@{
    generatedUtc = $generatedUtc
    apiVersion = 4
    description = "Declarative registry for behavior condition contracts."
    conditions = @(
        @{ name = "isChild"; fields = @() },
        @{ name = "isAdult"; fields = @() },
        @{ name = "isDay"; fields = @() },
        @{ name = "isNight"; fields = @() },
        @{ name = "randomChance"; fields = @("value|numberValue") },
        @{ name = "hasItem"; fields = @("value") },
        @{ name = "hasStatus"; fields = @("scope?", "key|value", "op?") },
        @{ name = "statusRemaining"; fields = @("scope?", "key|value", "op", "value|numberValue") },
        @{ name = "distanceToPlayer"; fields = @("op?", "numberValue") },
        @{ name = "sceneIs"; fields = @("op?", "value|numberValue") },
        @{ name = "roomIs"; fields = @("op?", "value|numberValue") },
        @{ name = "hasSwitchFlag"; fields = @("op?", "value|numberValue") },
        @{ name = "var"; fields = @("scope", "key", "op?", "value") }
    )
}

$outputs = @(
    @{ path = (Join-Path $docsDir "catalogs.json"); data = $catalogs },
    @{ path = (Join-Path $docsDir "actions.json"); data = $actions },
    @{ path = (Join-Path $docsDir "events.json"); data = $events },
    @{ path = (Join-Path $docsDir "runtime_contract/actions.registry.json"); data = $actionsRegistry },
    @{ path = (Join-Path $docsDir "runtime_contract/conditions.registry.json"); data = $conditionsRegistry }
)

foreach ($out in $outputs) {
    $parentDir = Split-Path -Path $out.path -Parent
    if (-not [string]::IsNullOrWhiteSpace($parentDir) -and -not (Test-Path $parentDir)) {
        if (-not $DryRun) {
            New-Item -ItemType Directory -Path $parentDir -Force | Out-Null
        }
    }
    $json = $out.data | ConvertTo-Json -Depth 12
    if ($DryRun) {
        Write-Host "[DryRun] Would write $($out.path)"
        continue
    }
    Set-Content -Path $out.path -Value $json -Encoding UTF8
    Write-Host "Wrote $($out.path)"
}

$docsGeneratorScript = Join-Path $resolvedRoot "tools/external_mods/generate_contract_docs.ps1"
if (Test-Path $docsGeneratorScript) {
    & $docsGeneratorScript -RepoRoot $resolvedRoot -OutDir "docs/generated" -DryRun:$DryRun *> $null
    if ($LASTEXITCODE -ne 0) {
        throw "generate_contract_docs.ps1 failed while exporting runtime references"
    }
    if (-not $DryRun) {
        Write-Host "Generated docs indexes under docs/generated"
    } else {
        Write-Host "[DryRun] Would generate docs indexes under docs/generated"
    }
} else {
    Write-Warning "docs generator script not found: $docsGeneratorScript"
}
```

## file: tools/external_mods/sync_examples_to_runtime.ps1
```text
param(
    [string]$SourceRoot = "docs/examples/external_mods",
    [string]$RuntimeRoot = "x64/Release/mods",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

$demoNames = @(
    "sss",
    "sss_content_pack",
    "ui_kit",
    "container_kit",
    "ai_templates",
    "lighting_kit",
    "world_profiles",
    "material_helpers",
    "persistence_kit",
    "narrative_kit",
    "devtools_kit",
    "demo_mini_inventory_h",
    "demo_bag_inventory_ext",
    "demo_furnace_system",
    "demo_npc_patrol",
    "demo_enemy_templates",
    "demo_vanilla_adapters",
    "demo_room_profiles",
    "demo_actor_lights",
    "demo_green_fire_magic",
    "aim_ots_toggle_demo",
    "fire_staff_demo",
    "freeze_staff_demo",
    "status_catalog_demo",
    "pistol_hitscan_demo",
    "spear_lunge_demo",
    "shield_skate_demo",
    "axe_prop_cut_demo",
    "freeze_dome_staff_demo",
    "framework_no_items_demo",
    "live_settings_realtime_demo",
    "unresolved_reference_demo",
    "compat_wrapper_demo",
    "demo_persistent_tents",
    "demo_loot_respawn",
    "demo_dialogue_npc",
    "demo_cutscene_intro",
    "demo_quest_chain",
    "demo_hot_reload_playground"
)

if (-not (Test-Path $SourceRoot)) {
    throw "Source root not found: $SourceRoot"
}

New-Item -ItemType Directory -Force -Path $RuntimeRoot | Out-Null

foreach ($demo in $demoNames) {
    $src = Join-Path $SourceRoot $demo
    $dst = Join-Path $RuntimeRoot $demo

    if (-not (Test-Path $src)) {
        Write-Warning "Skipping missing demo: $demo"
        continue
    }

    if ($Clean -and (Test-Path $dst)) {
        Remove-Item -Recurse -Force $dst
    }

    New-Item -ItemType Directory -Force -Path $dst | Out-Null
    Copy-Item -Recurse -Force (Join-Path $src '*') $dst
    Write-Host "Synced $demo"
}

Write-Host "External mod demos synchronized."
```

## file: docs/EXTERNAL_MOD_MANAGER_REFERENCE.md
```text
# External Mod Manager Reference (Sylian Foundry Modloader)

Technical reference for runtime behavior in `ExternalModManager` and related modules.

> Contract baseline: `apiVersion: 4`

## Source locations

- `soh/soh/Enhancements/external-mods/ExternalModManager.h` + `soh/soh/Enhancements/external-mods/ExternalModManager.cpp`
- `soh/soh/Enhancements/external-mods/ExternalModTypes.h`
- `soh/soh/Enhancements/external-mods/ExternalModContentRegistry.h` + `soh/soh/Enhancements/external-mods/ExternalModContentRegistry.cpp`
- `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.h` + `soh/soh/Enhancements/external-mods/ExternalModWasmRuntime.cpp`

## Runtime reference exports

Generated machine-readable references:

- `docs/catalogs.json`
- `docs/actions.json`
- `docs/events.json`

Regenerate with:

```powershell
tools/external_mods/export_runtime_reference.ps1
```

## Manager responsibilities

1. Discover external packages (folder/zip abstractions).
2. Validate manifests/capability files.
3. Register catalogs and behavior subscriptions.
4. Route hook events to behaviors/WASM.
5. Apply per-mod runtime budgets and isolation rules.
6. Cleanup state on reload/shutdown/disable.

## Capability/file enforcement

If a capability is declared, its file/field contract must be valid for that mod.

Canonical capability mapping is exported in `docs/catalogs.json`.

## Conflict resolution

For overlapping catalog IDs across mods:

1. Higher `loadPriority` wins.
2. Tie-breaker: alphabetical `mod.id`.
3. Conflicts are logged with context.

## Runtime budgets (defaults)

Current defaults (see `docs/catalogs.json` runtime.budgetDefaults):

- `maxCallMs = 2`
- `maxFrameBudgetMs = 2`
- `maxHookCallsPerFrame = 256`
- `maxActorInstances = 64`
- `maxActiveStatuses = 256`

## Hook and behavior observability

Runtime tracks and surfaces:

- hook calls per frame
- behavior/WASM execution counts
- budget drops and disable reasons

Use runtime logs and external-mod UI diagnostics for triage.

## Safety model

- Invalid/over-budget mod -> disable that mod, not the whole system.
- Reload must clear mod-owned runtime state deterministically.


```

## file: docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md
```text
# External Mods Data-Driven Reference (API v4)

Reference for JSON-driven mod content used by the Sylian Foundry Modloader.

> External mod contract baseline: `apiVersion: 4`

## 1) Core rules

- IDs must be namespaced (`modId:*`); `core:*` is reserved for built-ins.
- Declared capabilities must provide valid files/paths when required.
- Unknown/invalid schema versions fail the affected mod capability.

## 2) Manifest essentials (`mod.json`)

Typical fields:

- `id`, `name`, `version`, `apiVersion`
- `type` (`framework|content|tools`)
- `loadPriority`
- `dependencies` (`id`/`modId` + optional `versionRange`)
- `capabilities`
- `provides` (declared registries/content roots this mod owns)
- `uses` (optional hint list; runtime reference scan remains authoritative)
- `files.optional` (legacy escape hatch for non-fatal optional files)
- `settings` (`schemaVersion`, `schemaFile`, `defaultDomain`)
- `capabilityRationales` (optional object keyed by capability id)
- `permissionRationales` (optional object keyed by permission id)
- `uiCategory` (`core_api` or `mod`) for External Mods tab classification
- `releaseChannels` (`stable|beta|testers`; non-stable is gated in modloader UI until v1.0)
- `permissions` (`filesystem|network|process|nativeInterop`)
- Runtime security UX:
  - per-mod grants are persisted in `Save/external_mods/permissions/<modId>.json`
  - External Mods UI includes bulk permission presets (`Allow Safe`, `Deny Risky`, `Allow All`, `Reset Defaults`)
  - denied risky permissions trigger a startup guidance notification for that mod
- Runtime hardening:
  - repeated runtime-disable failures are quarantined in `Save/external_mods/quarantine/<modId>.json`
  - quarantined mods stay isolated until user clears quarantine from External Mods UI
- capability file fields (e.g., `damageDefinitions`, `itemUseProfiles`)
- optional runtime budgets (`runtime`)

Machine-readable mapping: `docs/catalogs.json` (`capabilities[]`).
Generated contract/registry indexes:
- `docs/generated/contract_index.json`
- `docs/generated/registry_index.json`

### Flexible contracts (v5.2)

The loader no longer treats unrelated missing files as hard blockers.

- Missing file for a registry declared in `provides` -> **FATAL**
- Missing file for an unused registry -> **WARN** + empty fallback
- Unresolved referenced id -> **ERROR** (feature degraded, mod stays enabled when safe)
- Only **FATAL** disables a mod

Structured diagnostics are exposed in runtime UI and validator output with:

- `severitySummary`
- `issues[]`
  - `severity`
  - `code`
  - `message`
  - `sourcePath`
  - `registryId`
  - `referencedId`
  - `suggestedFix`

## 3) Key content files

- `items/items.json`
- `items/use_profiles.json`
- `statuses/statuses.json`
- `combat/damage_profiles.json`
- `combat/targeting_profiles.json`
- `combat/projectiles.json`
- `combat/aoe_profiles.json`
- `movement/movement_profiles.json`
- `fx/fx_presets.json`
- `states/states.json`
- `spells/spells.json`
- `hooks/hooks.json`
- `behaviors/behaviors.json`
- `ui/screens.json`
- `ui/hud_layouts.json`
- `inventory_ext/pages.json`
- `containers/containers.json`
- `recipes/processing_recipes.json`
- `interactions/interactions.json`
- `actors/archetypes.json`
- `actors/adapters.json`
- `ai/behavior_trees.json`
- `ai/sensors.json`
- `nav/routes.json`
- `nav/nav_bridge.json`
- `debug/overlays.json`
- `settings/settings.schema.json` (optional, when `mod.json.settings.schemaVersion = 1`)

## 3.1) Settings schema (`settings/settings.schema.json`)

Settings are optional and data-driven.

Top-level fields:

- `version`
- `groups[]`
  - `id`, `label`, `order`
  - `entries[]`

Entry fields:

- `key`
- `type`: `bool|int|float|enum|string|color|keybind`
- `default`
- `label`, `help`
- `domain`: `global|save|session`
- `applyMode`: `realtime|scene_reload|restart`
- numeric clamps: `min|max|step`
- enum values: `enumValues[]`
- conditional visibility: `visibility.requiresCapability`, `visibility.requiresRegistry`, `visibility.experimental`

Runtime actions/events:

- `settings.get`
- `settings.set`
- `settings.reset`
- `settings.list`
- hook event: `settings.changed`

Persistence roots:

- global -> `Save/external_mods/settings/global/<modId>.json`
- save -> `Save/external_mods/settings/save/<slot>/<modId>.json`
- session -> in-memory only

## 4) Item definitions (`items/items.json`)

Common fields include:

- identifiers/display metadata
- `slot` / `placement`
- `useMode`
- `useProfile`
- `overrideVanillaItem`
- `aimReticleTextureAsset`
- `aimSelectToggle`
- `aimAttackButtonFire`
- `useTrigger` (`onUse` or `hammerGroundImpact`)

## 5) Use profiles (`items/use_profiles.json`)

Effects are ordered and data-driven. Common actions include:

- damage/status application
- projectile/AoE spawn
- movement profile application
- impulse/world-query dependent effects
- `spawnShockwave` for impact-style FX
- `spells.castSpell`

## 6) Combat catalogs

- Damage profiles support amount/type/flags and prop interaction policy.
- Targeting profiles resolve actor targets (front, lock-on, raycast, cone, sphere, self/player).
- Projectile and AoE profiles define hit/tick behavior, including target scope controls for AoE.
- AoE `targetScope` supports:
  - `all_non_player`
  - `enemies_bosses`
  - `enemies_bosses_props`
  - `player_enemies_bosses`
  - `all_with_player`

## 7) Status freeze profile (ice-trap no-damage mode)

`statuses/statuses.json` can extend a freeze status with:

- `freezeProfile.mode`: `legacy_timer | ice_trap_no_damage`
- `spawnIceShell`, `iceShellSize`
- `lockPosition`, `lockRotation`
- `playerInputLock`
- `breakEffectOnExpire`

Built-in reusable preset status id:

- `core:freeze_ice_trap_no_damage`

This mode freezes player/enemy/boss without periodic damage while preserving freeze lock behavior.

## 8) Status levitation profile (lift + suspend, no freeze)

`statuses/statuses.json` can extend a `core:high_jump` status with:

- `levitationProfile.mode`: `none | lift_suspend`
- `liftSpeed`, `holdHeight`
- `riseFrames`, `suspendFrames`
- `lockHorizontal`
- `gravityScaleWhileActive`

When `mode=lift_suspend`, targets are lifted and suspended by status runtime without using freeze/stun timers.

## 9) Movement profiles

`movement/movement_profiles.json` supports modifier and surf-oriented behavior with profile-defined parameters (speed/drag/slope/board behavior).

## 10) Hooks and behavior actions

- Hook list: `docs/events.json` (`hooks`)
- Behavior events: `docs/events.json` (`behaviorEvents`)
- Action names: `docs/actions.json`
- New lifecycle hooks include:
  - `onStatusApplied`, `onStatusTick`, `onStatusExpired`
  - `onStateApplied`, `onStateRemoved`

## 11) FX presets, states and spells catalogs

- `fx/fx_presets.json`: reusable action bundles, invoked by `fx.spawnPreset`.
- `states/states.json`: actor/player-oriented state definitions with `apply[]` and `remove[]`.
- `spells/spells.json`: reusable effect pipelines with cooldown and optional targeting profile.

These catalogs are capability-gated by:

- `fx.presets.v1`
```
```text
- `states.catalog.v1`
- `spells.catalog.v1`

## 12) v6 UI/container/actors/AI/nav/debug contracts

New capabilities:

- `ui.runtime.v1`, `ui.hud.v1`, `ui.inventory_ext.v1`
- `containers.v1`, `recipes.processing.v1`, `interactions.v1`
- `actors.archetypes.v1`, `actors.adapters.v1`
- `ai.behavior_trees.v1`, `ai.sensors.v1`
- `nav.routes.v1`, `nav.navmesh_bridge.v1`
- `debug.overlay.v1`

Common action families:

- `ui.*` (`openScreen`, `closeScreen`, `toggleScreen`, `focusNext`, `focusPrev`)
- `inventoryExt.*` (`createPage`, `moveItem`, `save`, `load`)
- `container.*` (`open`, `moveItem`, `startProcess`, `cancelProcess`, `getProgress`)
- `actors.*` (`spawnArchetype`, `despawnArchetype`)
- `interactions.invoke`
- `ai.*`, `sense.*`, `nav.*`, `debug.*`

New hook events:

- `onUiScreenOpened`, `onUiScreenClosed`, `onUiAction`
- `onContainerSlotChanged`, `onProcessStart`, `onProcessTick`, `onProcessComplete`
- `onArchetypeSpawn`, `onArchetypeDespawn`, `onInteraction`
- `onBehaviorNodeChanged`, `onPathRequested`, `onPathFailed`

## 13) Validation workflow

1. Validate schema/capabilities via runtime load.
2. Check logs for contextual parser/runtime errors.
3. Run `tools/external_mods/modtool.ps1 report` to emit validate+doctor diagnostics and regenerate docs indexes.
4. Use `modtool pack -ReleaseChannel <stable|beta|testers>` for deterministic channel-tagged artifacts.
3. Re-export references after contract changes.

```powershell
tools/external_mods/export_runtime_reference.ps1
```

## 14) Example packs

See `docs/examples/external_mods/` for runnable reference mods.

## 15) v7 world graphics contracts

New capabilities:

- `render.materials.v1`
- `render.pbr.v1`
- `render.lighting.v1`
- `render.postfx.v1`
- `world.scenes.v1`
- `world.rooms.v1`
- `assets.packs.v2`
- `debug.render_inspector.v1`

Capability-gated manifest fields:

- `materialDefinitions` -> `materials/materials.json`
- `pbrDefinitions` -> `render/pbr_profiles.json`
- `lightingDefinitions` -> `lighting/light_profiles.json`
- `postFxDefinitions` -> `render/postfx_presets.json`
- `sceneProfileDefinitions` -> `world/scene_profiles.json`
- `roomProfileDefinitions` -> `world/room_profiles.json`
- `assetPackDefinitions` -> `assets/packs.json`
- `renderInspectorDefinitions` -> `debug/render_inspector.json`

v7 action family additions:

- `world.setSceneProfile`
- `world.setRoomProfile`
- `render.setPostFxPreset`
- `render.spawnLight`
- `render.setSkylight`
- `render.overrideMaterial`

`render/pbr_profiles.json` supports optional SSAO controls:

```json
"ambientOcclusion": {
  "enabled": true,
  "quality": "low|medium|high",
  "radius": 0.55,
  "intensity": 0.9,
  "bias": 0.02,
  "power": 1.2,
  "maxDistance": 1200.0,
  "blurPasses": 2
}
```

Runtime precedence:

1. `gEnhancements.Graphics.AO.Enabled=0` disables AO globally.
2. If enabled globally, profile values are used (plus `AO.IntensityScale` multiplier).
3. If no profile is active, quality/CVar fallback is applied.

Backend support:

- OpenGL: SSAO pass supported.
- DirectX11: SSAO pass supported.
- Metal: fallback to non-AO rendering with `onRenderFallbackApplied`.

`render/postfx_presets.json` also supports optional volumetrics controls through `volumetrics`:

```json
"volumetrics": {
  "enabled": true,
  "quality": "low|medium|high",
  "density": 0.22,
  "anisotropy": 0.55,
  "startDistance": 35.0,
  "maxDistance": 1300.0,
  "heightFogEnabled": true,
  "baseHeight": -20.0,
  "heightFalloff": 0.0035,
  "lightShaftIntensity": 1.4,
  "shadowIntensity": 0.75,
  "temporalBlend": 0.82,
  "jitterScale": 0.9,
  "debugView": false
}
```

`lighting/light_profiles.json` now drives volumetric lights and per-light shadow budgets with:

- `type`: `directional|point|spot`
- `direction`, `innerConeDeg`, `outerConeDeg`
- `castShadows`
- `shadowResolution`, `shadowBias`, `shadowNormalBias`, `shadowRange`
- `volumetricIntensity`

Runtime/backend behavior for volumetrics:

- OpenGL: volumetrics pass supported.
- DirectX11: volumetrics pass supported.
- Metal: explicit fallback with `onRenderFallbackApplied` when the backend cannot run the pass.

Global volumetrics overrides:

- `gEnhancements.Graphics.Volumetrics.Enabled`
- `gEnhancements.Graphics.Volumetrics.Quality` (`0..3`, `0=auto`)
- `gEnhancements.Graphics.Volumetrics.DensityScale`
- `gEnhancements.Graphics.Volumetrics.ShadowQuality`
- `gEnhancements.Graphics.Volumetrics.DebugView`

Notes:

- SSAO is applied before volumetrics when both are active.
- The first volumetrics implementation forces the affected frame onto the single-sample path instead of combining with MSAA.
- See runnable examples in:
  - `docs/examples/external_mods/world_profiles/render/postfx_presets.json`
  - `docs/examples/external_mods/world_profiles/lighting/light_profiles.json`
  - `docs/examples/external_mods/lighting_kit/lighting/light_profiles.json`

v7 extended hooks:

- `onWorldSceneLoaded`
- `onWorldRoomEntered`
- `onWorldRoomExited`
- `onWorldOverworldTick`
- `onWorldTimeOfDayChanged`
- `onWorldSkyboxChanged`

## 16) v9 persistence + narrative + dev iteration contracts

### New capabilities

Persistence / living world:

- `world.persistence.v1`
- `world.storage.v1`
- `world.spawn_profiles.v1`
- `world.time_weather.v1`
- `world.seeding.v1`
- `world.migrations.v1`
- `debug.persistence_inspector.v1`

Narrative:

- `narrative.timeline.v1`
- `narrative.dialogue.v1`
- `narrative.quests.v1`
- `narrative.flags.v1`
- `debug.narrative_inspector.v1`

Dev / sandbox:

- `dev.hot_reload.v1`
- `dev.console.v1`
- `dev.watchers.v1`
- `wasm.sandbox.v2`
- `debug.reload_inspector.v1`

### New action families

- Persistence: `persist.*` (`ensureEntityGuid`, `saveEntityState`, `loadEntityState`, `deleteEntityState`, `setDomainValue`, `getDomainValue`, `runMigrations`)
- World runtime: `world.spawnFromProfile`, `world.time.setOverride`, `world.weather.setOverride`
- Narrative: `narrative.*` (`startDialogue`, `chooseOption`, `advanceDialogue`, `setFlag`, `clearFlag`, `startQuest`, `updateObjective`, `startTimeline`, `skipTimeline`)
- Dev: `dev.reloadAll`, `dev.reloadTarget`, `dev.console.exec`

### New hooks

- `onPersistentEntityLoaded`
- `onPersistentEntitySaved`
- `onPersistentDomainMigrated`
- `onWorldSpawnProfileTick`
- `world.time.segmentChanged`
- `world.weather.changed`
- `onDialogueStarted`
- `onDialogueChoiceCommitted`
- `onQuestStateChanged`
- `onTimelineStarted`
- `onTimelineCompleted`
- `onTimelineSkipped`
- `onHotReloadApplied`
- `onHotReloadFailed`
```
```text
- `onSandboxBudgetExceeded`
- `onSandboxPermissionDenied`

### Persistence backend path (v9)

External mod persistence is stored outside vanilla save sections:

`<save_root>/external_mods/persist/<worldSlotId>/<modId>/`

See also:
- `docs/PERSISTENCE_GUIDE.md`
- `docs/NARRATIVE_GUIDE.md`
- `docs/HOT_RELOAD_GUIDE.md`
```

## file: docs/examples/external_mods/README.md
```text
# External Mods Examples

Runnable data-driven examples:

- `sss`
- `sss_content_pack`
- `ui_kit`
- `container_kit`
- `ai_templates`
- `lighting_kit`
- `world_profiles`
- `material_helpers`
- `persistence_kit`
- `narrative_kit`
- `devtools_kit`
- `framework_no_items_demo`
- `unresolved_reference_demo`
- `live_settings_realtime_demo`
- `compat_wrapper_demo`
- `demo_mini_inventory_h`
- `demo_bag_inventory_ext`
- `demo_furnace_system`
- `demo_npc_patrol`
- `demo_enemy_templates`
- `demo_vanilla_adapters`
- `demo_room_profiles`
- `demo_actor_lights`
- `demo_green_fire_magic`
- `demo_persistent_tents`
- `demo_loot_respawn`
- `demo_dialogue_npc`
- `demo_cutscene_intro`
- `demo_quest_chain`
- `demo_hot_reload_playground`
- `aim_ots_toggle_demo`
- `axe_prop_cut_demo`
- `fire_staff_demo`
- `freeze_dome_staff_demo`
- `freeze_staff_demo`
- `pistol_hitscan_demo`
- `shield_skate_demo`
- `spear_lunge_demo`
- `status_catalog_demo`

Reference authoring docs for v7 suite:

- `v7_reference_suite_docs/PACK_AUTHOR_GUIDE.md`
- `v7_reference_suite_docs/FRAMEWORK_AUTHOR_GUIDE.md`
- `v7_reference_suite_docs/DEMO_PLAYBOOK.md`

To copy these examples into runtime `mods/` for local testing:

```powershell
pwsh ./tools/external_mods/sync_examples_to_runtime.ps1 -Clean
```

To scaffold a new framework/content/pack template:

```powershell
pwsh ./tools/external_mods/modtool.ps1 init -Template framework -Path docs/examples/external_mods/my_framework
```
```

## file: docs/examples/external_mods/live_settings_realtime_demo/mod.json
```text
{
  "id": "com.sylian.live_settings_realtime_demo",
  "name": "Live Settings Realtime Demo",
  "version": "0.1.0",
  "apiVersion": 4,
  "type": "content",
  "uiCategory": "mod",
  "loadPriority": 125,
  "entryScript": "scripts/init.json",
  "runtime": {
    "type": "wasm3-v1",
    "module": "scripts/noop.wat",
    "maxMemoryKb": 512,
    "maxCallMs": 2,
    "maxFrameBudgetMs": 2,
    "maxHookCallsPerFrame": 128,
    "maxActorInstances": 0,
    "maxActiveStatuses": 0
  },
  "dependencies": [],
  "capabilities": [
    "hooks.extended.v1"
  ],
  "capabilityRationales": {
    "hooks.extended.v1": "Listens to settings.changed so the demo can confirm realtime/queued apply events."
  },
  "hookDefinitions": "hooks/hooks.json",
  "settings": {
    "schemaVersion": 1,
    "schemaFile": "settings/settings.schema.json",
    "defaultDomain": "global"
  },
  "engineVersionRange": ">=9.1.2 <10.0.0"
}
```


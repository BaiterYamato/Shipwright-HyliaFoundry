# Context Pack
generated=2026-03-11T00:07:54.3819866-03:00
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
    "lastUpdatedUtc":  "2026-03-11T00:34:01Z",
    "stats":  {
                  "entryCount":  177,
                  "openCount":  74,
                  "doneCount":  102,
                  "deprecatedCount":  1,
                  "topicCount":  84,
                  "tagCount":  76
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
                 "mem-20260311-000003":  {
                                             "topic":  "external-mod input parser missing shift aliases",
                                             "status":  "open",
                                             "line":  176,
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
```
```text
                                             "status":  "done",
                                             "line":  44,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000004":  {
                                             "topic":  "plan:PLN-20260310-0003",
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
```

## file: soh/soh/Enhancements/external-mods/ExternalModTypes.h
```text
#pragma once

#include <array>
#include <cstdint>
#include <filesystem>
#include <memory>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Ship {
class Archive;
}

namespace SOH {

class ExternalModWasmRuntime;

enum class ExternalModInputTriggerType {
    Pressed,
    Held,
    Released,
};

enum class ExternalModItemSlot {
    Stick,
    Nut,
    Bomb,
    Bow,
    FireArrow,
    DinsFire,
    Slingshot,
    Ocarina,
    Bombchu,
    Hookshot,
    IceArrow,
    FaroresWind,
    Boomerang,
    Lens,
    Bean,
    Hammer,
    LightArrow,
    NayrusLove,
    Bottle1,
    Bottle2,
    Bottle3,
    Bottle4,
    TradeAdult,
    TradeChild,
};

enum class ExternalModItemAgePolicy {
    RespectVanilla,
    AllowChild,
    AllowAdult,
};

enum class ExternalModItemUseMode {
    Vanilla,
    Override,
    Augment,
};

enum class ExternalModItemUseTrigger {
    OnUse,
    HammerGroundImpact,
};

enum class ExternalModItemPlacement {
    Legacy,
    Virtual,
};

enum class ExternalModMovementMode {
    Modifier,
    Surf,
};

enum class ExternalModAimCameraContext {
    CUp,
    Bow,
    Hookshot,
    Slingshot,
    Boomerang,
};

enum class ExternalModAimCameraMode {
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimMouseButton {
    Left,
    Middle,
    Right,
    Backward,
    Forward,
};

enum class ExternalModAimMouseFireMode {
    Both,
    FirstPerson,
    OverShoulder,
};

enum class ExternalModAimReticleVisibility {
    AimOnly,
    ButtonHold,
    Selected,
};

enum class ExternalModModelUvOrigin {
    Auto,
    BottomLeft,
    TopLeft,
};

enum class ExternalModModelTextureFilter {
    Auto,
    Point,
    Bilerp,
};

enum class ExternalModActorArchetype {
    Npc,
    Prop,
    Trigger,
};

enum class ExternalModRuntimeModuleFormat {
    WasmBinary,
    WatText,
};

enum class ExternalModIssueSeverity {
    Info,
    Warn,
    Error,
    Fatal,
};

struct ExternalModIssue {
    ExternalModIssueSeverity severity = ExternalModIssueSeverity::Info;
    std::string code;
    std::string message;
    std::string sourcePath;
    std::string registryId;
    std::string referencedId;
    std::string suggestedFix;
};

struct ExternalModSeveritySummary {
    int32_t info = 0;
    int32_t warn = 0;
    int32_t error = 0;
    int32_t fatal = 0;
};

enum class ExternalModSettingValueType {
    Bool,
    Int,
    Float,
    Enum,
    String,
    Color,
    Keybind,
};

enum class ExternalModSettingDomain {
    Global,
    Save,
    Session,
};

enum class ExternalModSettingApplyMode {
    Realtime,
    SceneReload,
    Restart,
};

struct ExternalModSettingEntryDefinition {
    std::string key;
    std::string label;
    std::string help;
    std::string defaultValue;
    ExternalModSettingValueType type = ExternalModSettingValueType::String;
    float minValue = 0.0f;
    float maxValue = 0.0f;
    float stepValue = 0.0f;
    bool hasMin = false;
    bool hasMax = false;
    bool hasStep = false;
    std::vector<std::string> enumValues;
    ExternalModSettingDomain domain = ExternalModSettingDomain::Global;
    ExternalModSettingApplyMode applyMode = ExternalModSettingApplyMode::Realtime;
    std::string requiresCapability;
    std::string requiresRegistry;
    bool experimental = false;
};

struct ExternalModSettingGroupDefinition {
    std::string id;
    std::string label;
    int32_t order = 0;
    std::vector<ExternalModSettingEntryDefinition> entries;
};

struct ExternalModSettingsSchemaDefinition {
    bool valid = false;
    int32_t version = 1;
    std::vector<ExternalModSettingGroupDefinition> groups;
};

struct ExternalModManifest {
    std::string id;
    std::string name;
    std::string version;
    std::string type = "content";
    std::string uiCategory = "mod";
```
```text
    int32_t apiVersion = 0;
    std::string gameVersionMin;
    std::string engineVersionRange;
    std::string entryScript;
    std::vector<std::string> assets;
    struct Dependency {
        std::string modId;
        std::string versionRange;
    };
    std::vector<Dependency> dependencies;
    std::vector<std::string> releaseChannels;
    std::unordered_map<std::string, std::string> capabilityRationales;
    std::unordered_map<std::string, std::string> permissionRationales;
    std::vector<std::string> permissions;
    std::vector<std::string> provides;
    std::vector<std::string> uses;
    struct FilesPolicy {
        std::vector<std::string> optional;
    } files;
    struct SettingsPolicy {
        bool enabled = false;
        int32_t schemaVersion = 0;
        std::string schemaFile;
        std::string defaultDomain = "global";
    } settings;
    int32_t loadOrder = 0;
    int32_t loadPriority = 0;
    struct Entrypoints {
        std::string items;
        std::string combat;
        std::string movement;
        std::string camera;
        std::string ui;
        std::string actors;
        std::string quests;
        std::string wasm;
    } entrypoints;

    std::string runtimeType;
    std::string runtimeModule;
    int32_t runtimeMaxMemoryKb = 1024;
    int32_t runtimeMaxCallMs = 2;
    int32_t runtimeMaxFrameBudgetMs = 2;
    int32_t runtimeMaxHookCallsPerFrame = 256;
    int32_t runtimeMaxActorInstances = 64;
    int32_t runtimeMaxActiveStatuses = 256;
    std::string itemDefinitions;
    std::string inputDefinitions;
    std::string hookDefinitions;
    std::string actorDefinitions;
    std::string behaviorDefinitions;
    std::string sceneDefinitions;
    std::string statusDefinitions;
    std::string damageDefinitions;
    std::string targetingDefinitions;
    std::string projectileDefinitions;
    std::string aoeDefinitions;
    std::string movementDefinitions;
    std::string itemUseProfiles;
    std::string vanillaItemPatches;
    std::string cameraDefinitions;
    std::string itemStateDefinitions;
    std::string equippedModelDefinitions;
    std::string hudWidgetDefinitions;
    std::string hudReticleDefinitions;
    std::string uiScreenDefinitions;
    std::string uiHudDefinitions;
    std::string playerResourceDefinitions;
    std::string resourceRingDefinitions;
    std::string inventoryExtensionDefinitions;
    std::string containerDefinitions;
    std::string recipeDefinitions;
    std::string interactionDefinitions;
    std::string actorArchetypeDefinitions;
    std::string actorAdapterDefinitions;
    std::string behaviorTreeDefinitions;
    std::string sensorDefinitions;
    std::string routeDefinitions;
    std::string navBridgeDefinitions;
    std::string debugOverlayDefinitions;
    std::string effectGraphDefinitions;
    std::string combatHitRuleDefinitions;
    std::string surfDefinitions;
    std::string actorTagDefinitions;
    std::string worldPatchDefinitions;
    std::string questDefinitions;
    std::string dialogDefinitions;
    std::string sdkGeneratorDefinitions;
    std::string fxPresetDefinitions;
    std::string stateDefinitions;
    std::string spellDefinitions;
    std::string materialDefinitions;
    std::string pbrDefinitions;
    std::string lightingDefinitions;
    std::string postFxDefinitions;
    std::string sceneProfileDefinitions;
    std::string roomProfileDefinitions;
    std::string assetPackDefinitions;
    std::string renderInspectorDefinitions;
    std::string editorRuntimeDefinitions;
    std::string editorUiDefinitions;
    std::string editorSelectionDefinitions;
    std::string editorGizmoDefinitions;
    std::string editorLibraryDefinitions;
    std::string editorProjectDefinitions;
    std::string editorPlacementDefinitions;
    std::string assetImporterDefinitions;
    std::string worldAuthoringDefinitions;
    std::string collisionAuthoringDefinitions;
    std::string editorInspectorDefinitions;
    std::string worldPersistenceDefinitions;
    std::string worldStorageDefinitions;
    std::string worldSpawnProfileDefinitions;
    std::string worldTimeWeatherDefinitions;
    std::string worldSeedingDefinitions;
    std::string worldMigrationDefinitions;
    std::string persistenceInspectorDefinitions;
    std::string narrativeTimelineDefinitions;
    std::string narrativeDialogueDefinitions;
    std::string narrativeQuestDefinitions;
    std::string narrativeFlagDefinitions;
    std::string narrativeInspectorDefinitions;
    std::string devHotReloadDefinitions;
    std::string devConsoleDefinitions;
    std::string devWatcherDefinitions;
    std::string wasmSandboxDefinitions;
    std::string reloadInspectorDefinitions;
    std::vector<std::string> capabilities;
};

enum class ExternalModActionType {
    ShowNotification,
    TeleportToEntrance,
    LoadModScene,
    PressButton,
    ShowEquippedItemGet,
    SpawnSmoke,
    SpawnKusa,
    LanternLight,
    ApplyStatus,
    UseItemProfile,
    DealDamage,
    SpawnProjectile,
    SpawnAoE,
    ApplyMovementProfile,
    ApplyImpulse,
    GetGroundInfo,
    Raycast,
    RaycastAll,
    ClearStatus,
    ClearAllStatuses,
    SpawnActor,
    DespawnActor,
    SetActorState,
    MoveActorToPathNode,
    OpenDialog,
    SetSwitchFlag,
    ClearSwitchFlag,
    SetEventChkInf,
    ClearEventChkInf,
    SetInfTable,
    ClearInfTable,
    GiveRupees,
    TakeRupees,
    GrantModItem,
    RevokeModItem,
    SetVar,
    AddVar,
    ClampVar,
    EmitSignal,
    CallBehavior,
    ToggleAimCameraMode,
    SetAimCameraMode,
    SetAimCameraProfile,
    InvokeWasm,
    FxSpawnEffectSs,
    FxSpawnActorFx,
    FxSpawnPreset,
    FxStopFx,
    StatesApplyState,
    StatesClearState,
    StatesHasState,
    PlayerGetStateFlags,
    PlayerSetStateFlag,
    PlayerClearStateFlag,
    PlayerSetControlLock,
    PlayerSetGravityScale,
    PlayerSetBoostType,
    PlayerSetDamageResponse,
    SpellsCastSpell,
    UiOpenScreen,
    UiCloseScreen,
    UiToggleScreen,
    UiFocusNext,
    UiFocusPrev,
    InventoryExtCreatePage,
    InventoryExtMoveItem,
    InventoryExtSave,
    InventoryExtLoad,
    ContainerOpen,
    ContainerMoveItem,
    ContainerStartProcess,
    ContainerCancelProcess,
    ContainerGetProgress,
    ActorsSpawnArchetype,
    ActorsDespawnArchetype,
    ActorsToggleArchetype,
    InteractionsInvoke,
    AiRunBehavior,
    AiSetBlackboard,
    AiClearBlackboard,
    SenseFindTargets,
    SenseLineOfSight,
    SenseDistance,
    NavRequestPath,
    NavGetPathPoints,
    NavReleasePath,
    DebugShowOverlay,
    DebugHideOverlay,
    WorldSetSceneProfile,
```
```text
    WorldSetRoomProfile,
    RenderSetPostFxPreset,
    RenderSpawnLight,
    RenderDespawnLight,
    RenderSetSkylight,
    RenderClearPostFxPreset,
    RenderClearSkylight,
    RenderOverrideMaterial,
    RenderClearMaterialOverrides,
    PersistEnsureEntityGuid,
    PersistSaveEntityState,
    PersistLoadEntityState,
    PersistDeleteEntityState,
    PersistSetDomainValue,
    PersistGetDomainValue,
    PersistRunMigrations,
    WorldSpawnFromProfile,
    WorldTimeSetOverride,
    WorldWeatherSetOverride,
    NarrativeStartDialogue,
    NarrativeChooseOption,
    NarrativeAdvanceDialogue,
    NarrativeSetFlag,
    NarrativeClearFlag,
    NarrativeStartQuest,
    NarrativeUpdateObjective,
    NarrativeStartTimeline,
    NarrativeSkipTimeline,
    DevReloadAll,
    DevReloadTarget,
    DevConsoleExec,
    SettingsGet,
    SettingsSet,
    SettingsReset,
    SettingsList,
    SetResourceValue,
    AddResourceValue,
    ConsumeResource,
    RefillResource,
    SetResourceCapacity,
};

enum class ExternalModStatusType {
    Fire,
    Freeze,
    Stun,
    Poison,
    Blind,
    Speed,
    Slow,
    HighJump,
    Strength,
    Weakness,
    Custom,
};

enum class ExternalModStatusTarget {
    FrontTarget,
    Self,
    Player,
    ActorHandle,
};

struct ExternalModAction {
    ExternalModActionType type = ExternalModActionType::ShowNotification;
    std::string text;
    int16_t entranceIndex = 0;
    std::string modSceneId;
    int32_t sceneSpawnId = 0;
    int32_t buttonMask = 0;
    std::string itemId;
    std::string exportName;
    std::vector<int32_t> args;

    std::string actorDefinitionId;
    uint32_t actorHandle = 0;
    std::string actorStateKey;
    std::string actorStateValue;
    int32_t pathNodeIndex = 0;
    int32_t dialogId = 0;
    int32_t intValue = 0;

    std::string variableScope;
    std::string variableKey;
    std::string variableValue;
    float variableNumber = 0.0f;
    float variableMin = 0.0f;
    float variableMax = 0.0f;
    bool variableHasNumber = false;
    bool variableHasRange = false;
    std::string signalName;
    std::string behaviorId;
    ExternalModStatusType statusType = ExternalModStatusType::Freeze;
    ExternalModStatusTarget statusTarget = ExternalModStatusTarget::FrontTarget;
    bool hasStatusTarget = false;
    int32_t durationFrames = 90;
    int32_t tickFrames = 15;
    int32_t damagePerTick = 1;
    int32_t shakeFrames = 12;
    float freezeRange = 180.0f;
    int32_t intensity = 255;
    int32_t blueIntensity = 255;
    float speedMultiplier = 0.5f;
    float jumpMultiplier = 1.5f;
    float strengthMultiplier = 2.0f;
    float weaknessMultiplier = 2.0f;
    float blindSkipChance = 0.35f;
    float blindYawJitterDeg = 20.0f;
    std::string statusId;
    std::string damageProfileId;
    std::string targetingProfileId;
    std::string itemUseProfileId;
    std::string projectileProfileId;
    std::string aoeProfileId;
    std::string movementProfileId;
    std::string fxPresetId;
    std::string stateId;
    std::string spellId;
    std::string aimCameraProfileId;
    ExternalModAimCameraMode aimCameraMode = ExternalModAimCameraMode::FirstPerson;
    std::string patchOperation;
    float range = 180.0f;
    float angle = 0.0f;
    float radius = 0.0f;
    float impulseStrength = 0.0f;
    std::string fxEffectName;
    int32_t fxEffectId = -1;
    int32_t fxActorId = -1;
    std::string fxOverlayName;
    float fxScale = 1.0f;
    int32_t fxLifeFrames = 20;
    bool fxAttachFollow = false;
    int32_t fxSpawnEveryFrames = 0;
    std::string fxStoreKey;
    std::string fxHandleKey;
    std::string stateDomain;
    std::string stateFlag;
    std::string stateControlMode;
    std::string uiScreenId;
    std::string resourceId;
    std::string inventoryPageId;
    int32_t inventorySlotCount = 0;
    std::string sourceBinding;
    std::string destinationBinding;
    int32_t moveCount = 0;
    std::string containerId;
    std::string recipeId;
    std::string interactionId;
    std::string archetypeId;
    std::string spawnOrigin;
    float forwardDistance = 0.0f;
    float upOffset = 0.0f;
    std::string behaviorTreeId;
    std::string blackboardKey;
    std::string blackboardValue;
    std::string sensorType;
    std::string routeId;
    std::string navHandleKey;
    std::string overlayId;
    std::string renderProfileId;
    std::string renderMatch;
    std::string renderScope;
    std::string renderAttach;
    int32_t sceneId = -1;
    int32_t roomId = -1;
    int32_t durationMs = 0;
    float blendValue = 1.0f;
    bool renderFollow = true;
    bool hasWorldPos = false;
    float worldPosX = 0.0f;
    float worldPosY = 0.0f;
    float worldPosZ = 0.0f;
    bool boolValue = false;
    bool hasBoolValue = false;
    float floatValue = 0.0f;
    bool hasFloatValue = false;
    std::string persistenceDomainId;
    std::string persistenceEntityGuid;
    std::string persistenceEntityScope;
    std::string persistenceStateKey;
    std::string persistenceStateValue;
    std::string migrationId;
    std::string spawnProfileId;
    std::string timeSegmentId;
    std::string weatherProfileId;
    std::string dialogueId;
    std::string dialogueNodeId;
    std::string dialogueOptionId;
    std::string questId;
    std::string questObjectiveId;
    std::string questState;
    std::string timelineId;
    std::string consoleCommandId;
    std::string settingsKey;
    std::string settingsValue;
    std::string settingsDomain;
    std::string settingsApplyMode;
    bool settingsResetAll = false;
};

struct ExternalModSceneAction {
    int16_t sceneId = 0;
    std::vector<ExternalModAction> actions;
};

struct ExternalModTriggerVolume {
    std::string id;
    int16_t sceneId = 0;
    float minX = 0.0f;
    float minY = 0.0f;
    float minZ = 0.0f;
    float maxX = 0.0f;
    float maxY = 0.0f;
    float maxZ = 0.0f;
    int32_t cooldownFrames = 0;
    int32_t cooldownRemaining = 0;
    bool wasInside = false;
    std::vector<ExternalModAction> actions;
};

```

## file: soh/soh/Enhancements/external-mods/ExternalModManager.cpp
```text
#include "ExternalModManager.h"

#include <algorithm>
#include <array>
#include <chrono>
#include <cmath>
#include <cctype>
#include <cstring>
#include <functional>
#include <fstream>
#include <iomanip>
#include <limits>
#include <map>
#include <queue>
#include <regex>
#include <sstream>
#include <set>
#include <unordered_map>
#include <unordered_set>

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>
#include <zip.h>
#include <stb_image.h>

#include <ship/Context.h>
#include <ship/controller/controldeck/ControlDeck.h>
#include <ship/controller/controldevice/controller/Controller.h>
#include <ship/controller/controldevice/controller/ControllerButton.h>
#include <ship/controller/controldevice/controller/mapping/keyboard/KeyboardKeyToButtonMapping.h>
#include <ship/controller/controldevice/controller/mapping/keyboard/KeyboardScancodes.h>
#include <ship/resource/File.h>
#include <ship/resource/archive/Archive.h>
#include <fast/resource/type/DisplayList.h>
#include <fast/resource/ResourceType.h>

#include "ExternalModItemRuntime.h"
#include "ExternalModContentRegistry.h"
#include "ExternalModInterop.h"
#include "ExternalModWorldGraphicsRuntime.h"
#include "ExternalModWatCompiler.h"
#include "ExternalModWasmRuntime.h"
#include "soh/SaveManager.h"
#include "soh/Enhancements/game-interactor/GameInteractor.h"
#include "soh/Notification/Notification.h"
#include "soh/OTRGlobals.h"
#include "soh/ResourceManagerHelpers.h"
#include "objects/object_link_boy/object_link_boy.h"
#include "objects/object_gi_shield_2/object_gi_shield_2.h"
#include "src/overlays/actors/ovl_En_Kusa/z_en_kusa.h"
#include "src/overlays/actors/ovl_En_Wood02/z_en_wood02.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "variables.h"
#include "functions.h"

GetItemEntry ItemTable_Retrieve(int16_t getItemID);
GetItemID RetrieveGetItemIDFromItemID(ItemID itemID);

extern SaveContext gSaveContext;
extern PlayState* gPlayState;
void Interface_DrawItemIconTexture(PlayState* play, void* texture, s16 button);
void Interface_DrawAmmoCount(PlayState* play, s16 button, s16 alpha);
void Player_StartMode_Idle(PlayState* play, Player* thisx);
}

namespace SOH {

namespace {
constexpr int32_t kExternalModApiVersionMin = 4;
constexpr int32_t kExternalModApiVersionMax = 4;
constexpr int32_t kExternalModApiVersionV4 = 4;
constexpr uint64_t kMaxManifestBytes = 256 * 1024;
constexpr uint64_t kMaxScriptBytes = 1024 * 1024;
constexpr uint64_t kMaxAssetBytes = 512ull * 1024ull * 1024ull;
constexpr uint64_t kMaxWasmBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxWatSourceBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxInputDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxActorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxHookDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxStatusDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxDamageDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxTargetingDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxUseProfileDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxProjectileDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxAoEDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxMovementDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxCameraDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxVanillaPatchDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxItemStateDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxEquippedModelDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxHudWidgetDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxReticleDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxUiScreenDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxUiHudDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxPlayerResourceDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxResourceRingDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxInventoryExtensionDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxContainerDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxRecipeDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxInteractionDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxActorArchetypeDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxActorAdapterDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxBehaviorTreeDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxSensorDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxRouteDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxNavBridgeDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxDebugOverlayDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxEffectGraphDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxCombatHitRulesBytes = 512 * 1024;
constexpr uint64_t kMaxSurfDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxActorTagDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxWorldPatchDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxQuestDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxDialogDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxSdkGeneratorDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxFxPresetDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxStateDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxSpellDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxMaterialDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxPbrDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxLightingDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxPostFxDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxSceneProfileDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxRoomProfileDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxAssetPackDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxRenderInspectorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxEditorRuntimeDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorUiDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorSelectionDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorGizmoDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorLibraryDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorProjectDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorPlacementDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxAssetImporterDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxWorldAuthoringDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxCollisionAuthoringDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxEditorInspectorDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxWorldPersistenceDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxWorldStorageDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxWorldSpawnProfileDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxWorldTimeWeatherDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxWorldSeedingDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxWorldMigrationDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxPersistenceInspectorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxNarrativeFlagDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxNarrativeDialogueDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxNarrativeQuestDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxNarrativeTimelineDefinitionBytes = 1024 * 1024;
constexpr uint64_t kMaxNarrativeInspectorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxDevHotReloadDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxDevConsoleDefinitionBytes = 512 * 1024;
constexpr uint64_t kMaxDevWatcherDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxWasmSandboxDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxReloadInspectorDefinitionBytes = 256 * 1024;
constexpr uint64_t kMaxItemIconBytes = 4ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemModelBytes = 8ull * 1024ull * 1024ull;
constexpr uint64_t kMaxItemModelTextureBytes = 16ull * 1024ull * 1024ull;
constexpr uint64_t kMaxObjMaterialBytes = 512 * 1024;
constexpr uint64_t kMaxHookshotTextureBytes = 4ull * 1024ull * 1024ull;
constexpr int32_t kItemIconSize = 32;
constexpr int32_t kItemModelTextureSize = 32;
constexpr int32_t kMaxDecodedIconDimension = 2048;
constexpr int32_t kHookshotMetalTextureWidth = 8;
constexpr int32_t kHookshotMetalTextureHeight = 8;
constexpr int32_t kHookshotHandleTextureWidth = 16;
constexpr int32_t kHookshotHandleTextureHeight = 8;
constexpr int32_t kHookshotDesignTextureWidth = 16;
constexpr int32_t kHookshotDesignTextureHeight = 32;
constexpr int32_t kHookshotChainTextureWidth = 16;
constexpr int32_t kHookshotChainTextureHeight = 32;
constexpr int32_t kHookshotReticleTextureWidth = 64;
constexpr int32_t kHookshotReticleTextureHeight = 64;
constexpr int32_t kAimReticleTextureWidth = 64;
constexpr int32_t kAimReticleTextureHeight = 64;
constexpr size_t kMaxItemModelTriangles = 4096;
constexpr float kMinDisplayListModelScale = 0.05f;
constexpr float kMaxDisplayListModelScale = 2000.0f;
constexpr int32_t kDefaultTriggerCooldownFrames = 90;
constexpr int32_t kDefaultRuntimeMemoryKb = 1024;
constexpr int32_t kDefaultRuntimeCallMs = 2;
constexpr int32_t kDefaultRuntimeFrameBudgetMs = 2;
constexpr int32_t kDefaultRuntimeHookCallsPerFrame = 256;
constexpr int32_t kDefaultRuntimeActorInstances = 64;
constexpr int32_t kDefaultRuntimeActiveStatuses = 256;
constexpr u8 kAgeReqAdult = LINK_AGE_ADULT;
constexpr u8 kAgeReqChild = LINK_AGE_CHILD;
constexpr u8 kAgeReqNone = 9;
constexpr size_t kItemIconTableSize = sizeof(gItemIcons) / sizeof(gItemIcons[0]);
constexpr size_t kExternalModInventoryCellsPerPage = 24;
constexpr size_t kExternalModInventoryMaxCellCount = 4096;
constexpr const char* kExternalModsInventorySaveSectionName = "externalModsInventory";
constexpr int32_t kExternalModsInventorySaveVersion = 1;
std::array<void*, kItemIconTableSize> gVanillaItemIcons{};
bool gVanillaItemIconsCaptured = false;

struct ExternalModHookshotTexturePatchRecord {
    std::string displayListPath;
    std::string patchName;
};

std::vector<ExternalModHookshotTexturePatchRecord> gExternalModHookshotTexturePatches{};
std::string gExternalModHookshotTextureOverrideKey;
std::unordered_set<std::string> gExternalModMissingDisplayListWarnings{};
std::unordered_set<std::string> gExternalModDisplayListDrawDebugLogs{};
bool gExternalModPlayerStatusOverridesApplied = false;
constexpr const char* kCoreFreezeNoDamageStatusId = "core:freeze_ice_trap_no_damage";
constexpr uint8_t kObjIcePolySizeSmall = 0;
constexpr uint8_t kObjIcePolySizeMedium = 1;
constexpr uint8_t kObjIcePolySizeLarge = 2;

void LoadExternalModsInventorySection() {
    ExternalModManager::Instance().LoadPersistentInventoryState();
}

void SaveExternalModsInventorySection(SaveContext* saveContext, int sectionID, bool fullSave) {
    (void)saveContext;
```
```text
    (void)sectionID;
    (void)fullSave;
    ExternalModManager::Instance().SavePersistentInventoryState();
}

const std::unordered_map<std::string, int16_t> kSceneAliases = {
    { "SCENE_KOKIRI_FOREST", SCENE_KOKIRI_FOREST },
    { "SCENE_LINKS_HOUSE", SCENE_LINKS_HOUSE },
    { "SCENE_KAKARIKO_VILLAGE", SCENE_KAKARIKO_VILLAGE },
};

const std::unordered_map<std::string, int16_t> kEntranceAliases = {
    { "ENTR_KOKIRI_FOREST_0", ENTR_KOKIRI_FOREST_0 },
    { "ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE", ENTR_KOKIRI_FOREST_OUTSIDE_LINKS_HOUSE },
    { "ENTR_LINKS_HOUSE_CHILD_SPAWN", ENTR_LINKS_HOUSE_CHILD_SPAWN },
    { "ENTR_LINKS_HOUSE_0_1", ENTR_LINKS_HOUSE_0_1 },
    { "ENTR_KAKARIKO_VILLAGE_FRONT_GATE", ENTR_KAKARIKO_VILLAGE_FRONT_GATE },
    { "ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK", ENTR_KAKARIKO_VILLAGE_OUTSIDE_POTION_SHOP_BACK },
};

int16_t ResolveSceneIdForEntranceIndex(int16_t entranceIndex) {
    const auto isValidEntranceIndex = [](int32_t index) {
        return index >= 0 && index < static_cast<int32_t>(ARRAY_COUNT(gEntranceTable));
    };

    const int32_t setupAdjustedIndex = static_cast<int32_t>(entranceIndex) + static_cast<int32_t>(gSaveContext.sceneSetupIndex);
    if (isValidEntranceIndex(setupAdjustedIndex)) {
        return gEntranceTable[setupAdjustedIndex].scene;
    }

    const int32_t directIndex = static_cast<int32_t>(entranceIndex);
    if (isValidEntranceIndex(directIndex)) {
        return gEntranceTable[directIndex].scene;
    }

    return static_cast<int16_t>(-1);
}

const std::unordered_map<std::string, int32_t> kButtonAliases = {
    { "BTN_A", BTN_A },
    { "BTN_B", BTN_B },
    { "BTN_Z", BTN_Z },
    { "BTN_START", BTN_START },
    { "BTN_DUP", BTN_DUP },
    { "BTN_DDOWN", BTN_DDOWN },
    { "BTN_DLEFT", BTN_DLEFT },
    { "BTN_DRIGHT", BTN_DRIGHT },
    { "BTN_L", BTN_L },
    { "BTN_R", BTN_R },
    { "BTN_CUP", BTN_CUP },
    { "BTN_CDOWN", BTN_CDOWN },
    { "BTN_CLEFT", BTN_CLEFT },
    { "BTN_CRIGHT", BTN_CRIGHT },
    { "BTN_CUSTOM_MODIFIER1", BTN_CUSTOM_MODIFIER1 },
    { "BTN_CUSTOM_MODIFIER2", BTN_CUSTOM_MODIFIER2 },
    { "BTN_CUSTOM_MOD_ACTION1", BTN_CUSTOM_MOD_ACTION1 },
    { "BTN_CUSTOM_MOD_ACTION2", BTN_CUSTOM_MOD_ACTION2 },
    { "BTN_CUSTOM_MOD_ACTION3", BTN_CUSTOM_MOD_ACTION3 },
    { "BTN_CUSTOM_MOD_ACTION4", BTN_CUSTOM_MOD_ACTION4 },
    { "BTN_CUSTOM_MOD_ACTION5", BTN_CUSTOM_MOD_ACTION5 },
    { "BTN_CUSTOM_MOD_ACTION6", BTN_CUSTOM_MOD_ACTION6 },
    { "BTN_CUSTOM_MOD_ACTION7", BTN_CUSTOM_MOD_ACTION7 },
    { "BTN_CUSTOM_MOD_ACTION8", BTN_CUSTOM_MOD_ACTION8 },
    { "BTN_CUSTOM_MOD_ACTION9", BTN_CUSTOM_MOD_ACTION9 },
    { "MOD_ACTION1", BTN_CUSTOM_MOD_ACTION1 },
    { "MOD_ACTION2", BTN_CUSTOM_MOD_ACTION2 },
    { "MOD_ACTION3", BTN_CUSTOM_MOD_ACTION3 },
    { "MOD_ACTION4", BTN_CUSTOM_MOD_ACTION4 },
    { "MOD_ACTION5", BTN_CUSTOM_MOD_ACTION5 },
    { "MOD_ACTION6", BTN_CUSTOM_MOD_ACTION6 },
    { "MOD_ACTION7", BTN_CUSTOM_MOD_ACTION7 },
    { "MOD_ACTION8", BTN_CUSTOM_MOD_ACTION8 },
    { "MOD_ACTION9", BTN_CUSTOM_MOD_ACTION9 },
    { "A", BTN_A },
    { "B", BTN_B },
    { "Z", BTN_Z },
    { "START", BTN_START },
    { "DUP", BTN_DUP },
    { "DDOWN", BTN_DDOWN },
    { "DLEFT", BTN_DLEFT },
    { "DRIGHT", BTN_DRIGHT },
    { "L", BTN_L },
    { "R", BTN_R },
    { "CUP", BTN_CUP },
    { "CDOWN", BTN_CDOWN },
    { "CLEFT", BTN_CLEFT },
    { "CRIGHT", BTN_CRIGHT },
};

const std::unordered_map<std::string, Ship::KbScancode> kKeyboardKeyAliases = {
    { "A", Ship::LUS_KB_A },   { "B", Ship::LUS_KB_B },   { "C", Ship::LUS_KB_C },   { "D", Ship::LUS_KB_D },
    { "E", Ship::LUS_KB_E },   { "F", Ship::LUS_KB_F },   { "G", Ship::LUS_KB_G },   { "H", Ship::LUS_KB_H },
    { "I", Ship::LUS_KB_I },   { "J", Ship::LUS_KB_J },   { "K", Ship::LUS_KB_K },   { "L", Ship::LUS_KB_L },
    { "M", Ship::LUS_KB_M },   { "N", Ship::LUS_KB_N },   { "O", Ship::LUS_KB_O },   { "P", Ship::LUS_KB_P },
    { "Q", Ship::LUS_KB_Q },   { "R", Ship::LUS_KB_R },   { "S", Ship::LUS_KB_S },   { "T", Ship::LUS_KB_T },
    { "U", Ship::LUS_KB_U },   { "V", Ship::LUS_KB_V },   { "W", Ship::LUS_KB_W },   { "X", Ship::LUS_KB_X },
    { "Y", Ship::LUS_KB_Y },   { "Z", Ship::LUS_KB_Z },   { "TAB", Ship::LUS_KB_TAB },
    { "0", Ship::LUS_KB_0 },   { "1", Ship::LUS_KB_1 },   { "2", Ship::LUS_KB_2 },   { "3", Ship::LUS_KB_3 },
    { "4", Ship::LUS_KB_4 },   { "5", Ship::LUS_KB_5 },   { "6", Ship::LUS_KB_6 },   { "7", Ship::LUS_KB_7 },
    { "8", Ship::LUS_KB_8 },   { "9", Ship::LUS_KB_9 },
    { "ESC", Ship::LUS_KB_ESCAPE },         { "ESCAPE", Ship::LUS_KB_ESCAPE },
    { "ENTER", Ship::LUS_KB_ENTER },        { "SPACE", Ship::LUS_KB_SPACE },
    { "BACKSPACE", Ship::LUS_KB_BACKSPACE }, { "SHIFT", Ship::LUS_KB_SHIFT },
    { "LSHIFT", Ship::LUS_KB_SHIFT },       { "LEFTSHIFT", Ship::LUS_KB_SHIFT },
    { "RSHIFT", Ship::LUS_KB_RSHIFT },      { "RIGHTSHIFT", Ship::LUS_KB_RSHIFT },
    { "CTRL", Ship::LUS_KB_CONTROL },       { "CONTROL", Ship::LUS_KB_CONTROL },
    { "LCTRL", Ship::LUS_KB_CONTROL },      { "LEFTCTRL", Ship::LUS_KB_CONTROL },
    { "ALT", Ship::LUS_KB_ALT },            { "LALT", Ship::LUS_KB_ALT },
    { "LEFTALT", Ship::LUS_KB_ALT },
    { "F1", Ship::LUS_KB_F1 }, { "F2", Ship::LUS_KB_F2 }, { "F3", Ship::LUS_KB_F3 }, { "F4", Ship::LUS_KB_F4 },
    { "F5", Ship::LUS_KB_F5 }, { "F6", Ship::LUS_KB_F6 }, { "F7", Ship::LUS_KB_F7 }, { "F8", Ship::LUS_KB_F8 },
    { "F9", Ship::LUS_KB_F9 }, { "F10", Ship::LUS_KB_F10 }, { "F11", Ship::LUS_KB_F11 }, { "F12", Ship::LUS_KB_F12 },
};

const std::unordered_set<Ship::KbScancode> kReservedDefaultKeyboardScancodes = {
    Ship::LUS_KB_F1, Ship::LUS_KB_F5, Ship::LUS_KB_F6, Ship::LUS_KB_F7, Ship::LUS_KB_F9, Ship::LUS_KB_TAB, Ship::LUS_KB_I,
};
const std::unordered_map<std::string, ExternalModAimMouseButton> kAimMouseButtonAliases = {
    { "left", ExternalModAimMouseButton::Left },
    { "middle", ExternalModAimMouseButton::Middle },
    { "right", ExternalModAimMouseButton::Right },
    { "backward", ExternalModAimMouseButton::Backward },
    { "forward", ExternalModAimMouseButton::Forward },
};

const std::unordered_map<std::string, ExternalModAimMouseFireMode> kAimMouseFireModeAliases = {
    { "both", ExternalModAimMouseFireMode::Both },
    { "firstperson", ExternalModAimMouseFireMode::FirstPerson },
    { "first_person", ExternalModAimMouseFireMode::FirstPerson },
    { "overshoulder", ExternalModAimMouseFireMode::OverShoulder },
    { "over_shoulder", ExternalModAimMouseFireMode::OverShoulder },
};

const std::unordered_map<std::string, ExternalModAimReticleVisibility> kAimReticleVisibilityAliases = {
    { "aim_only", ExternalModAimReticleVisibility::AimOnly },
    { "aimonly", ExternalModAimReticleVisibility::AimOnly },
    { "button_hold", ExternalModAimReticleVisibility::ButtonHold },
    { "buttonhold", ExternalModAimReticleVisibility::ButtonHold },
    { "selected", ExternalModAimReticleVisibility::Selected },
};

const std::unordered_set<ExternalModActionType> kSupportedCameraHotkeyActions = {
    ExternalModActionType::ToggleAimCameraMode,
    ExternalModActionType::SetAimCameraMode,
    ExternalModActionType::SetAimCameraProfile,
};

const std::unordered_set<ExternalModActionType> kSupportedHotkeyActions = {
    ExternalModActionType::ToggleAimCameraMode,
    ExternalModActionType::SetAimCameraMode,
    ExternalModActionType::SetAimCameraProfile,
    ExternalModActionType::ActorsToggleArchetype,
};
constexpr const char* kAimCameraOverShoulderCVar = "gExternalMods.AimCamera.OverShoulderEnabled";
constexpr const char* kCoreDefaultAimCameraProfileId = "core:ots_default";

const ExternalModAimCameraProfile& GetCoreDefaultAimCameraProfile() {
    static ExternalModAimCameraProfile profile = [] {
        ExternalModAimCameraProfile p;
        p.id = kCoreDefaultAimCameraProfileId;
        p.contextsMask = 0x1F;
        p.cUpFirstPersonMode = CAM_MODE_FIRSTPERSON;
        p.bowFirstPersonMode = CAM_MODE_BOWARROW;
        p.hookshotFirstPersonMode = CAM_MODE_HOOKSHOT;
        p.slingshotFirstPersonMode = CAM_MODE_SLINGSHOT;
        p.boomerangFirstPersonMode = CAM_MODE_BOWARROW;
        p.cUpOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.bowOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.hookshotOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.slingshotOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.boomerangOverShoulderMode = CAM_MODE_BOWARROWZ;
        p.shoulder = "right";
        p.aimRay = "camera_center";
        p.reticleX = 0.5f;
        p.reticleY = 0.5f;
        return p;
    }();
    return profile;
}

std::string ResolveProfileIdForMod(const std::string& modId, const std::string& profileId) {
    if (profileId.empty()) {
        return {};
    }
    if (profileId.find(':') != std::string::npos || modId.empty()) {
        return profileId;
    }
    return modId + ":" + profileId;
}

bool IsHigherPriorityProvider(const ExternalModPackage* lhs, const ExternalModPackage* rhs) {
    if (lhs == nullptr) {
        return false;
    }
    if (rhs == nullptr) {
        return true;
    }
    if (lhs->manifest.loadPriority != rhs->manifest.loadPriority) {
        return lhs->manifest.loadPriority > rhs->manifest.loadPriority;
    }
    return lhs->manifest.id < rhs->manifest.id;
}

template <typename TDefinition, typename TCollectionSelector>
const TDefinition* FindDefinitionAcrossPackages(const std::vector<ExternalModPackage>& packages, const std::string& id,
                                                TCollectionSelector&& selector,
                                                const ExternalModPackage** outOwnerPackage = nullptr) {
    const TDefinition* bestDefinition = nullptr;
    const ExternalModPackage* bestPackage = nullptr;

    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        const auto& collection = selector(package.runtime);
        const auto definitionIt =
            std::find_if(collection.begin(), collection.end(), [&id](const TDefinition& definition) { return definition.id == id; });
        if (definitionIt == collection.end()) {
            continue;
        }
```
```text
        if (!IsHigherPriorityProvider(&package, bestPackage)) {
            continue;
        }
        bestPackage = &package;
        bestDefinition = &(*definitionIt);
    }

    if (outOwnerPackage != nullptr) {
        *outOwnerPackage = bestPackage;
    }
    return bestDefinition;
}

std::string GetProfileOwnerModId(const std::string& profileId, const std::string& fallbackModId) {
    const auto separator = profileId.find(':');
    if (separator == std::string::npos || separator == 0) {
        return fallbackModId;
    }
    return profileId.substr(0, separator);
}

struct EquippedActionButtonMapping {
    int32_t mask;
    int32_t slotIndex;
    const char* alias;
};

constexpr std::array<EquippedActionButtonMapping, 7> kEquippedActionButtonMappings = {{
    { BTN_CLEFT, 1, "BTN_CLEFT" },
    { BTN_CDOWN, 2, "BTN_CDOWN" },
    { BTN_CRIGHT, 3, "BTN_CRIGHT" },
    { BTN_DUP, 4, "BTN_DUP" },
    { BTN_DDOWN, 5, "BTN_DDOWN" },
    { BTN_DLEFT, 6, "BTN_DLEFT" },
    { BTN_DRIGHT, 7, "BTN_DRIGHT" },
}};

bool TryResolveEquippedActionButtonMask(int32_t inputMask, int32_t& outResolvedMask, int32_t& outSlotIndex) {
    outResolvedMask = 0;
    outSlotIndex = 0;

    int32_t matches = 0;
    const auto normalizedMask = static_cast<uint32_t>(inputMask);
    for (const auto& mapping : kEquippedActionButtonMappings) {
        if ((normalizedMask & static_cast<uint32_t>(mapping.mask)) != 0) {
            outResolvedMask = mapping.mask;
            outSlotIndex = mapping.slotIndex;
            ++matches;
        }
    }

    return matches == 1;
}

std::string BuildSupportedEquippedActionButtonList() {
    std::stringstream ss;
    for (size_t i = 0; i < kEquippedActionButtonMappings.size(); ++i) {
        if (i > 0) {
            ss << '|';
        }
        ss << kEquippedActionButtonMappings[i].alias;
    }
    return ss.str();
}

std::string ToLower(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::tolower(c)); });
    return value;
}

std::string ToUpper(std::string value) {
    std::transform(value.begin(), value.end(), value.begin(),
                   [](unsigned char c) { return static_cast<char>(std::toupper(c)); });
    return value;
}

std::string NormalizeKeyboardKeyToken(std::string value) {
    value = ToUpper(value);
    value.erase(std::remove_if(value.begin(), value.end(),
                               [](unsigned char c) { return c == '_' || c == '-' || std::isspace(c) != 0; }),
                value.end());
    return value;
}

bool ParseKeyboardKeyToken(const std::string& value, int32_t& outScancode, std::string& outError) {
    if (value.empty()) {
        outError = "keyboard key token must not be empty";
        return false;
    }

    auto token = NormalizeKeyboardKeyToken(value);
    if (token.rfind("LUSKB", 0) == 0) {
        token = token.substr(5);
    } else if (token.rfind("KB", 0) == 0) {
        token = token.substr(2);
    }

    const auto it = kKeyboardKeyAliases.find(token);
    if (it == kKeyboardKeyAliases.end()) {
        outError = "unsupported keyboard key token: " + value;
        return false;
    }

    outScancode = static_cast<int32_t>(it->second);
    return true;
}

bool IsReservedDefaultKeyboardScancode(int32_t scancode) {
    return kReservedDefaultKeyboardScancodes.contains(static_cast<Ship::KbScancode>(scancode));
}

bool ParseAimCameraContextToken(const std::string& value, ExternalModAimCameraContext& outContext) {
    const auto normalized = ToLower(value);
    if (normalized == "cup" || normalized == "c_up" || normalized == "c-up" || normalized == "firstperson") {
        outContext = ExternalModAimCameraContext::CUp;
        return true;
    }
    if (normalized == "bow" || normalized == "bowarrow") {
        outContext = ExternalModAimCameraContext::Bow;
        return true;
    }
    if (normalized == "hookshot" || normalized == "longshot") {
        outContext = ExternalModAimCameraContext::Hookshot;
        return true;
    }
    if (normalized == "slingshot") {
        outContext = ExternalModAimCameraContext::Slingshot;
        return true;
    }
    if (normalized == "boomerang") {
        outContext = ExternalModAimCameraContext::Boomerang;
        return true;
    }
    return false;
}

uint8_t AimCameraContextToMask(ExternalModAimCameraContext context) {
    switch (context) {
        case ExternalModAimCameraContext::CUp:
            return 1 << 0;
        case ExternalModAimCameraContext::Bow:
            return 1 << 1;
        case ExternalModAimCameraContext::Hookshot:
            return 1 << 2;
        case ExternalModAimCameraContext::Slingshot:
            return 1 << 3;
        case ExternalModAimCameraContext::Boomerang:
            return 1 << 4;
        default:
            return 0;
    }
}

bool ParseAimCameraModeToken(const std::string& value, ExternalModAimCameraMode& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "firstperson" || normalized == "first_person") {
        outMode = ExternalModAimCameraMode::FirstPerson;
        return true;
    }
    if (normalized == "overshoulder" || normalized == "over_shoulder" || normalized == "thirdperson" ||
        normalized == "third_person") {
        outMode = ExternalModAimCameraMode::OverShoulder;
        return true;
    }
    return false;
}

bool ParseCameraModeTypeToken(const std::string& value, int16_t& outMode) {
    const auto normalized = ToLower(value);
    if (normalized == "firstperson" || normalized == "first_person") {
        outMode = CAM_MODE_FIRSTPERSON;
        return true;
    }
    if (normalized == "bowarrow" || normalized == "bow_arrow") {
        outMode = CAM_MODE_BOWARROW;
        return true;
    }
    if (normalized == "bowarrowz" || normalized == "bow_arrow_z" || normalized == "bowarrow_z") {
        outMode = CAM_MODE_BOWARROWZ;
        return true;
    }
    if (normalized == "hookshot") {
        outMode = CAM_MODE_HOOKSHOT;
        return true;
    }
    if (normalized == "slingshot") {
        outMode = CAM_MODE_SLINGSHOT;
        return true;
    }
    if (normalized == "boomerang") {
        outMode = CAM_MODE_BOWARROW;
        return true;
    }
    return false;
}

int16_t GetCameraModeForContext(const ExternalModAimCameraProfile& profile, ExternalModAimCameraContext context,
                                bool overShoulder) {
    if (overShoulder) {
        switch (context) {
            case ExternalModAimCameraContext::CUp:
                return profile.cUpOverShoulderMode;
            case ExternalModAimCameraContext::Bow:
                return profile.bowOverShoulderMode;
            case ExternalModAimCameraContext::Hookshot:
                return profile.hookshotOverShoulderMode;
            case ExternalModAimCameraContext::Slingshot:
                return profile.slingshotOverShoulderMode;
            case ExternalModAimCameraContext::Boomerang:
                return profile.boomerangOverShoulderMode;
            default:
                return CAM_MODE_BOWARROWZ;
        }
    }

    switch (context) {
        case ExternalModAimCameraContext::CUp:
            return profile.cUpFirstPersonMode;
        case ExternalModAimCameraContext::Bow:
```

## file: soh/soh/Enhancements/external-mods/ExternalModWorldGraphicsRuntime.cpp
```text
#include "ExternalModWorldGraphicsRuntime.h"

#include <algorithm>
#include <array>
#include <cctype>
#include <cmath>
#include <cstdint>
#include <limits>
#include <optional>
#include <sstream>
#include <string>
#include <tuple>
#include <vector>

#include <spdlog/spdlog.h>

#include "ExternalModManager.h"
#include "fast/backends/gfx_rendering_api.h"
#include "libultraship/bridge/consolevariablebridge.h"
#include "ship/Context.h"

extern "C" {
#include <z64.h>
#include "macros.h"
#include "functions.h"
extern PlayState* gPlayState;
void gfx_set_force_depth_aware_fog(uint8_t enabled);
void gfx_set_ambient_occlusion_enabled(uint8_t enabled);
void gfx_set_ambient_occlusion_debug_view(uint8_t enabled);
void gfx_set_ambient_occlusion_config(float radius, float intensity, float bias, float power, float maxDistance, int32_t blurPasses,
                                      uint8_t quality);
int32_t gfx_get_ambient_occlusion_fallback_reason();
void gfx_clear_ambient_occlusion_fallback_reason();
void gfx_set_volumetrics_enabled(uint8_t enabled);
void gfx_set_volumetrics_debug_view(uint8_t enabled);
void gfx_set_volumetrics_config(float density, float anisotropy, float startDistance, float maxDistance, uint8_t quality,
                                uint8_t shadowQuality, float baseHeight, float heightFalloff, uint8_t heightFogEnabled,
                                float lightShaftIntensity, float shadowIntensity, float temporalBlend, float jitterScale,
                                uint8_t maxShadowedLights, uint8_t resetHistory);
void gfx_set_volumetrics_camera(const float* currentViewProjection, const float* inverseViewProjection,
                                const float* previousViewProjection, float cameraX, float cameraY, float cameraZ,
                                float nearPlane, float farPlane);
void gfx_set_volumetric_lights(const Fast::GfxVolumetricLight* lights, size_t count);
int32_t gfx_get_volumetrics_fallback_reason();
void gfx_clear_volumetrics_fallback_reason();
}

namespace SOH {

namespace {

constexpr float kPi = 3.14159265358979323846f;

template <typename T>
constexpr T ClampValue(T value, T minValue, T maxValue) {
    if (value < minValue) {
        return minValue;
    }
    if (value > maxValue) {
        return maxValue;
    }
    return value;
}

bool IsHigherPriorityPackage(const ExternalModPackage* lhs, const ExternalModPackage* rhs) {
    if (lhs == nullptr) {
        return false;
    }
    if (rhs == nullptr) {
        return true;
    }
    if (lhs->manifest.loadPriority != rhs->manifest.loadPriority) {
        return lhs->manifest.loadPriority > rhs->manifest.loadPriority;
    }
    return lhs->manifest.id < rhs->manifest.id;
}

template <typename TDefinition, typename TCollectionSelector>
const TDefinition* FindDefinitionAcrossPackages(const std::vector<ExternalModPackage>& packages, const std::string& id,
                                                TCollectionSelector&& collectionSelector,
                                                const ExternalModPackage** outOwnerPackage = nullptr) {
    const TDefinition* bestDefinition = nullptr;
    const ExternalModPackage* bestPackage = nullptr;

    if (id.empty()) {
        if (outOwnerPackage != nullptr) {
            *outOwnerPackage = nullptr;
        }
        return nullptr;
    }

    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }

        const auto& collection = collectionSelector(package.runtime);
        const auto definitionIt = std::find_if(collection.begin(), collection.end(),
                                               [&id](const TDefinition& definition) { return definition.id == id; });
        if (definitionIt == collection.end()) {
            continue;
        }

        if (!IsHigherPriorityPackage(&package, bestPackage)) {
            continue;
        }

        bestPackage = &package;
        bestDefinition = &(*definitionIt);
    }

    if (outOwnerPackage != nullptr) {
        *outOwnerPackage = bestPackage;
    }
    return bestDefinition;
}

std::optional<int32_t> TryGetIntBlackboardValue(const ExternalModRuntime& runtime, const std::string& key) {
    const auto it = runtime.globalBlackboard.find(key);
    if (it == runtime.globalBlackboard.end()) {
        return std::nullopt;
    }
    try {
        return std::stoi(it->second);
    } catch (...) {
        return std::nullopt;
    }
}

bool MatchesSceneRoom(int16_t currentScene, int16_t currentRoom, const ExternalModRuntime& runtime, const std::string& sceneKey,
                      const std::string& roomKey) {
    const auto sceneValue = TryGetIntBlackboardValue(runtime, sceneKey);
    if (sceneValue.has_value() && static_cast<int16_t>(*sceneValue) != currentScene) {
        return false;
    }

    if (!roomKey.empty()) {
        const auto roomValue = TryGetIntBlackboardValue(runtime, roomKey);
        if (roomValue.has_value() && static_cast<int16_t>(*roomValue) != currentRoom) {
            return false;
        }
    }
    return true;
}

std::array<float, 3> KelvinToRgb(float kelvin) {
    kelvin = ClampValue(kelvin, 1000.0f, 40000.0f) / 100.0f;
    float red = 0.0f;
    float green = 0.0f;
    float blue = 0.0f;

    if (kelvin <= 66.0f) {
        red = 255.0f;
        green = 99.4708025861f * std::log(kelvin) - 161.1195681661f;
        blue = kelvin <= 19.0f ? 0.0f : 138.5177312231f * std::log(kelvin - 10.0f) - 305.0447927307f;
    } else {
        red = 329.698727446f * std::pow(kelvin - 60.0f, -0.1332047592f);
        green = 288.1221695283f * std::pow(kelvin - 60.0f, -0.0755148492f);
        blue = 255.0f;
    }

    red = ClampValue(red, 0.0f, 255.0f);
    green = ClampValue(green, 0.0f, 255.0f);
    blue = ClampValue(blue, 0.0f, 255.0f);
    return { red / 255.0f, green / 255.0f, blue / 255.0f };
}

uint8_t ToByteColor(float linearValue) {
    return static_cast<uint8_t>(ClampValue(linearValue * 255.0f, 0.0f, 255.0f));
}

int16_t ToFogNearFromDensity(float density) {
    const float normalized = ClampValue(density, 0.0f, 1.0f);
    return static_cast<int16_t>(ClampValue(1000.0f - normalized * 850.0f, 0.0f, 1000.0f));
}

int16_t ToFogFarFromDensity(float density) {
    const float normalized = ClampValue(density, 0.0f, 1.0f);
    return static_cast<int16_t>(ClampValue(1000.0f - normalized * 300.0f, 0.0f, 1000.0f));
}

struct ResolvedGraphicsState {
    const ExternalModPackage* sceneProfileSource = nullptr;
    const ExternalModSceneProfileDefinition* sceneProfile = nullptr;
    const ExternalModPackage* roomProfileSource = nullptr;
    const ExternalModRoomProfileDefinition* roomProfile = nullptr;
    const ExternalModPackage* postFxSource = nullptr;
    const ExternalModPostFxPresetDefinition* postFx = nullptr;
    float postFxBlend = 1.0f;
    const ExternalModPackage* skylightSource = nullptr;
    const ExternalModLightProfileDefinition* skylight = nullptr;
    bool fallbackSpotToPoint = false;
};

ResolvedGraphicsState ResolveGraphicsState(const std::vector<ExternalModPackage>& packages, int16_t sceneId, int16_t roomId) {
    ResolvedGraphicsState state{};

    // Action scene override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeSceneProfileId.empty()) {
            continue;
        }
        if (!MatchesSceneRoom(sceneId, roomId, package.runtime, "__world_scene_profile_scene", "")) {
            continue;
        }

        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModSceneProfileDefinition>(
            packages, package.runtime.activeSceneProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModSceneProfileDefinition>& {
                return runtime.sceneProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.sceneProfileSource)) {
            continue;
        }
        state.sceneProfileSource = &package;
        state.sceneProfile = profile;
        if (profileOwner != nullptr) {
            state.sceneProfileSource = profileOwner;
```
```text
        }
    }

    // Auto scene
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& profile : package.runtime.sceneProfiles) {
            if (profile.sceneId != sceneId) {
                continue;
            }
            if (!IsHigherPriorityPackage(&package, state.sceneProfileSource)) {
                continue;
            }
            state.sceneProfileSource = &package;
            state.sceneProfile = &profile;
        }
    }

    // Action room override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeRoomProfileId.empty()) {
            continue;
        }
        if (!MatchesSceneRoom(sceneId, roomId, package.runtime, "__world_room_profile_scene", "__world_room_profile_room")) {
            continue;
        }

        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModRoomProfileDefinition>(
            packages, package.runtime.activeRoomProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModRoomProfileDefinition>& {
                return runtime.roomProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.roomProfileSource)) {
            continue;
        }
        state.roomProfileSource = &package;
        state.roomProfile = profile;
        if (profileOwner != nullptr) {
            state.roomProfileSource = profileOwner;
        }
    }

    // Auto room
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
        }
        for (const auto& profile : package.runtime.roomProfiles) {
            if (profile.roomId != roomId) {
                continue;
            }
            if (profile.sceneId >= 0 && profile.sceneId != sceneId) {
                continue;
            }
            if (!IsHigherPriorityPackage(&package, state.roomProfileSource)) {
                continue;
            }
            state.roomProfileSource = &package;
            state.roomProfile = &profile;
        }
    }

    // Action postfx override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activePostFxPresetId.empty()) {
            continue;
        }

        const ExternalModPackage* presetOwner = nullptr;
        const auto* preset = FindDefinitionAcrossPackages<ExternalModPostFxPresetDefinition>(
            packages, package.runtime.activePostFxPresetId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModPostFxPresetDefinition>& {
                return runtime.postFxPresets;
            },
            &presetOwner);
        if (preset == nullptr || !IsHigherPriorityPackage(&package, state.postFxSource)) {
            continue;
        }
        state.postFxSource = presetOwner != nullptr ? presetOwner : &package;
        state.postFx = preset;
        state.postFxBlend = ClampValue(package.runtime.activePostFxBlend, 0.0f, 1.0f);
    }

    if (state.postFx == nullptr) {
        std::string presetId;
        if (state.roomProfile != nullptr && !state.roomProfile->postFxPresetId.empty()) {
            presetId = state.roomProfile->postFxPresetId;
        } else if (state.sceneProfile != nullptr && !state.sceneProfile->postFxPresetId.empty()) {
            presetId = state.sceneProfile->postFxPresetId;
        }
        if (!presetId.empty()) {
            const ExternalModPackage* presetOwner = nullptr;
            state.postFx = FindDefinitionAcrossPackages<ExternalModPostFxPresetDefinition>(
                packages, presetId,
                [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModPostFxPresetDefinition>& {
                    return runtime.postFxPresets;
                },
                &presetOwner);
            state.postFxSource = presetOwner;
            state.postFxBlend = 1.0f;
        }
    }

    // Action skylight override
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled || package.runtime.activeSkylightProfileId.empty()) {
            continue;
        }
        const ExternalModPackage* profileOwner = nullptr;
        const auto* profile = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
            packages, package.runtime.activeSkylightProfileId,
            [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                return runtime.lightProfiles;
            },
            &profileOwner);
        if (profile == nullptr || !IsHigherPriorityPackage(&package, state.skylightSource)) {
            continue;
        }
        state.skylightSource = profileOwner != nullptr ? profileOwner : &package;
        state.skylight = profile;
    }

    if (state.skylight == nullptr) {
        std::string profileId;
        if (state.roomProfile != nullptr && !state.roomProfile->skylightProfileId.empty()) {
            profileId = state.roomProfile->skylightProfileId;
        } else if (state.sceneProfile != nullptr && !state.sceneProfile->skylightProfileId.empty()) {
            profileId = state.sceneProfile->skylightProfileId;
        }
        if (!profileId.empty()) {
            const ExternalModPackage* profileOwner = nullptr;
            state.skylight = FindDefinitionAcrossPackages<ExternalModLightProfileDefinition>(
                packages, profileId,
                [](const ExternalModRuntime& runtime) -> const std::vector<ExternalModLightProfileDefinition>& {
                    return runtime.lightProfiles;
                },
                &profileOwner);
            state.skylightSource = profileOwner;
        }
    }

    return state;
}

uint8_t ParseAmbientOcclusionQuality(const std::string& quality) {
    std::string normalized = quality;
    std::transform(normalized.begin(), normalized.end(), normalized.begin(),
                   [](unsigned char ch) { return static_cast<char>(std::tolower(ch)); });
    if (normalized == "low") {
        return 1;
    }
    if (normalized == "medium") {
        return 2;
    }
    if (normalized == "high") {
        return 3;
    }
    return 2;
}

uint8_t ParseVolumetricsQuality(const std::string& quality) {
    return ParseAmbientOcclusionQuality(quality);
}

uint8_t GetVolumetricsShadowBudget(uint8_t quality, uint8_t shadowQuality) {
    if (shadowQuality == 0 || quality == 0) {
        return 0;
    }
    switch (quality) {
        case 1:
            return 1;
        case 2:
            return 2;
        case 3:
            return 4;
        default:
            return 0;
    }
}

void SetIdentityMatrix(float* outMatrix) {
    std::fill(outMatrix, outMatrix + 16, 0.0f);
    outMatrix[0] = 1.0f;
    outMatrix[5] = 1.0f;
    outMatrix[10] = 1.0f;
    outMatrix[15] = 1.0f;
}

void CopyMatrixToColumnMajor(const MtxF& matrix, float* outMatrix) {
    for (size_t row = 0; row < 4; ++row) {
        for (size_t col = 0; col < 4; ++col) {
            outMatrix[col * 4 + row] = matrix.mf[row][col];
        }
    }
}

std::array<float, 3> NormalizeVector(std::array<float, 3> value, std::array<float, 3> fallback = { { 0.0f, -1.0f, 0.0f } }) {
    const float lengthSq = value[0] * value[0] + value[1] * value[1] + value[2] * value[2];
    if (lengthSq <= 0.000001f) {
        return fallback;
    }
    const float invLength = 1.0f / std::sqrt(lengthSq);
    return { { value[0] * invLength, value[1] * invLength, value[2] * invLength } };
}

std::string GetAmbientOcclusionFallbackReasonName(int32_t code) {
    switch (code) {
        case 1:
            return "AO_BACKEND_UNSUPPORTED_METAL";
        case 2:
            return "AO_SHADER_COMPILE_FAILED";
        case 3:
            return "AO_RUNTIME_UNSUPPORTED";
        default:
            return "";
    }
```
```text
}

std::string GetVolumetricsFallbackReasonName(int32_t code) {
    switch (code) {
        case 1:
            return "VOLUMETRICS_BACKEND_UNSUPPORTED_METAL";
        case 2:
            return "VOLUMETRICS_SHADER_COMPILE_FAILED";
        case 3:
            return "VOLUMETRICS_RUNTIME_UNSUPPORTED";
        default:
            return "";
    }
}

} // namespace

struct ExternalModWorldGraphicsRuntime::Impl {
    struct FrameLightRecord {
        LightInfo info{};
        LightNode* node = nullptr;
        std::string modId;
        int32_t handle = 0;
    };

    bool envCaptured = false;
    std::array<uint8_t, 3> lightAmbientColor{};
    std::array<uint8_t, 3> lightFogColor{};
    int16_t lightFogNear = 0;
    int16_t lightFogFar = 0;
    std::array<uint8_t, 3> envAmbientColor{};
    std::array<uint8_t, 3> envFogColor{};
    std::array<int8_t, 3> envLight1Dir{};
    std::array<int8_t, 3> envLight2Dir{};
    std::array<uint8_t, 3> envLight1Color{};
    std::array<uint8_t, 3> envLight2Color{};
    int16_t envFogNear = 0;
    int16_t envFogFar = 0;
    bool envFillScreen = false;
    std::array<uint8_t, 4> envScreenFillColor{};
    bool forcedFogOverlayActive = false;
    std::array<uint8_t, 4> appliedFogOverlayColor{};
    bool depthAwareFogForced = false;
    bool aoEnabled = false;
    uint8_t aoQuality = 0;
    int32_t aoFallbackReason = 0;
    std::string aoSourceProfileId;
    std::string aoSourceModId;
    bool volumetricsEnabled = false;
    uint8_t volumetricsQuality = 0;
    uint8_t volumetricsShadowQuality = 0;
    int32_t volumetricsFallbackReason = 0;
    std::string volumetricsSourceProfileId;
    std::string volumetricsSourceModId;
    size_t volumetricLightCount = 0;
    size_t volumetricShadowLightCount = 0;
    std::array<float, 16> previousViewProjection{};
    bool previousViewProjectionValid = false;
    int16_t previousSceneId = -1;
    int16_t previousRoomId = -1;
    std::vector<FrameLightRecord> frameLights;
    std::string lastResolvedSignature;
    std::string inspectorSummary;
};

ExternalModWorldGraphicsRuntime::ExternalModWorldGraphicsRuntime() : mImpl(std::make_unique<Impl>()) {
    SetIdentityMatrix(mImpl->previousViewProjection.data());
}

ExternalModWorldGraphicsRuntime::~ExternalModWorldGraphicsRuntime() = default;

void ExternalModWorldGraphicsRuntime::OnPlayDrawBegin(ExternalModManager& manager,
                                                      std::vector<ExternalModPackage>& packages, PlayState* play) {
    if (play == nullptr) {
        return;
    }

    // Defensive cleanup in case End hook was skipped.
    OnPlayDrawEnd(play);

    mImpl->envCaptured = true;
    for (size_t i = 0; i < 3; ++i) {
        mImpl->lightAmbientColor[i] = play->lightCtx.ambientColor[i];
        mImpl->lightFogColor[i] = play->lightCtx.fogColor[i];
        mImpl->envAmbientColor[i] = play->envCtx.lightSettings.ambientColor[i];
        mImpl->envFogColor[i] = play->envCtx.lightSettings.fogColor[i];
        mImpl->envLight1Dir[i] = play->envCtx.lightSettings.light1Dir[i];
        mImpl->envLight2Dir[i] = play->envCtx.lightSettings.light2Dir[i];
        mImpl->envLight1Color[i] = play->envCtx.lightSettings.light1Color[i];
        mImpl->envLight2Color[i] = play->envCtx.lightSettings.light2Color[i];
    }
    mImpl->lightFogNear = play->lightCtx.fogNear;
    mImpl->lightFogFar = play->lightCtx.fogFar;
    mImpl->envFogNear = play->envCtx.lightSettings.fogNear;
    mImpl->envFogFar = play->envCtx.lightSettings.fogFar;
    mImpl->envFillScreen = play->envCtx.fillScreen;
    for (size_t i = 0; i < 4; ++i) {
        mImpl->envScreenFillColor[i] = play->envCtx.screenFillColor[i];
    }
    mImpl->forcedFogOverlayActive = false;
    mImpl->appliedFogOverlayColor.fill(0);
    mImpl->depthAwareFogForced = false;
    mImpl->aoEnabled = false;
    mImpl->aoQuality = 0;
    mImpl->aoFallbackReason = 0;
    mImpl->aoSourceProfileId.clear();
    mImpl->aoSourceModId.clear();
    mImpl->volumetricsEnabled = false;
    mImpl->volumetricsQuality = 0;
    mImpl->volumetricsShadowQuality = 0;
    mImpl->volumetricsFallbackReason = 0;
    mImpl->volumetricsSourceProfileId.clear();
    mImpl->volumetricsSourceModId.clear();
    mImpl->volumetricLightCount = 0;
    mImpl->volumetricShadowLightCount = 0;

    const int16_t sceneId = play->sceneNum;
    const int16_t roomId = play->roomCtx.curRoom.num;
    const ResolvedGraphicsState resolvedState = ResolveGraphicsState(packages, sceneId, roomId);

    std::ostringstream signatureBuilder;
    signatureBuilder << sceneId << ":" << roomId << "|"
                     << (resolvedState.sceneProfile != nullptr ? resolvedState.sceneProfile->id : "none") << "|"
                     << (resolvedState.roomProfile != nullptr ? resolvedState.roomProfile->id : "none") << "|"
                     << (resolvedState.postFx != nullptr ? resolvedState.postFx->id : "none") << "|"
                     << (resolvedState.skylight != nullptr ? resolvedState.skylight->id : "none");
    const std::string resolvedSignature = signatureBuilder.str();
    const bool renderProfileChanged = resolvedSignature != mImpl->lastResolvedSignature;
    if (renderProfileChanged) {
        ExternalModHookEventContext context;
        context.scene = sceneId;
        context.value = resolvedSignature;
        manager.EmitExtendedHook(ExternalModHookType::OnRenderProfileResolved, context, "OnRenderProfileResolved");
        mImpl->lastResolvedSignature = resolvedSignature;
    }

    if (resolvedState.postFx != nullptr) {
        if (resolvedState.postFx->forceDepthAwareFog) {
            gfx_set_force_depth_aware_fog(1);
            mImpl->depthAwareFogForced = true;
        }

        std::array<uint8_t, 3> targetFogColor = {
            ToByteColor(resolvedState.postFx->fogColor[0]), ToByteColor(resolvedState.postFx->fogColor[1]),
            ToByteColor(resolvedState.postFx->fogColor[2]),
        };
        const float blend = ClampValue(resolvedState.postFxBlend, 0.0f, 1.0f);
        for (size_t i = 0; i < 3; ++i) {
            const float sourceFog = static_cast<float>(mImpl->lightFogColor[i]);
            const float blendedFog = sourceFog + (static_cast<float>(targetFogColor[i]) - sourceFog) * blend;
            play->lightCtx.fogColor[i] = static_cast<uint8_t>(ClampValue(blendedFog, 0.0f, 255.0f));
            play->envCtx.lightSettings.fogColor[i] = play->lightCtx.fogColor[i];
        }

        const int16_t targetFogNear = resolvedState.postFx->hasFogNear
                                          ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogNear, 0, 1000))
                                          : ToFogNearFromDensity(resolvedState.postFx->fogDensity);
        const int16_t targetFogFar = resolvedState.postFx->hasFogFar
                                         ? static_cast<int16_t>(ClampValue(resolvedState.postFx->fogFar, 0, 1000))
                                         : ToFogFarFromDensity(resolvedState.postFx->fogDensity);
        const float blendedNear =
            static_cast<float>(mImpl->lightFogNear) + (static_cast<float>(targetFogNear - mImpl->lightFogNear) * blend);
        const float blendedFar =
            static_cast<float>(mImpl->lightFogFar) + (static_cast<float>(targetFogFar - mImpl->lightFogFar) * blend);
        play->lightCtx.fogNear = static_cast<int16_t>(ClampValue(blendedNear, 0.0f, 1000.0f));
        play->lightCtx.fogFar = static_cast<int16_t>(ClampValue(blendedFar, 0.0f, 1000.0f));
        play->envCtx.lightSettings.fogNear = play->lightCtx.fogNear;
        play->envCtx.lightSettings.fogFar = play->lightCtx.fogFar;

        const float overlayStrength = ClampValue(resolvedState.postFx->fogOverlayStrength * blend, 0.0f, 1.0f);
        if (resolvedState.postFx->forceFogOverlay && overlayStrength > 0.0f && !play->envCtx.fillScreen) {
            play->envCtx.fillScreen = true;
            play->envCtx.screenFillColor[0] = targetFogColor[0];
            play->envCtx.screenFillColor[1] = targetFogColor[1];
            play->envCtx.screenFillColor[2] = targetFogColor[2];
            play->envCtx.screenFillColor[3] = static_cast<uint8_t>(ClampValue(overlayStrength * 255.0f, 0.0f, 255.0f));
            mImpl->forcedFogOverlayActive = true;
            mImpl->appliedFogOverlayColor = { play->envCtx.screenFillColor[0], play->envCtx.screenFillColor[1],
                                              play->envCtx.screenFillColor[2], play->envCtx.screenFillColor[3] };
        }
    }

    if (resolvedState.sceneProfile != nullptr) {
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t ambient = ToByteColor(resolvedState.sceneProfile->ambientColor[i]);
            play->lightCtx.ambientColor[i] = ambient;
            play->envCtx.lightSettings.ambientColor[i] = ambient;
        }
    }

    if (resolvedState.skylight != nullptr) {
        std::array<float, 3> colorLinear = resolvedState.skylight->colorLinear;
        if (resolvedState.skylight->hasKelvin) {
            colorLinear = KelvinToRgb(resolvedState.skylight->kelvin);
        }
        const float intensity = ClampValue(resolvedState.skylight->intensity, 0.0f, 8.0f);
        for (size_t i = 0; i < 3; ++i) {
            const uint8_t colorByte = ToByteColor(colorLinear[i] * intensity);
            play->envCtx.lightSettings.light1Color[i] = colorByte;
            play->envCtx.lightSettings.light2Color[i] = colorByte;
            play->envCtx.lightSettings.ambientColor[i] =
                std::max(play->envCtx.lightSettings.ambientColor[i], static_cast<uint8_t>(colorByte / 2));
            play->lightCtx.ambientColor[i] = play->envCtx.lightSettings.ambientColor[i];
        }
        const auto& direction = resolvedState.skylight->direction;
        play->envCtx.lightSettings.light1Dir[0] = static_cast<int8_t>(ClampValue(direction[0] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[1] = static_cast<int8_t>(ClampValue(direction[1] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light1Dir[2] = static_cast<int8_t>(ClampValue(direction[2] * 127.0f, -127.0f, 127.0f));
        play->envCtx.lightSettings.light2Dir[0] = -play->envCtx.lightSettings.light1Dir[0];
        play->envCtx.lightSettings.light2Dir[1] = -play->envCtx.lightSettings.light1Dir[1];
        play->envCtx.lightSettings.light2Dir[2] = -play->envCtx.lightSettings.light1Dir[2];
    }

    int32_t maxDynamicLightsTotal = 128;
    int32_t maxDynamicLightsNear = 32;
    const ExternalModPbrDefinition* selectedAoProfile = nullptr;
    const ExternalModPackage* selectedAoPackage = nullptr;
    for (const auto& package : packages) {
        if (!package.valid || !package.runtime.enabled) {
            continue;
```

## file: libultraship/include/fast/interpreter.h
```text
#pragma once

#include <stdbool.h>
#include <stdint.h>
#include <unordered_map>
#include <map>
#include <list>
#include <cstddef>
#include <vector>
#include <stack>
#include <string>

#include "fast/lus_gbi.h"
#include "fast/types.h"
#include "fast/ucodehandlers.h"
#include "backends/gfx_rendering_api.h"

#include "fast/resource/type/Texture.h"
#include "ship/resource/Resource.h"

// TODO figure out why changing these to 640x480 makes the game only render in a quarter of the window
#define SCREEN_WIDTH 320
#define SCREEN_HEIGHT 240

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
#include <compare>
#endif

/*enum {
    CC_0,
    CC_TEXEL0,
    CC_TEXEL1,
    CC_PRIM,
    CC_SHADE,
    CC_ENV,
    CC_TEXEL0A,
    CC_LOD
};*/

enum {
    SHADER_0,
    SHADER_INPUT_1,
    SHADER_INPUT_2,
    SHADER_INPUT_3,
    SHADER_INPUT_4,
    SHADER_INPUT_5,
    SHADER_INPUT_6,
    SHADER_INPUT_7,
    SHADER_TEXEL0,
    SHADER_TEXEL0A,
    SHADER_TEXEL1,
    SHADER_TEXEL1A,
    SHADER_1,
    SHADER_COMBINED,
    SHADER_NOISE
};

#ifdef __cplusplus
enum class ShaderOpts {
    ALPHA,
    FOG,
    TEXTURE_EDGE,
    NOISE,
    _2CYC,
    ALPHA_THRESHOLD,
    INVISIBLE,
    GRAYSCALE,
    TEXEL0_CLAMP_S,
    TEXEL0_CLAMP_T,
    TEXEL1_CLAMP_S,
    TEXEL1_CLAMP_T,
    TEXEL0_MASK,
    TEXEL1_MASK,
    TEXEL0_BLEND,
    TEXEL1_BLEND,
    USE_SHADER,
    MAX
};

#define SHADER_OPT(opt) ((uint64_t)(1 << static_cast<int>(ShaderOpts::opt)))
#endif

struct ColorCombinerKey {
    uint64_t combine_mode;
    uint64_t options;

#ifdef __cplusplus
    auto operator<=>(const ColorCombinerKey&) const = default;
#endif
};

#define SHADER_MAX_TEXTURES 6
#define SHADER_FIRST_TEXTURE 0
#define SHADER_FIRST_MASK_TEXTURE 2
#define SHADER_FIRST_REPLACEMENT_TEXTURE 4

struct CCFeatures {
    int c[2][2][4];
    bool opt_alpha;
    bool opt_fog;
    bool opt_texture_edge;
    bool opt_noise;
    bool opt_2cyc;
    bool opt_alpha_threshold;
    bool opt_invisible;
    bool opt_grayscale;
    bool usedTextures[2];
    bool used_masks[2];
    bool used_blend[2];
    bool clamp[2][2];
    int numInputs;
    bool do_single[2][2];
    bool do_multiply[2][2];
    bool do_mix[2][2];
    bool color_alpha_same[2];
    int16_t shader_id;
};

void gfx_cc_get_features(uint64_t shader_id0, uint32_t shader_id1, struct CCFeatures* cc_features);

union Gfx;

namespace Fast {

class GfxRenderingAPI;
class GfxWindowBackend;

constexpr size_t MAX_SEGMENT_POINTERS = 16;

struct GfxExecStack {
    // This is a dlist stack used to handle dlist calls.
    std::stack<F3DGfx*> cmd_stack = {};
    // This is also a dlist stack but a std::vector is used to make it possible
    // to iterate on the elements.
    // The purpose of this is to identify an instruction at a poin in time
    // which would not be possible with just a F3DGfx* because a dlist can be called multiple times
    // what we do instead is store the call path that leads to the instruction (including branches)
    std::vector<const F3DGfx*> gfx_path = {};
    struct CodeDisp {
        const char* file;
        int line;
    };
    // stack for OpenDisp/CloseDisps
    std::vector<CodeDisp> disp_stack{};

    void start(F3DGfx* dlist);
    void stop();
    F3DGfx*& currCmd();
    void openDisp(const char* file, int line);
    void closeDisp();
    const std::vector<CodeDisp>& getDisp() const;
    void branch(F3DGfx* caller);
    void call(F3DGfx* caller, F3DGfx* callee);
    F3DGfx* ret();
};

struct XYWidthHeight {
    int16_t x, y;
    uint32_t width, height;
};

struct GfxDimensions {
    float internal_mul;
    uint32_t width, height;
    float aspect_ratio;
};

struct TextureCacheKey {
    const uint8_t* texture_addr;
    const uint8_t* palette_addrs[2];
    uint8_t fmt, siz;
    uint8_t palette_index;
    uint32_t size_bytes;

    bool operator==(const TextureCacheKey&) const noexcept = default;

    struct Hasher {
        size_t operator()(const TextureCacheKey& key) const noexcept {
            uintptr_t addr = (uintptr_t)key.texture_addr;
            return (size_t)(addr ^ (addr >> 5));
        }
    };
};

typedef std::unordered_map<TextureCacheKey, struct TextureCacheValue, TextureCacheKey::Hasher> TextureCacheMap;
typedef std::pair<const TextureCacheKey, struct TextureCacheValue> TextureCacheNode;

struct TextureCacheValue {
    uint32_t texture_id;
    uint8_t cms, cmt;
    bool linear_filter;

    std::list<struct TextureCacheMapIter>::iterator lru_location;
};

struct TextureCacheMapIter {
    TextureCacheMap::iterator it;
};

struct RGBA {
    uint8_t r, g, b, a;
};

struct LoadedVertex {
    float x, y, z, w;
    float u, v;
    struct RGBA color;
    uint8_t clip_rej;
};

struct RawTexMetadata {
    uint16_t width, height;
    float h_byte_scale = 1, v_pixel_scale = 1;
    std::shared_ptr<Fast::Texture> resource;
    Fast::TextureType type;
};

```
```text
struct ShaderMod {
    bool enabled = false;
    int16_t id;
    uint8_t type;
};

#define MAX_LIGHTS 32
#define MAX_VERTICES 64

struct RSP {
    float modelview_matrix_stack[11][4][4];
    uint8_t modelview_matrix_stack_size;

    float MP_matrix[4][4];
    float P_matrix[4][4];

    F3DLight_t lookat[2];
    F3DLight current_lights[MAX_LIGHTS + 1];
    float current_lights_coeffs[MAX_LIGHTS][3];
    float current_lookat_coeffs[2][3]; // lookat_x, lookat_y
    uint8_t current_num_lights;        // includes ambient light
    bool lights_changed;

    uint32_t geometry_mode;
    int16_t fog_mul, fog_offset;

    uint32_t extra_geometry_mode;

    struct {
        // U0.16
        uint16_t s, t;
    } texture_scaling_factor;

    struct LoadedVertex loaded_vertices[MAX_VERTICES + 4];
    ShaderMod current_shader;
};

struct RDP {
    const uint8_t* palettes[2];
    struct {
        const uint8_t* addr;
        uint8_t siz;
        uint32_t width;
        uint32_t tex_flags;
        struct RawTexMetadata raw_tex_metadata;
    } texture_to_load;
    struct {
        const uint8_t* addr;
        uint32_t orig_size_bytes;
        uint32_t size_bytes;
        uint32_t full_image_line_size_bytes;
        uint32_t line_size_bytes;
        uint32_t tex_flags;
        struct RawTexMetadata raw_tex_metadata;
        bool masked;
        bool blended;
    } loaded_texture[2];
    struct {
        uint8_t fmt;
        uint8_t siz;
        uint8_t cms, cmt;
        uint8_t shifts, shiftt;
        float uls, ult, lrs, lrt;
        uint16_t tmem; // 0-511, in 64-bit word units
        uint32_t line_size_bytes;
        uint8_t palette;
        uint8_t tmem_index; // 0 or 1 for offset 0 kB or offset 2 kB, respectively
    } texture_tile[8];
    bool textures_changed[2];

    uint8_t first_tile_index;

    uint32_t other_mode_l, other_mode_h;
    uint64_t combine_mode;
    bool grayscale;
    ShaderMod current_shader;

    uint8_t prim_lod_fraction;
    struct RGBA env_color, prim_color, fog_color, fill_color, grayscale_color;
    struct XYWidthHeight viewport, scissor;
    bool viewport_or_scissor_changed;
    void* z_buf_address;
    void* color_image_address;
};

typedef enum Attribute {
    MTX_PROJECTION,
    MTX_LOAD,
    MTX_PUSH,
    MTX_NOPUSH,
    CULL_FRONT,
    CULL_BACK,
    CULL_BOTH,
    MV_VIEWPORT,
    MV_LIGHT,
} Attribute;

extern GfxExecStack g_exec_stack;

struct GfxTextureCache {
    TextureCacheMap map;
    std::list<TextureCacheMapIter> lru;
    std::vector<uint32_t> free_texture_ids;
};

struct ColorCombiner {
    uint64_t shader_id0;
    uint32_t shader_id1;
    bool usedTextures[2];
    struct ShaderProgram* prg[16];
    uint8_t shader_input_mapping[2][7];
};

struct RenderingState {
    uint8_t depth_test_and_mask; // 1: depth test, 2: depth mask
    bool decal_mode;
    bool alpha_blend;
    struct XYWidthHeight viewport, scissor;
    struct ShaderProgram* mShaderProgram;
    TextureCacheNode* mTextures[SHADER_MAX_TEXTURES];
};

struct FBInfo {
    uint32_t orig_width, orig_height;       // Original shape
    uint32_t applied_width, applied_height; // Up-scaled for the viewport
    uint32_t native_width, native_height;   // Max "native" size of the screen, used for up-scaling
    bool resize;                            // Scale to match the viewport
};

struct MaskedTextureEntry {
    uint8_t* mask;
    uint8_t* replacementData;
};

class Interpreter {
  public:
    Interpreter();
    ~Interpreter();

    void Init(GfxWindowBackend* wapi, class GfxRenderingAPI* rapi, const char* game_name, bool start_in_fullscreen,
              uint32_t width, uint32_t height, uint32_t posX, uint32_t posY);
    void Destroy();
    void GetDimensions(uint32_t* width, uint32_t* height, int32_t* posX, int32_t* posY);
    GfxRenderingAPI* GetCurrentRenderingAPI();
    void StartFrame();
    void RunGuiOnly();
    void Run(Gfx* commands, const std::unordered_map<Mtx*, MtxF>& mtx_replacements);
    void EndFrame();
    void HandleWindowEvents();
    bool IsFrameReady();
    bool ViewportMatchesRendererResolution();
    int GetTargetFps();
    void SetTargetFps(int fps);
    void SetMaxFrameLatency(int latency);
    int CreateFrameBuffer(uint32_t width, uint32_t height, uint32_t native_width, uint32_t native_height,
                          uint8_t resize);
    void SetFrameBuffer(int fb, float noiseScale);
    void CopyFrameBuffer(int fb_dst_id, int fb_src_id, bool copyOnce, bool* hasCopiedPtr);
    void ResetFrameBuffer();
    void AdjustPixelDepthCoordinates(float& x, float& y);
    void GetPixelDepthPrepare(float x, float y);
    uint16_t GetPixelDepth(float x, float y);
    void RegisterBlendedTexture(const char* name, uint8_t* mask, uint8_t* replacement);
    void UnregisterBlendedTexture(const char* name);

    void SetNativeDimensions(float width, float height);
    void SetResolutionMultiplier(float multiplier);
    void SetMsaaLevel(uint32_t level);
    void GetCurDimensions(uint32_t* width, uint32_t* height);

    // private: TODO make these private
    void Flush();
    ShaderProgram* LookupOrCreateShaderProgram(uint64_t id0, uint64_t id1);
    ColorCombiner* LookupOrCreateColorCombiner(const ColorCombinerKey& key);
    void TextureCacheClear();
    bool TextureCacheLookup(int i, const TextureCacheKey& key);
    void TextureCacheDelete(const uint8_t* origAddr);
    void ImportTextureRgba16(int tile, bool importReplacement);
    void ImportTextureRgba32(int tile, bool importReplacement);
    void ImportTextureIA4(int tile, bool importReplacement);
    void ImportTextureIA8(int tile, bool importReplacement);
    void ImportTextureIA16(int tile, bool importReplacement);
    void ImportTextureI4(int tile, bool importReplacement);
    void ImportTextureI8(int tile, bool importReplacement);
    void ImportTextureCi4(int tile, bool importReplacement);
    void ImportTextureCi8(int tile, bool importReplacement);
    void ImportTextureRaw(int tile, bool importReplacement);
    void ImportTextureImg(int tile, bool importReplacement);
    void ImportTexture(int i, int tile, bool importReplacement);
    void ImportTextureMask(int i, int tile);
    void CalculateNormalDir(const F3DLight_t*, float coeffs[3]);

    void GfxSpMatrix(uint8_t params, const int32_t* addr);
    void GfxSpPopMatrix(uint32_t count);
    void GfxSpVertex(size_t numVertices, size_t destIndex, const F3DVtx* vertices);
    void GfxSpModifyVertex(uint16_t vtxIdx, uint8_t where, uint32_t val);
    void GfxSpTri1(uint8_t vtx1Idx, uint8_t vtx2Idx, uint8_t vtx3Idx, bool isRect);
    void GfxSpGeometryMode(uint32_t clear, uint32_t set);
    void GfxSpExtraGeometryMode(uint32_t clear, uint32_t set);
    void GfxSpMovememF3dex2(uint8_t index, uint8_t offset, const void* data);
    void GfxSpMovememF3d(uint8_t index, uint8_t offset, const void* data);
    void GfxSpMovewordF3dex2(uint8_t index, uint16_t offset, uintptr_t data);
    void GfxSpMovewordF3d(uint8_t index, uint16_t offset, uintptr_t data);
    void GfxSpTexture(uint16_t sc, uint16_t tc, uint8_t level, uint8_t tile, uint8_t on);
    void GfxDpSetScissor(uint32_t mode, uint32_t ulx, uint32_t uly, uint32_t lrx, uint32_t lry);
    void GfxDpSetTextureImage(uint32_t format, uint32_t size, uint32_t width, const char* texPath, uint32_t texFlags,
                              RawTexMetadata rawTexMetdata, const void* addr);
    void GfxDpSetTile(uint8_t fmt, uint32_t siz, uint32_t line, uint32_t tmem, uint8_t tile, uint32_t palette,
                      uint32_t cmt, uint32_t maskt, uint32_t shiftt, uint32_t cms, uint32_t masks, uint32_t shifts);
    void GfxDpSetTileSize(uint8_t tile, uint16_t uls, uint16_t ult, uint16_t lrs, uint16_t lrt);
    void GfxDpLoadTlut(uint8_t tile, uint32_t high_index);
    void GfxDpLoadBlock(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t dxt);
    void GfxDpLoadTile(uint8_t tile, uint32_t uls, uint32_t ult, uint32_t lrs, uint32_t lrt);
    void GfxDpSetCombineMode(uint32_t rgb, uint32_t alpha, uint32_t rgb_cyc2, uint32_t alpha_cyc2);
    void GfxDpSetGrayscaleColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void GfxDpSetEnvColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void GfxDpSetPrimColor(uint8_t m, uint8_t r, uint8_t l, uint8_t g, uint8_t b, uint8_t a);
    void GfxDpSetFogColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void GfxDpSetBlendColor(uint8_t r, uint8_t g, uint8_t b, uint8_t a);
    void GfxDpSetFillColor(uint32_t pickedColor);
```
```text
    void GfxDrawRectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry);
    void GfxDpTextureRectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry, uint8_t tile, int16_t uls,
                               int16_t ult, int16_t dsdx, int16_t dtdy, bool flip);
    void GfxDpImageRectangle(int32_t tile, int32_t w, int32_t h, int32_t ulx, int32_t uly, int16_t uls, int16_t ult,
                             int32_t lrx, int32_t lry, int16_t lrs, int16_t lrt);
    void GfxDpFillRectangle(int32_t ulx, int32_t uly, int32_t lrx, int32_t lry);
    void GfxDpSetZImage(void* zBufAddr);
    void GfxDpSetColorImage(uint32_t format, uint32_t size, uint32_t width, void* address);
    void GfxSpSetOtherMode(uint32_t shift, uint32_t num_bits, uint64_t mode);
    void GfxDpSetOtherMode(uint32_t h, uint32_t l);

    void Gfxs2dexBgCopy(F3DuObjBg* bg);
    void Gfxs2dexBg1cyc(F3DuObjBg* bg);
    void Gfxs2dexRecyCopy(F3DuObjSprite* spr);

    void AdjustWidthHeightForScale(uint32_t& width, uint32_t& height, uint32_t nativeWidth,
                                   uint32_t nativeHeight) const;
    float AdjXForAspectRatio(float x) const;
    void AdjustVIewportOrScissor(XYWidthHeight* area);
    void CalcAndSetViewport(const F3DVp_t* viewport);
    int16_t CreateShader(const std::string& path);

    void SpReset();
    void* SegAddr(uintptr_t w1);

    static const char* CCMUXtoStr(uint32_t ccmux);
    static const char* ACMUXtoStr(uint32_t acmux);
    static void GenerateCC(ColorCombiner* comb, const ColorCombinerKey& key);
    static std::string GetBaseTexturePath(const std::string& path);
    static void NormalizeVector(float v[3]);
    static void TransposedMatrixMul(float res[3], const float a[3], const float b[4][4]);
    static void MatrixMul(float res[4][4], const float a[4][4], const float b[4][4]);

    RSP* mRsp;
    RDP* mRdp;
    RenderingState mRenderingState{};

    GfxTextureCache mTextureCache{};
    std::map<ColorCombinerKey, ColorCombiner> mColorCombinerPool; // color_combiner_pool;
    std::map<ColorCombinerKey, ColorCombiner>::iterator mPrevCombiner = mColorCombinerPool.end();
    uint8_t* mTexUploadBuffer = nullptr;

    GfxDimensions mGfxCurrentWindowDimensions{}; // gfx_current_window_dimensions;
    int32_t mCurWindowPosX{};
    int32_t mCurWindowPosY{};
    GfxDimensions mCurDimensions{};        // gfx_current_dimensions;
    GfxDimensions mPrvDimensions{};        // gfx_prev_dimensions;
    XYWidthHeight mGameWindowViewport{};   // gfx_current_game_window_viewport;
    XYWidthHeight mNativeDimensions{};     // gfx_native_dimensions;
    XYWidthHeight mPrevNativeDimensions{}; // gfx_prev_native_dimensions;
    uintptr_t mGfxFrameBuffer{};

    unsigned int mMsaaLevel = 1;
    bool mDroppedFrame{};
    float* mBufVbo; // 3 vertices in a triangle and 32 floats per vtx
    size_t mBufVboLen{};
    size_t mBufVboNumTris{};
    GfxWindowBackend* mWapi = nullptr;
    GfxRenderingAPI* mRapi = nullptr;

    uintptr_t mSegmentPointers[MAX_SEGMENT_POINTERS]{};

    bool mFbActive{};
    bool mRendersToFb{}; // game_renders_to_framebuffer;
    std::map<int, FBInfo>::iterator mActiveFrameBuffer;
    std::map<int, FBInfo> mFrameBuffers;

    int mGameFb{};             // game_framebuffer;
    int mGameFbMsaaResolved{}; // game_framebuffer_msaa_resolved;

    std::set<std::pair<float, float>> mGetPixelDepthPending; // get_pixel_depth_pending;
    std::unordered_map<std::pair<float, float>, uint16_t, hash_pair_ff> mGetPixelDepthCached; // get_pixel_depth_cached;
    std::map<std::string, MaskedTextureEntry> mMaskedTextures;

    const std::unordered_map<Mtx*, MtxF>* mCurMtxReplacements;
    bool mMarkerOn; // This was originally a debug feature. Now it seems to control s2dex?
    std::vector<std::string> shader_ids;
    int mInterpolationIndex;
    int mInterpolationIndexTarget;
};

void gfx_set_target_ucode(UcodeHandlers ucode);
void gfx_push_current_dir(char* path);
int32_t gfx_check_image_signature(const char* imgData);
const char* GfxGetOpcodeName(int8_t opcode);

} // namespace Fast

extern "C" void gfx_texture_cache_clear();
extern "C" int gfx_create_framebuffer(uint32_t width, uint32_t height, uint32_t native_width, uint32_t native_height,
                                      uint8_t resize);
extern "C" void gfx_set_force_depth_aware_fog(uint8_t enabled);
extern "C" void gfx_set_ambient_occlusion_enabled(uint8_t enabled);
extern "C" void gfx_set_ambient_occlusion_debug_view(uint8_t enabled);
extern "C" void gfx_set_ambient_occlusion_config(float radius, float intensity, float bias, float power, float maxDistance,
                                                 int32_t blurPasses, uint8_t quality);
extern "C" int32_t gfx_get_ambient_occlusion_fallback_reason();
extern "C" void gfx_clear_ambient_occlusion_fallback_reason();
extern "C" void gfx_set_volumetrics_enabled(uint8_t enabled);
extern "C" void gfx_set_volumetrics_debug_view(uint8_t enabled);
extern "C" void gfx_set_volumetrics_config(float density, float anisotropy, float startDistance, float maxDistance,
                                           uint8_t quality, uint8_t shadowQuality, float baseHeight, float heightFalloff,
                                           uint8_t heightFogEnabled, float lightShaftIntensity, float shadowIntensity,
                                           float temporalBlend, float jitterScale, uint8_t maxShadowedLights,
                                           uint8_t resetHistory);
extern "C" void gfx_set_volumetrics_camera(const float* currentViewProjection, const float* inverseViewProjection,
                                           const float* previousViewProjection, float cameraX, float cameraY,
                                           float cameraZ, float nearPlane, float farPlane);
extern "C" void gfx_set_volumetric_lights(const Fast::GfxVolumetricLight* lights, size_t count);
extern "C" int32_t gfx_get_volumetrics_fallback_reason();
extern "C" void gfx_clear_volumetrics_fallback_reason();
```

## file: libultraship/src/fast/interpreter.cpp
```text
#define NOMINMAX

#include <math.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <assert.h>
#include <stdio.h>

#include <any>
#include <map>
#include <set>
#include <unordered_map>
#include <vector>
#include <list>
#include <stack>
#include "fast/resource/type/Light.h"

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif
#include "fast/debug/GfxDebugger.h"
#include "fast/types.h"
#include <string>

#include "fast/interpreter.h"
#include "fast/lus_gbi.h"
#include "fast/backends/gfx_window_manager_api.h"
#include "fast/backends/gfx_rendering_api.h"

#include "ship/window/gui/Gui.h"
#include "ship/resource/ResourceManager.h"
#include "ship/utils/Utils.h"
#include "ship/Context.h"
#include "ship/config/ConsoleVariable.h"

#include "libultraship/libultra/os.h"

#include <spdlog/fmt/fmt.h>

std::stack<std::string> currentDir;

#define SEG_ADDR(seg, addr) (addr | (seg << 24) | 1)
#define SUPPORT_CHECK(x) assert(x)

// SCALE_M_N: upscale/downscale M-bit integer to N-bit
#define SCALE_5_8(VAL_) (((VAL_)*0xFF) / 0x1F)
#define SCALE_8_5(VAL_) ((((VAL_) + 4) * 0x1F) / 0xFF)
#define SCALE_4_8(VAL_) ((VAL_)*0x11)
#define SCALE_8_4(VAL_) ((VAL_) / 0x11)
#define SCALE_3_8(VAL_) ((VAL_)*0x24)
#define SCALE_8_3(VAL_) ((VAL_) / 0x24)

// Based off the current set native dimensions or active framebuffer
#define HALF_SCREEN_WIDTH(activeFb) ((mFbActive ? activeFb->second.orig_width : mNativeDimensions.width) / 2)
#define HALF_SCREEN_HEIGHT(activeFb) ((mFbActive ? activeFb->second.orig_height : mNativeDimensions.height) / 2)

// Ratios for current window dimensions or active framebuffer scaled size
#define RATIO_X(activeFb, dims) \
    ((mFbActive ? activeFb->second.applied_width : dims.width) / (2.0f * HALF_SCREEN_WIDTH(activeFb)))
#define RATIO_Y(activeFb, dims) \
    ((mFbActive ? activeFb->second.applied_height : dims.height) / (2.0f * HALF_SCREEN_HEIGHT(activeFb)))

#define TEXTURE_CACHE_MAX_SIZE 500

namespace Fast {

static UcodeHandlers ucode_handler_index = ucode_f3dex2;

const static uint32_t f3dex2AttrHandler[] = {
    F3DEX2_G_MTX_PROJECTION, F3DEX2_G_MTX_LOAD,  F3DEX2_G_MTX_PUSH,  F3DEX_G_MTX_NOPUSH,
    F3DEX2_G_CULL_FRONT,     F3DEX2_G_CULL_BACK, F3DEX2_G_CULL_BOTH,
};

const static uint32_t f3dexAttrHandler[] = { F3DEX_G_MTX_PROJECTION, F3DEX_G_MTX_LOAD,   F3DEX_G_MTX_PUSH,
                                             F3DEX_G_MTX_NOPUSH,     F3DEX_G_CULL_FRONT, F3DEX_G_CULL_BACK,
                                             F3DEX_G_CULL_BOTH };

static constexpr std::array ucode_attr_handlers = {
    &f3dexAttrHandler,  // ucode_f3db
    &f3dexAttrHandler,  // ucode_f3d
    &f3dexAttrHandler,  // ucode_f3dex
    &f3dexAttrHandler,  // ucode_f3exb
    &f3dex2AttrHandler, // ucode_f3ex2
    &f3dex2AttrHandler, // ucode_s2dex
};

static uint32_t get_attr(Attribute attr) {
    const auto ucode_map = ucode_attr_handlers[ucode_handler_index];
    // assert(ucode_map->contains(attr) && "Attribute not found in the current ucode handler");
    return (*ucode_map)[attr];
}

static std::string GetPathWithoutFileName(char* filePath) {
    size_t len = strlen(filePath);

    for (size_t i = len - 1; (long)i >= 0; i--) {
        if (filePath[i] == '/' || filePath[i] == '\\') {
            return std::string(filePath).substr(0, i);
        }
    }

    return filePath;
}

constexpr size_t MAX_TRI_BUFFER = 256;

Interpreter::Interpreter() {
    mRsp = new RSP();
    mRdp = new RDP();
    mBufVbo = new float[MAX_TRI_BUFFER * (32 * 3)];
}

Interpreter::~Interpreter() {
    delete mRsp;
    delete mRdp;
    delete[] mBufVbo;
}

static std::weak_ptr<Interpreter> mInstance;
static bool gForceDepthAwareFog = false;
static GfxAmbientOcclusionConfig gAmbientOcclusionConfig{};
static int32_t gAmbientOcclusionFallbackReason = 0;
static GfxVolumetricsConfig gVolumetricsConfig{};
static std::array<GfxVolumetricLight, GFX_MAX_VOLUMETRIC_LIGHTS> gVolumetricLights{};
static size_t gVolumetricLightCount = 0;
static int32_t gVolumetricsFallbackReason = 0;
enum {
    GFX_AO_FALLBACK_NONE = 0,
    GFX_AO_FALLBACK_BACKEND_UNSUPPORTED_METAL = 1,
    GFX_AO_FALLBACK_SHADER_COMPILE_FAILED = 2,
    GFX_AO_FALLBACK_RUNTIME_UNSUPPORTED = 3,
};
enum {
    GFX_VOLUMETRICS_FALLBACK_NONE = 0,
    GFX_VOLUMETRICS_FALLBACK_BACKEND_UNSUPPORTED_METAL = 1,
    GFX_VOLUMETRICS_FALLBACK_SHADER_COMPILE_FAILED = 2,
    GFX_VOLUMETRICS_FALLBACK_RUNTIME_UNSUPPORTED = 3,
};

static void SetIdentityMatrix(float* matrix) {
    memset(matrix, 0, sizeof(float) * 16);
    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;
}
// Set a cached pointer to the instance so we don't need to go through the window every time
void GfxSetInstance(std::shared_ptr<Interpreter> gfx) {
    mInstance = gfx;
}

void Interpreter::Flush() {
    if (mBufVboLen > 0) {
        mRapi->DrawTriangles(mBufVbo, mBufVboLen, mBufVboNumTris);
        mBufVboLen = 0;
        mBufVboNumTris = 0;
    }
}

ShaderProgram* Interpreter::LookupOrCreateShaderProgram(uint64_t id0, uint64_t id1) {
    ShaderProgram* prg = mRapi->LookupShader(id0, id1);
    if (prg == nullptr) {
        mRapi->UnloadShader(mRenderingState.mShaderProgram);
        prg = mRapi->CreateAndLoadNewShader(id0, id1);
        mRenderingState.mShaderProgram = prg;
    }
    return prg;
}

const char* Interpreter::CCMUXtoStr(uint32_t ccmux) {
    static constexpr std::array tbl = {
        "G_CCMUX_COMBINED",
        "G_CCMUX_TEXEL0",
        "G_CCMUX_TEXEL1",
        "G_CCMUX_PRIMITIVE",
        "G_CCMUX_SHADE",
        "G_CCMUX_ENVIRONMENT",
        "G_CCMUX_1",
        "G_CCMUX_COMBINED_ALPHA",
        "G_CCMUX_TEXEL0_ALPHA",
        "G_CCMUX_TEXEL1_ALPHA",
        "G_CCMUX_PRIMITIVE_ALPHA",
        "G_CCMUX_SHADE_ALPHA",
        "G_CCMUX_ENV_ALPHA",
        "G_CCMUX_LOD_FRACTION",
        "G_CCMUX_PRIM_LOD_FRAC",
        "G_CCMUX_K5",
    };
    if (ccmux > tbl.size()) {
        return "G_CCMUX_0";
    }
    return tbl[ccmux];
}

// Seems unused
const char* Interpreter::ACMUXtoStr(uint32_t acmux) {
    static constexpr std::array tbl = {
        "G_ACMUX_COMBINED or G_ACMUX_LOD_FRACTION",
        "G_ACMUX_TEXEL0",
        "G_ACMUX_TEXEL1",
        "G_ACMUX_PRIMITIVE",
        "G_ACMUX_SHADE",
        "G_ACMUX_ENVIRONMENT",
        "G_ACMUX_1 or G_ACMUX_PRIM_LOD_FRAC",
        "G_ACMUX_0",
    };
    return tbl[acmux];
}

void Interpreter::GenerateCC(ColorCombiner* comb, const ColorCombinerKey& key) {
    const bool is2Cyc = (key.options & SHADER_OPT(_2CYC)) != 0;

    uint8_t c[2][2][4];
    uint64_t shaderId0 = 0;
    uint32_t shaderId1 = key.options;
    uint8_t shaderInputMapping[2][7] = { { 0 } };
    bool usedTextures[2]{};
    for (uint32_t i = 0; i < 2 && (i == 0 || is2Cyc); i++) {
```
```text
        uint32_t rgbA = (key.combine_mode >> (i * 28)) & 0xf;
        uint32_t rgbB = (key.combine_mode >> (i * 28 + 4)) & 0xf;
        uint32_t rgbC = (key.combine_mode >> (i * 28 + 8)) & 0x1f;
        uint32_t rgbD = (key.combine_mode >> (i * 28 + 13)) & 7;
        uint32_t alphaA = (key.combine_mode >> (i * 28 + 16)) & 7;
        uint32_t alphaB = (key.combine_mode >> (i * 28 + 16 + 3)) & 7;
        uint32_t alphaC = (key.combine_mode >> (i * 28 + 16 + 6)) & 7;
        uint32_t alphaD = (key.combine_mode >> (i * 28 + 16 + 9)) & 7;

        if (rgbA >= 8) {
            rgbA = G_CCMUX_0;
        }
        if (rgbB >= 8) {
            rgbB = G_CCMUX_0;
        }
        if (rgbC >= 16) {
            rgbC = G_CCMUX_0;
        }
        if (rgbD == 7) {
            rgbD = G_CCMUX_0;
        }

        if (rgbA == rgbB || rgbC == G_CCMUX_0) {
            // Normalize
            rgbA = G_CCMUX_0;
            rgbB = G_CCMUX_0;
            rgbC = G_CCMUX_0;
        }
        if (alphaA == alphaB || alphaC == G_ACMUX_0) {
            // Normalize
            alphaA = G_ACMUX_0;
            alphaB = G_ACMUX_0;
            alphaC = G_ACMUX_0;
        }
        if (i == 1) {
            if (rgbA != G_CCMUX_COMBINED && rgbB != G_CCMUX_COMBINED && rgbC != G_CCMUX_COMBINED &&
                rgbD != G_CCMUX_COMBINED) {
                // First cycle RGB not used, so clear it away
                c[0][0][0] = c[0][0][1] = c[0][0][2] = c[0][0][3] = G_CCMUX_0;
            }
            if (rgbC != G_CCMUX_COMBINED_ALPHA && alphaA != G_ACMUX_COMBINED && alphaB != G_ACMUX_COMBINED &&
                alphaD != G_ACMUX_COMBINED) {
                // First cycle ALPHA not used, so clear it away
                c[0][1][0] = c[0][1][1] = c[0][1][2] = c[0][1][3] = G_ACMUX_0;
            }
        }

        c[i][0][0] = rgbA;
        c[i][0][1] = rgbB;
        c[i][0][2] = rgbC;
        c[i][0][3] = rgbD;
        c[i][1][0] = alphaA;
        c[i][1][1] = alphaB;
        c[i][1][2] = alphaC;
        c[i][1][3] = alphaD;
    }
    if (!is2Cyc) {
        for (uint32_t i = 0; i < 2; i++) {
            for (uint32_t k = 0; k < 4; k++) {
                c[1][i][k] = i == 0 ? G_CCMUX_0 : G_ACMUX_0;
            }
        }
    }
    {
        uint8_t inputNumber[32] = { 0 };
        uint32_t nextInputNumber = SHADER_INPUT_1;
        for (uint32_t i = 0; i < 2 && (i == 0 || is2Cyc); i++) {
            for (uint32_t j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][0][j]) {
                    case G_CCMUX_0:
                        val = SHADER_0;
                        break;
                    case G_CCMUX_1:
                        val = SHADER_1;
                        break;
                    case G_CCMUX_TEXEL0:
                        val = SHADER_TEXEL0;
                        // Set the opposite texture when reading from the second cycle color options
                        if (i == 0) {
                            usedTextures[0] = true;
                        } else {
                            usedTextures[1] = true;
                        }
                        break;
                    case G_CCMUX_TEXEL1:
                        val = SHADER_TEXEL1;
                        if (i == 0) {
                            usedTextures[1] = true;
                        } else {
                            usedTextures[0] = true;
                        }
                        break;
                    case G_CCMUX_TEXEL0_ALPHA:
                        val = SHADER_TEXEL0A;
                        if (i == 0) {
                            usedTextures[0] = true;
                        } else {
                            usedTextures[1] = true;
                        }
                        break;
                    case G_CCMUX_TEXEL1_ALPHA:
                        val = SHADER_TEXEL1A;
                        if (i == 0) {
                            usedTextures[1] = true;
                        } else {
                            usedTextures[0] = true;
                        }
                        break;
                    case G_CCMUX_NOISE:
                        val = SHADER_NOISE;
                        break;
                    case G_CCMUX_PRIMITIVE:
                    case G_CCMUX_PRIMITIVE_ALPHA:
                    case G_CCMUX_PRIM_LOD_FRAC:
                    case G_CCMUX_SHADE:
                    case G_CCMUX_ENVIRONMENT:
                    case G_CCMUX_ENV_ALPHA:
                    case G_CCMUX_LOD_FRACTION:
                        if (inputNumber[c[i][0][j]] == 0) {
                            shaderInputMapping[0][nextInputNumber - 1] = c[i][0][j];
                            inputNumber[c[i][0][j]] = nextInputNumber++;
                        }
                        val = inputNumber[c[i][0][j]];
                        break;
                    case G_CCMUX_COMBINED:
                        val = SHADER_COMBINED;
                        break;
                    default:
                        fprintf(stderr, "Unsupported ccmux: %d\n", c[i][0][j]);
                        break;
                }
                shaderId0 |= (uint64_t)val << (i * 32 + j * 4);
            }
        }
    }
    {
        uint8_t inputNumber[16] = { 0 };
        uint32_t nextInputNumber = SHADER_INPUT_1;
        for (uint32_t i = 0; i < 2; i++) {
            for (uint32_t j = 0; j < 4; j++) {
                uint32_t val = 0;
                switch (c[i][1][j]) {
                    case G_ACMUX_0:
                        val = SHADER_0;
                        break;
                    case G_ACMUX_TEXEL0:
                        val = SHADER_TEXEL0;
                        // Set the opposite texture when reading from the second cycle color options
                        if (i == 0) {
                            usedTextures[0] = true;
                        } else {
                            usedTextures[1] = true;
                        }
                        break;
                    case G_ACMUX_TEXEL1:
                        val = SHADER_TEXEL1;
                        if (i == 0) {
                            usedTextures[1] = true;
                        } else {
                            usedTextures[0] = true;
                        }
                        break;
                    case G_ACMUX_LOD_FRACTION:
                        // case G_ACMUX_COMBINED: same numerical value
                        if (j != 2) {
                            val = SHADER_COMBINED;
                            break;
                        }
                        c[i][1][j] = G_CCMUX_LOD_FRACTION;
                        [[fallthrough]]; // for G_ACMUX_LOD_FRACTION
                    case G_ACMUX_1:
                        // case G_ACMUX_PRIM_LOD_FRAC: same numerical value
                        if (j != 2) {
                            val = SHADER_1;
                            break;
                        }
                        [[fallthrough]]; // for G_ACMUX_PRIM_LOD_FRAC
                    case G_ACMUX_PRIMITIVE:
                    case G_ACMUX_SHADE:
                    case G_ACMUX_ENVIRONMENT:
                        if (inputNumber[c[i][1][j]] == 0) {
                            shaderInputMapping[1][nextInputNumber - 1] = c[i][1][j];
                            inputNumber[c[i][1][j]] = nextInputNumber++;
                        }
                        val = inputNumber[c[i][1][j]];
                        break;
                }
                shaderId0 |= (uint64_t)val << (i * 32 + 16 + j * 4);
            }
        }
    }
    comb->shader_id0 = shaderId0;
    comb->shader_id1 = shaderId1;
    comb->usedTextures[0] = usedTextures[0];
    comb->usedTextures[1] = usedTextures[1];
    // comb->prg = gfx_lookup_or_create_mShaderProgram(shader_id0, shader_id1);
    memcpy(comb->shader_input_mapping, shaderInputMapping, sizeof(shaderInputMapping));
}

ColorCombiner* Interpreter::LookupOrCreateColorCombiner(const ColorCombinerKey& key) {
    if (mPrevCombiner != mColorCombinerPool.end() && mPrevCombiner->first == key) {
        return &mPrevCombiner->second;
    }
    mPrevCombiner = mColorCombinerPool.find(key);
    if (mPrevCombiner != mColorCombinerPool.end()) {
        return &mPrevCombiner->second;
    }
    Flush();
    mPrevCombiner = mColorCombinerPool.insert(std::make_pair(key, ColorCombiner())).first;
    GenerateCC(&mPrevCombiner->second, key);
    return &mPrevCombiner->second;
}

void Interpreter::TextureCacheClear() {
    for (const auto& entry : mTextureCache.map) {
        mTextureCache.free_texture_ids.push_back(entry.second.texture_id);
    }
    mTextureCache.map.clear();
    mTextureCache.lru.clear();
```
```text
}

bool Interpreter::TextureCacheLookup(int i, const TextureCacheKey& key) {
    TextureCacheMap::iterator it = mTextureCache.map.find(key);
    TextureCacheNode** n = &mRenderingState.mTextures[i];

    if (it != mTextureCache.map.end()) {
        mRapi->SelectTexture(i, it->second.texture_id);
        *n = &*it;
        mTextureCache.lru.splice(mTextureCache.lru.end(), mTextureCache.lru,
                                 it->second.lru_location); // move to back
        return true;
    }

    if (mTextureCache.map.size() >= TEXTURE_CACHE_MAX_SIZE) {
        // Remove the texture that was least recently used
        it = mTextureCache.lru.front().it;
        mTextureCache.free_texture_ids.push_back(it->second.texture_id);
        mTextureCache.map.erase(it);
        mTextureCache.lru.pop_front();
    }

    uint32_t texture_id;
    if (!mTextureCache.free_texture_ids.empty()) {
        texture_id = mTextureCache.free_texture_ids.back();
        mTextureCache.free_texture_ids.pop_back();
    } else {
        texture_id = mRapi->NewTexture();
    }

    it = mTextureCache.map.insert(std::make_pair(key, TextureCacheValue())).first;
    TextureCacheNode* node = &*it;
    node->second.texture_id = texture_id;
    node->second.lru_location = mTextureCache.lru.insert(mTextureCache.lru.end(), { it });

    mRapi->SelectTexture(i, texture_id);
    mRapi->SetSamplerParameters(i, false, 0, 0);
    *n = node;
    return false;
}

std::string Interpreter::GetBaseTexturePath(const std::string& path) {
    if (path.starts_with(Ship::IResource::gAltAssetPrefix)) {
        return path.substr(Ship::IResource::gAltAssetPrefix.length());
    }

    return path;
}

void Interpreter::TextureCacheDelete(const uint8_t* origAddr) {
    while (mTextureCache.map.bucket_count() > 0) {
        TextureCacheKey key = { origAddr, { 0 }, 0, 0, 0 }; // bucket index only depends on the address
        size_t bucket = mTextureCache.map.bucket(key);
        bool again = false;
        for (auto it = mTextureCache.map.begin(bucket); it != mTextureCache.map.end(bucket); ++it) {
            if (it->first.texture_addr == origAddr) {
                mTextureCache.lru.erase(it->second.lru_location);
                mTextureCache.free_texture_ids.push_back(it->second.texture_id);
                mTextureCache.map.erase(it->first);
                again = true;
                break;
            }
        }
        if (!again) {
            break;
        }
    }
}

void Interpreter::ImportTextureRgba16(int tile, bool importReplacement) {
    const RawTexMetadata* metadata = &mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].raw_tex_metadata;
    const uint8_t* addr =
        importReplacement && (metadata->resource != nullptr)
            ? mMaskedTextures.find(GetBaseTexturePath(metadata->resource->GetInitData()->Path))->second.replacementData
            : mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].addr;
    uint32_t sizeBytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].size_bytes;
    uint32_t fullImageLineSizeBytes =
        mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].line_size_bytes;

    uint32_t width = mRdp->texture_tile[tile].line_size_bytes / 2;
    uint32_t height = sizeBytes / mRdp->texture_tile[tile].line_size_bytes;

    // A single line of pixels should not equal the entire image (height == 1 non-withstanding)
    if (fullImageLineSizeBytes == sizeBytes) {
        fullImageLineSizeBytes = width * 2;
    }

    uint32_t i = 0;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t clrIdx = (y * (fullImageLineSizeBytes / 2)) + (x);

            uint16_t col16 = (addr[2 * clrIdx] << 8) | addr[2 * clrIdx + 1];
            uint8_t a = col16 & 1;
            uint8_t r = col16 >> 11;
            uint8_t g = (col16 >> 6) & 0x1f;
            uint8_t b = (col16 >> 1) & 0x1f;
            mTexUploadBuffer[4 * i + 0] = SCALE_5_8(r);
            mTexUploadBuffer[4 * i + 1] = SCALE_5_8(g);
            mTexUploadBuffer[4 * i + 2] = SCALE_5_8(b);
            mTexUploadBuffer[4 * i + 3] = a ? 255 : 0;

            i++;
        }
    }

    mRapi->UploadTexture(mTexUploadBuffer, width, height);
}

void Interpreter::ImportTextureRgba32(int tile, bool importReplacement) {
    const RawTexMetadata* metadata = &mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].raw_tex_metadata;
    const uint8_t* addr =
        importReplacement && (metadata->resource != nullptr)
            ? mMaskedTextures.find(GetBaseTexturePath(metadata->resource->GetInitData()->Path))->second.replacementData
            : mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes =
        mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(full_image_line_size_bytes == line_size_bytes);

    uint32_t width = mRdp->texture_tile[tile].line_size_bytes / 2;
    uint32_t height = (size_bytes / 2) / mRdp->texture_tile[tile].line_size_bytes;
    mRapi->UploadTexture(addr, width, height);
}

void Interpreter::ImportTextureIA4(int tile, bool importReplacement) {
    const RawTexMetadata* metadata = &mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].raw_tex_metadata;
    const uint8_t* addr =
        importReplacement && (metadata->resource != nullptr)
            ? mMaskedTextures.find(GetBaseTexturePath(metadata->resource->GetInitData()->Path))->second.replacementData
            : mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].addr;
    uint32_t sizeBytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].size_bytes;
    uint32_t fullImageLineSizeBytes =
        mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t lineSizeBytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(fullImageLineSizeBytes == lineSizeBytes);

    for (uint32_t i = 0; i < sizeBytes * 2; i++) {
        uint8_t byte = addr[i / 2];
        uint8_t part = (byte >> (4 - (i % 2) * 4)) & 0xf;
        uint8_t intensity = part >> 1;
        uint8_t alpha = part & 1;
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        mTexUploadBuffer[4 * i + 0] = SCALE_3_8(r);
        mTexUploadBuffer[4 * i + 1] = SCALE_3_8(g);
        mTexUploadBuffer[4 * i + 2] = SCALE_3_8(b);
        mTexUploadBuffer[4 * i + 3] = alpha ? 255 : 0;
    }

    uint32_t width = mRdp->texture_tile[tile].line_size_bytes * 2;
    uint32_t height = sizeBytes / mRdp->texture_tile[tile].line_size_bytes;

    mRapi->UploadTexture(mTexUploadBuffer, width, height);
}

void Interpreter::ImportTextureIA8(int tile, bool importReplacement) {
    const RawTexMetadata* metadata = &mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].raw_tex_metadata;
    const uint8_t* addr =
        importReplacement && (metadata->resource != nullptr)
            ? mMaskedTextures.find(GetBaseTexturePath(metadata->resource->GetInitData()->Path))->second.replacementData
            : mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].addr;
    uint32_t sizeBytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].size_bytes;
    uint32_t fullImageLineSizeBytes =
        mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t lineSizeBytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].line_size_bytes;
    SUPPORT_CHECK(fullImageLineSizeBytes == lineSizeBytes);

    for (uint32_t i = 0; i < sizeBytes; i++) {
        uint8_t intensity = addr[i] >> 4;
        uint8_t alpha = addr[i] & 0xf;
        uint8_t r = intensity;
        uint8_t g = intensity;
        uint8_t b = intensity;
        mTexUploadBuffer[4 * i + 0] = SCALE_4_8(r);
        mTexUploadBuffer[4 * i + 1] = SCALE_4_8(g);
        mTexUploadBuffer[4 * i + 2] = SCALE_4_8(b);
        mTexUploadBuffer[4 * i + 3] = SCALE_4_8(alpha);
    }

    uint32_t width = mRdp->texture_tile[tile].line_size_bytes;
    uint32_t height = sizeBytes / mRdp->texture_tile[tile].line_size_bytes;

    mRapi->UploadTexture(mTexUploadBuffer, width, height);
}

void Interpreter::ImportTextureIA16(int tile, bool importReplacement) {
    const RawTexMetadata* metadata = &mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].raw_tex_metadata;
    const uint8_t* addr =
        importReplacement && (metadata->resource != nullptr)
            ? mMaskedTextures.find(GetBaseTexturePath(metadata->resource->GetInitData()->Path))->second.replacementData
            : mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].addr;
    uint32_t size_bytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].size_bytes;
    uint32_t full_image_line_size_bytes =
        mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].full_image_line_size_bytes;
    uint32_t line_size_bytes = mRdp->loaded_texture[mRdp->texture_tile[tile].tmem_index].line_size_bytes;

    uint32_t width = mRdp->texture_tile[tile].line_size_bytes / 2;
    uint32_t height = size_bytes / mRdp->texture_tile[tile].line_size_bytes;

    // A single line of pixels should not equal the entire image (height == 1 non-withstanding)
    if (full_image_line_size_bytes == size_bytes) {
        full_image_line_size_bytes = width * 2;
    }

    uint32_t i = 0;

    for (uint32_t y = 0; y < height; y++) {
        for (uint32_t x = 0; x < width; x++) {
            uint32_t clrIdx = (y * (full_image_line_size_bytes / 2)) + (x);

            uint8_t intensity = addr[2 * clrIdx];
            uint8_t alpha = addr[2 * clrIdx + 1];
            uint8_t r = intensity;
            uint8_t g = intensity;
            uint8_t b = intensity;
```

## file: libultraship/src/fast/backends/gfx_opengl.cpp
```text
#include "ship/window/Window.h"
#ifdef ENABLE_OPENGL

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include <algorithm>
#include <array>
#include <cmath>
#include <map>
#include <unordered_map>
#include <vector>

#ifndef _LANGUAGE_C
#define _LANGUAGE_C
#endif

#ifdef __MINGW32__
#define FOR_WINDOWS 1
#else
#define FOR_WINDOWS 0
#endif

#include "fast/backends/gfx_opengl.h"
#include "ship/window/gui/Gui.h"
#include <prism/processor.h>
#include <fstream>
#include "ship/Context.h"
#include "ship/resource/factory/ShaderFactory.h"
#include "fast/interpreter.h"
#include "ship/config/ConsoleVariable.h"
#include <spdlog/spdlog.h>

namespace Fast {
namespace {
constexpr int32_t kAoFallbackNone = 0;
constexpr int32_t kAoFallbackBackendUnsupportedMetal = 1;
constexpr int32_t kAoFallbackShaderCompileFailed = 2;
constexpr int32_t kAoFallbackRuntimeUnsupported = 3;

GLuint CompileShader(GLenum shaderType, const char* source) {
    GLuint shader = glCreateShader(shaderType);
    glShaderSource(shader, 1, &source, nullptr);
    glCompileShader(shader);
    GLint status = GL_FALSE;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status == GL_TRUE) {
        return shader;
    }

    GLint logLength = 0;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(static_cast<size_t>(std::max(logLength, 1)));
    glGetShaderInfoLog(shader, logLength, nullptr, log.data());
    SPDLOG_ERROR("[FAST][AO] OpenGL shader compile failed: {}", log.data());
    glDeleteShader(shader);
    return 0;
}

GLuint LinkProgram(GLuint vertexShader, GLuint fragmentShader) {
    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);
    GLint status = GL_FALSE;
    glGetProgramiv(program, GL_LINK_STATUS, &status);
    if (status == GL_TRUE) {
        return program;
    }

    GLint logLength = 0;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
    std::vector<char> log(static_cast<size_t>(std::max(logLength, 1)));
    glGetProgramInfoLog(program, logLength, nullptr, log.data());
    SPDLOG_ERROR("[FAST][AO] OpenGL program link failed: {}", log.data());
    glDeleteProgram(program);
    return 0;
}

const char* kFullscreenTriangleVs = R"(
#version 330 core
void main() {
    vec2 pos = vec2((gl_VertexID << 1) & 2, gl_VertexID & 2);
    gl_Position = vec4(pos * 2.0 - 1.0, 0.0, 1.0);
}
)";

const char* kAoGenerateFs = R"(
#version 330 core
uniform sampler2D uDepthTex;
uniform vec2 uInvResolution;
uniform float uRadiusPixels;
uniform float uIntensity;
uniform float uBias;
uniform float uPower;
uniform float uMaxDistance;
uniform int uSampleCount;
out float outAo;

float ReadDepth(vec2 uv) {
    return texture(uDepthTex, clamp(uv, vec2(0.0), vec2(1.0))).r;
}

void main() {
    vec2 uv = gl_FragCoord.xy * uInvResolution;
    float centerDepth = ReadDepth(uv);
    if (centerDepth >= 0.9999) {
        outAo = 1.0;
        return;
    }

    vec2 kernel[16] = vec2[16](
        vec2(1.0, 0.0), vec2(-1.0, 0.0), vec2(0.0, 1.0), vec2(0.0, -1.0),
        vec2(0.707, 0.707), vec2(-0.707, 0.707), vec2(0.707, -0.707), vec2(-0.707, -0.707),
        vec2(2.0, 0.0), vec2(-2.0, 0.0), vec2(0.0, 2.0), vec2(0.0, -2.0),
        vec2(1.5, 0.5), vec2(-1.5, 0.5), vec2(1.5, -0.5), vec2(-1.5, -0.5)
    );

    float occlusion = 0.0;
    float maxDistanceDepth = uMaxDistance * 0.0001;
    int samples = clamp(uSampleCount, 4, 16);
    for (int i = 0; i < samples; i++) {
        vec2 offset = kernel[i] * uRadiusPixels * uInvResolution;
        float sampleDepth = ReadDepth(uv + offset);
        float delta = sampleDepth - centerDepth;
        if (delta > uBias && delta < maxDistanceDepth) {
            occlusion += 1.0;
        }
    }
    float ao = 1.0 - (occlusion / float(samples)) * uIntensity;
    outAo = pow(clamp(ao, 0.0, 1.0), max(uPower, 0.0001));
}
)";

const char* kAoBlurFs = R"(
#version 330 core
uniform sampler2D uAoTex;
uniform vec2 uInvResolution;
uniform vec2 uDirection;
out float outAo;

void main() {
    vec2 uv = gl_FragCoord.xy * uInvResolution;
    float w0 = 0.227027f;
    float w1 = 0.316216f;
    float w2 = 0.070270f;
    float result = texture(uAoTex, uv).r * w0;
    result += texture(uAoTex, uv + uDirection * 1.384615f * uInvResolution).r * w1;
    result += texture(uAoTex, uv - uDirection * 1.384615f * uInvResolution).r * w1;
    result += texture(uAoTex, uv + uDirection * 3.230769f * uInvResolution).r * w2;
    result += texture(uAoTex, uv - uDirection * 3.230769f * uInvResolution).r * w2;
    outAo = clamp(result, 0.0, 1.0);
}
)";

const char* kAoCompositeFs = R"(
#version 330 core
uniform sampler2D uAoTex;
uniform vec2 uInvResolution;
uniform int uDebugView;
out vec4 outColor;

void main() {
    vec2 uv = gl_FragCoord.xy * uInvResolution;
    float ao = texture(uAoTex, uv).r;
    if (uDebugView == 1) {
        outColor = vec4(vec3(ao), 1.0);
    } else {
        outColor = vec4(ao, ao, ao, 1.0);
    }
}
)";

constexpr int32_t kVolumetricsFallbackNone = 0;
constexpr int32_t kVolumetricsFallbackBackendUnsupportedMetal = 1;
constexpr int32_t kVolumetricsFallbackShaderCompileFailed = 2;
constexpr int32_t kVolumetricsFallbackRuntimeUnsupported = 3;

const char* kVolumetricsRaymarchFs = R"(
#version 330 core
#define MAX_LIGHTS 8
uniform sampler2D uDepthTex;
uniform sampler2D uPrevVolumeTex;
uniform vec2 uInvFullResolution;
uniform vec2 uInvLowResolution;
uniform vec3 uCameraWorldPosition;
uniform float uDensity;
uniform float uAnisotropy;
uniform float uStartDistance;
uniform float uMaxDistance;
uniform float uBaseHeight;
uniform float uHeightFalloff;
uniform float uLightShaftIntensity;
uniform float uShadowIntensity;
uniform float uTemporalBlend;
uniform float uJitterScale;
uniform int uStepCount;
uniform int uLightCount;
uniform int uShadowQuality;
uniform int uHistoryValid;
uniform int uHeightFogEnabled;
uniform vec2 uDepthNdcParams;
uniform vec2 uNdcDepthParams;
uniform mat4 uCurrViewProj;
uniform mat4 uInvViewProj;
uniform mat4 uPrevViewProj;
uniform vec4 uLightPositionRadius[MAX_LIGHTS];
uniform vec4 uLightDirectionIntensity[MAX_LIGHTS];
uniform vec4 uLightColorVolumetric[MAX_LIGHTS];
uniform vec4 uLightConeShadow[MAX_LIGHTS];
uniform vec4 uLightShadowParams[MAX_LIGHTS];
layout(location = 0) out vec4 outColor;
layout(location = 1) out float outDepth;

float ReadDepth(vec2 uv) {
    return texture(uDepthTex, clamp(uv, vec2(0.0), vec2(1.0))).r;
}

float Noise(vec2 p) {
```
```text
    return fract(52.9829189 * fract(dot(p, vec2(0.06711056, 0.00583715))));
}

vec3 ReconstructWorldPosition(vec2 uv, float depth) {
    float ndcZ = depth * uDepthNdcParams.x + uDepthNdcParams.y;
    vec4 clip = vec4(uv * 2.0 - 1.0, ndcZ, 1.0);
    vec4 world = uInvViewProj * clip;
    return world.xyz / max(world.w, 0.0001);
}

bool ProjectWorld(mat4 viewProj, vec3 worldPos, out vec2 uv, out float deviceDepth) {
    vec4 clip = viewProj * vec4(worldPos, 1.0);
    if (clip.w <= 0.0001) {
        uv = vec2(-1.0);
        deviceDepth = 1.0;
        return false;
    }
    vec3 ndc = clip.xyz / clip.w;
    uv = ndc.xy * 0.5 + 0.5;
    deviceDepth = ndc.z * uNdcDepthParams.x + uNdcDepthParams.y;
    return uv.x >= 0.0 && uv.x <= 1.0 && uv.y >= 0.0 && uv.y <= 1.0;
}

float PhaseHG(float cosTheta, float g) {
    float g2 = g * g;
    float denom = pow(max(1.0 + g2 - 2.0 * g * cosTheta, 0.001), 1.5);
    return (1.0 - g2) / max(12.5663706 * denom, 0.001);
}

float ComputeShadow(int lightIndex, vec3 sampleWorldPos) {
    if (uShadowQuality <= 0 || uLightShadowParams[lightIndex].w < 0.5) {
        return 1.0;
    }

    vec3 traceDirection = vec3(0.0);
    float maxTraceDistance = max(uLightConeShadow[lightIndex].w, 0.0);
    int lightType = int(uLightShadowParams[lightIndex].z + 0.5);
    if (lightType == 0) {
        traceDirection = normalize(-uLightDirectionIntensity[lightIndex].xyz);
    } else {
        vec3 toLight = uLightPositionRadius[lightIndex].xyz - sampleWorldPos;
        float lengthToLight = length(toLight);
        if (lengthToLight <= 0.001) {
            return 1.0;
        }
        traceDirection = toLight / lengthToLight;
        maxTraceDistance = min(maxTraceDistance, lengthToLight);
    }
    if (maxTraceDistance <= 0.001) {
        return 1.0;
    }

    float occlusion = 0.0;
    int shadowSamples = uShadowQuality == 1 ? 2 : (uShadowQuality == 2 ? 3 : 4);
    float shadowBias = uLightConeShadow[lightIndex].z + uLightShadowParams[lightIndex].x * 0.01;
    for (int sampleIndex = 1; sampleIndex <= shadowSamples; ++sampleIndex) {
        float travel = maxTraceDistance * (float(sampleIndex) / float(shadowSamples));
        vec3 probeWorldPos = sampleWorldPos + traceDirection * travel;
        vec2 probeUv;
        float probeDepth;
        if (!ProjectWorld(uCurrViewProj, probeWorldPos, probeUv, probeDepth)) {
            continue;
        }
        float sceneDepth = ReadDepth(probeUv);
        if (sceneDepth + shadowBias < probeDepth) {
            occlusion += 1.0;
        }
    }

    return clamp(1.0 - (occlusion / float(shadowSamples)) * uShadowIntensity, 0.0, 1.0);
}

float ComputeLightAttenuation(int lightIndex, vec3 sampleWorldPos, out vec3 lightDir) {
    int lightType = int(uLightShadowParams[lightIndex].z + 0.5);
    lightDir = normalize(-uLightDirectionIntensity[lightIndex].xyz);
    if (lightType == 0) {
        return 1.0;
    }

    vec3 toLight = uLightPositionRadius[lightIndex].xyz - sampleWorldPos;
    float distanceToLight = length(toLight);
    if (distanceToLight <= 0.001) {
        return 0.0;
    }
    lightDir = toLight / distanceToLight;
    float radius = max(uLightPositionRadius[lightIndex].w, 0.001);
    float attenuation = clamp(1.0 - distanceToLight / radius, 0.0, 1.0);
    attenuation *= attenuation;

    if (lightType == 2) {
        float coneDot = dot(normalize(sampleWorldPos - uLightPositionRadius[lightIndex].xyz),
                            normalize(uLightDirectionIntensity[lightIndex].xyz));
        float innerCos = uLightConeShadow[lightIndex].x;
        float outerCos = uLightConeShadow[lightIndex].y;
        float coneAttenuation = smoothstep(outerCos, innerCos, coneDot);
        attenuation *= coneAttenuation;
    }

    return attenuation;
}

void main() {
    vec2 uv = gl_FragCoord.xy * uInvLowResolution;
    float fullDepth = ReadDepth(uv);
    outDepth = fullDepth;
    if (fullDepth >= 0.9999) {
        outColor = vec4(0.0);
        return;
    }

    vec3 surfaceWorldPos = ReconstructWorldPosition(uv, fullDepth);
    vec3 viewVector = surfaceWorldPos - uCameraWorldPosition;
    float surfaceDistance = min(length(viewVector), uMaxDistance);
    if (surfaceDistance <= uStartDistance + 0.001) {
        outColor = vec4(0.0, 0.0, 0.0, 1.0);
        return;
    }

    vec3 rayDirection = normalize(viewVector);
    int stepCount = clamp(uStepCount, 8, 64);
    float stepSize = (surfaceDistance - uStartDistance) / float(stepCount);
    float jitter = (Noise(gl_FragCoord.xy) - 0.5) * uJitterScale;
    float t = uStartDistance + stepSize * (0.5 + jitter);
    vec3 scattering = vec3(0.0);
    float transmittance = 1.0;

    for (int stepIndex = 0; stepIndex < 64; ++stepIndex) {
        if (stepIndex >= stepCount || t >= surfaceDistance || t >= uMaxDistance) {
            break;
        }

        vec3 sampleWorldPos = uCameraWorldPosition + rayDirection * t;
        float density = uDensity;
        if (uHeightFogEnabled == 1) {
            density *= exp(-max(sampleWorldPos.y - uBaseHeight, 0.0) * uHeightFalloff);
        }
        density = max(density, 0.0);
        float mediumStep = density * stepSize * 0.01;
        if (mediumStep > 0.0) {
            vec3 stepLighting = vec3(0.0);
            for (int lightIndex = 0; lightIndex < MAX_LIGHTS; ++lightIndex) {
                if (lightIndex >= uLightCount) {
                    break;
                }
                vec3 lightDir;
                float attenuation = ComputeLightAttenuation(lightIndex, sampleWorldPos, lightDir);
                if (attenuation <= 0.0001) {
                    continue;
                }
                float visibility = ComputeShadow(lightIndex, sampleWorldPos);
                float phase = PhaseHG(dot(lightDir, -rayDirection), uAnisotropy);
                vec3 lightColor = uLightColorVolumetric[lightIndex].rgb;
                float lightIntensity = uLightDirectionIntensity[lightIndex].w * uLightColorVolumetric[lightIndex].a;
                stepLighting += lightColor * (lightIntensity * attenuation * visibility * phase);
            }
            scattering += stepLighting * (mediumStep * transmittance * uLightShaftIntensity);
            transmittance *= exp(-mediumStep * 1.35);
            if (transmittance <= 0.01) {
                break;
            }
        }
        t += stepSize;
    }

    if (uHistoryValid == 1 && uTemporalBlend > 0.0) {
        vec2 prevUv;
        float prevDepth;
        if (ProjectWorld(uPrevViewProj, surfaceWorldPos, prevUv, prevDepth)) {
            vec3 prevColor = texture(uPrevVolumeTex, prevUv).rgb;
            scattering = mix(scattering, prevColor, uTemporalBlend);
        }
    }

    outColor = vec4(max(scattering, vec3(0.0)), 1.0);
}
)";

const char* kVolumetricsCompositeFs = R"(
#version 330 core
uniform sampler2D uVolumeTex;
uniform sampler2D uLowDepthTex;
uniform sampler2D uFullDepthTex;
uniform vec2 uInvFullResolution;
uniform vec2 uInvLowResolution;
uniform int uDebugView;
out vec4 outColor;

float ReadFullDepth(vec2 uv) {
    return texture(uFullDepthTex, clamp(uv, vec2(0.0), vec2(1.0))).r;
}

vec3 SampleUpscaled(vec2 uv, float fullDepth) {
    vec2 lowSize = vec2(1.0 / uInvLowResolution.x, 1.0 / uInvLowResolution.y);
    vec2 pixel = uv * lowSize - 0.5;
    vec2 base = floor(pixel);
    vec2 frac = fract(pixel);
    vec3 accum = vec3(0.0);
    float weightSum = 0.0;
    for (int oy = 0; oy < 2; ++oy) {
        for (int ox = 0; ox < 2; ++ox) {
            vec2 coord = base + vec2(float(ox), float(oy)) + 0.5;
            vec2 sampleUv = clamp(coord * uInvLowResolution, vec2(0.0), vec2(1.0));
            float bilinear = (ox == 0 ? 1.0 - frac.x : frac.x) * (oy == 0 ? 1.0 - frac.y : frac.y);
            float lowDepth = texture(uLowDepthTex, sampleUv).r;
            float depthWeight = exp(-abs(lowDepth - fullDepth) * 128.0);
            float weight = max(bilinear * depthWeight, 0.0001);
            accum += texture(uVolumeTex, sampleUv).rgb * weight;
            weightSum += weight;
        }
    }
    return accum / max(weightSum, 0.0001);
}

void main() {
    vec2 uv = gl_FragCoord.xy * uInvFullResolution;
    float fullDepth = ReadFullDepth(uv);
    vec3 volume = SampleUpscaled(uv, fullDepth);
    if (uDebugView == 1) {
        outColor = vec4(volume, 1.0);
    } else {
```
```text
        outColor = vec4(volume, 1.0);
    }
}
)";
} // namespace

int GfxRenderingAPIOGL::GetMaxTextureSize() {
    GLint max_texture_size;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &max_texture_size);
    return max_texture_size;
}

const char* GfxRenderingAPIOGL::GetName() {
    return "OpenGL";
}

GfxClipParameters GfxRenderingAPIOGL::GetClipParameters() {
    return { false, mFrameBuffers[mCurrentFrameBuffer].invertY };
}

static void VertexArraySetAttribs(ShaderProgram* prg) {
    size_t numFloats = prg->numFloats;
    size_t pos = 0;

    for (int i = 0; i < prg->numAttribs; i++) {
        glEnableVertexAttribArray(prg->attribLocations[i]);
        glVertexAttribPointer(prg->attribLocations[i], prg->attribSizes[i], GL_FLOAT, GL_FALSE,
                              numFloats * sizeof(float), (void*)(pos * sizeof(float)));
        pos += prg->attribSizes[i];
    }
}

void GfxRenderingAPIOGL::SetUniforms(ShaderProgram* prg) const {
    glUniform1i(prg->frameCountLocation, mFrameCount);
    glUniform1f(prg->noiseScaleLocation, mCurrentNoiseScale);
}

void GfxRenderingAPIOGL::SetPerDrawUniforms() {
    if (mCurrentShaderProgram->usedTextures[0] || mCurrentShaderProgram->usedTextures[1]) {
        GLint filtering[2] = { textures[mCurrentTextureIds[0]].filtering, textures[mCurrentTextureIds[1]].filtering };
        glUniform1iv(mCurrentShaderProgram->texture_filtering_location, 2, filtering);

        GLint width[2] = { textures[mCurrentTextureIds[0]].width, textures[mCurrentTextureIds[1]].width };
        glUniform1iv(mCurrentShaderProgram->texture_width_location, 2, width);

        GLint height[2] = { textures[mCurrentTextureIds[0]].height, textures[mCurrentTextureIds[1]].height };
        glUniform1iv(mCurrentShaderProgram->texture_height_location, 2, height);
    }
}

void GfxRenderingAPIOGL::UnloadShader(ShaderProgram* old_prg) {
    if (old_prg != nullptr) {
        for (unsigned int i = 0; i < old_prg->numAttribs; i++) {
            glDisableVertexAttribArray(old_prg->attribLocations[i]);
        }
    }
}

void GfxRenderingAPIOGL::LoadShader(ShaderProgram* new_prg) {
    // if (!new_prg) return;
    mCurrentShaderProgram = new_prg;
    glUseProgram(new_prg->openglProgramId);
    VertexArraySetAttribs(new_prg);
    SetUniforms(new_prg);
}

#define RAND_NOISE "((random(vec3(floor(gl_FragCoord.xy * noise_scale), float(frame_count))) + 1.0) / 2.0)"

static const char* shader_item_to_str(uint32_t item, bool with_alpha, bool only_alpha, bool inputs_have_alpha,
                                      bool first_cycle, bool hint_single_element) {
    if (!only_alpha) {
        switch (item) {
            case SHADER_0:
                return with_alpha ? "vec4(0.0, 0.0, 0.0, 0.0)" : "vec3(0.0, 0.0, 0.0)";
            case SHADER_1:
                return with_alpha ? "vec4(1.0, 1.0, 1.0, 1.0)" : "vec3(1.0, 1.0, 1.0)";
            case SHADER_INPUT_1:
                return with_alpha || !inputs_have_alpha ? "vInput1" : "vInput1.rgb";
            case SHADER_INPUT_2:
                return with_alpha || !inputs_have_alpha ? "vInput2" : "vInput2.rgb";
            case SHADER_INPUT_3:
                return with_alpha || !inputs_have_alpha ? "vInput3" : "vInput3.rgb";
            case SHADER_INPUT_4:
                return with_alpha || !inputs_have_alpha ? "vInput4" : "vInput4.rgb";
            case SHADER_TEXEL0:
                return first_cycle ? (with_alpha ? "texVal0" : "texVal0.rgb")
                                   : (with_alpha ? "texVal1" : "texVal1.rgb");
            case SHADER_TEXEL0A:
                return first_cycle
                           ? (hint_single_element ? "texVal0.a"
                                                  : (with_alpha ? "vec4(texVal0.a, texVal0.a, texVal0.a, texVal0.a)"
                                                                : "vec3(texVal0.a, texVal0.a, texVal0.a)"))
                           : (hint_single_element ? "texVal1.a"
                                                  : (with_alpha ? "vec4(texVal1.a, texVal1.a, texVal1.a, texVal1.a)"
                                                                : "vec3(texVal1.a, texVal1.a, texVal1.a)"));
            case SHADER_TEXEL1A:
                return first_cycle
                           ? (hint_single_element ? "texVal1.a"
                                                  : (with_alpha ? "vec4(texVal1.a, texVal1.a, texVal1.a, texVal1.a)"
                                                                : "vec3(texVal1.a, texVal1.a, texVal1.a)"))
                           : (hint_single_element ? "texVal0.a"
                                                  : (with_alpha ? "vec4(texVal0.a, texVal0.a, texVal0.a, texVal0.a)"
                                                                : "vec3(texVal0.a, texVal0.a, texVal0.a)"));
            case SHADER_TEXEL1:
                return first_cycle ? (with_alpha ? "texVal1" : "texVal1.rgb")
                                   : (with_alpha ? "texVal0" : "texVal0.rgb");
            case SHADER_COMBINED:
                return with_alpha ? "texel" : "texel.rgb";
            case SHADER_NOISE:
                return with_alpha ? "vec4(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")"
                                  : "vec3(" RAND_NOISE ", " RAND_NOISE ", " RAND_NOISE ")";
        }
    } else {
        switch (item) {
            case SHADER_0:
                return "0.0";
            case SHADER_1:
                return "1.0";
            case SHADER_INPUT_1:
                return "vInput1.a";
            case SHADER_INPUT_2:
                return "vInput2.a";
            case SHADER_INPUT_3:
                return "vInput3.a";
            case SHADER_INPUT_4:
                return "vInput4.a";
            case SHADER_TEXEL0:
                return first_cycle ? "texVal0.a" : "texVal1.a";
            case SHADER_TEXEL0A:
                return first_cycle ? "texVal0.a" : "texVal1.a";
            case SHADER_TEXEL1A:
                return first_cycle ? "texVal1.a" : "texVal0.a";
            case SHADER_TEXEL1:
                return first_cycle ? "texVal1.a" : "texVal0.a";
            case SHADER_COMBINED:
                return "texel.a";
            case SHADER_NOISE:
                return RAND_NOISE;
        }
    }
    return "";
}

bool get_bool(prism::ContextTypes* value) {
    if (std::holds_alternative<int>(*value)) {
        return std::get<int>(*value) == 1;
    }
    return false;
}

prism::ContextTypes* append_formula(prism::ContextTypes* _, prism::ContextTypes* a_arg, prism::ContextTypes* a_single,
                                    prism::ContextTypes* a_mult, prism::ContextTypes* a_mix,
                                    prism::ContextTypes* a_with_alpha, prism::ContextTypes* a_only_alpha,
                                    prism::ContextTypes* a_alpha, prism::ContextTypes* a_first_cycle) {
    auto c = std::get<prism::MTDArray<int>>(*a_arg);
    bool do_single = get_bool(a_single);
    bool do_multiply = get_bool(a_mult);
    bool do_mix = get_bool(a_mix);
    bool with_alpha = get_bool(a_with_alpha);
    bool only_alpha = get_bool(a_only_alpha);
    bool opt_alpha = get_bool(a_alpha);
    bool first_cycle = get_bool(a_first_cycle);
    std::string out = "";
    if (do_single) {
        out += shader_item_to_str(c.at(only_alpha, 3), with_alpha, only_alpha, opt_alpha, first_cycle, false);
    } else if (do_multiply) {
        out += shader_item_to_str(c.at(only_alpha, 0), with_alpha, only_alpha, opt_alpha, first_cycle, false);
        out += " * ";
        out += shader_item_to_str(c.at(only_alpha, 2), with_alpha, only_alpha, opt_alpha, first_cycle, true);
    } else if (do_mix) {
        out += "mix(";
        out += shader_item_to_str(c.at(only_alpha, 1), with_alpha, only_alpha, opt_alpha, first_cycle, false);
        out += ", ";
        out += shader_item_to_str(c.at(only_alpha, 0), with_alpha, only_alpha, opt_alpha, first_cycle, false);
        out += ", ";
        out += shader_item_to_str(c.at(only_alpha, 2), with_alpha, only_alpha, opt_alpha, first_cycle, true);
        out += ")";
    } else {
        out += "(";
        out += shader_item_to_str(c.at(only_alpha, 0), with_alpha, only_alpha, opt_alpha, first_cycle, false);
        out += " - ";
        out += shader_item_to_str(c.at(only_alpha, 1), with_alpha, only_alpha, opt_alpha, first_cycle, false);
        out += ") * ";
        out += shader_item_to_str(c.at(only_alpha, 2), with_alpha, only_alpha, opt_alpha, first_cycle, true);
        out += " + ";
        out += shader_item_to_str(c.at(only_alpha, 3), with_alpha, only_alpha, opt_alpha, first_cycle, false);
    }
    return new prism::ContextTypes{ out };
}

std::optional<std::string> opengl_include_fs(const std::string& path) {
    auto init = std::make_shared<Ship::ResourceInitData>();
    init->Type = (uint32_t)Ship::ResourceType::Shader;
    init->ByteOrder = Ship::Endianness::Native;
    init->Format = RESOURCE_FORMAT_BINARY;
    auto res = std::static_pointer_cast<Ship::Shader>(
        Ship::Context::GetInstance()->GetResourceManager()->LoadResource(path, true, init));
    if (res == nullptr) {
        return std::nullopt;
    }
    auto inc = static_cast<std::string*>(res->GetRawPointer());
    return *inc;
}

std::string GfxRenderingAPIOGL::BuildFsShader(const CCFeatures& cc_features) {
    prism::Processor processor;
    prism::ContextItems mContext = {
        { "o_c", M_ARRAY(cc_features.c, int, 2, 2, 4) },
        { "o_alpha", cc_features.opt_alpha },
        { "o_fog", cc_features.opt_fog },
        { "o_texture_edge", cc_features.opt_texture_edge },
        { "o_noise", cc_features.opt_noise },
        { "o_2cyc", cc_features.opt_2cyc },
        { "o_alpha_threshold", cc_features.opt_alpha_threshold },
        { "o_invisible", cc_features.opt_invisible },
        { "o_grayscale", cc_features.opt_grayscale },
        { "o_textures", M_ARRAY(cc_features.usedTextures, bool, 2) },
        { "o_masks", M_ARRAY(cc_features.used_masks, bool, 2) },
        { "o_blend", M_ARRAY(cc_features.used_blend, bool, 2) },
        { "o_clamp", M_ARRAY(cc_features.clamp, bool, 2, 2) },
```


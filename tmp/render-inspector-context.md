# Context Pack
generated=2026-03-11T00:08:28.8965343-03:00
maxFiles=6 maxSnippetsPerFile=3 snippetLines=220

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

## file: docs/examples/external_mods/lighting_kit/debug/render_inspector.json
```text
{
  "schemaVersion": 1,
  "overlays": [
    {
      "id": "com.sylian.lighting_kit:render_debug",
      "enabledByDefault": false,
      "showMaterialUnderCursor": true,
      "showLightBudget": true,
      "showPostFxState": true
    }
  ]
}
```


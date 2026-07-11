# Context Pack
generated=2026-03-12T15:38:02.7602279-03:00
maxFiles=8 maxSnippetsPerFile=2 snippetLines=180

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
    "lastUpdatedUtc":  "2026-03-12T18:20:52Z",
    "stats":  {
                  "entryCount":  246,
                  "openCount":  90,
                  "doneCount":  155,
                  "deprecatedCount":  1,
                  "topicCount":  114,
                  "tagCount":  100
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
                 "mem-20260311-000025":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "done",
                                             "line":  199,
                                             "source":  "memory.log"
                                         },
                 "mem-20260311-000036":  {
                                             "topic":  "plan:PLN-20260303-0006",
                                             "status":  "done",
                                             "line":  210,
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
                 "mem-20260311-000020":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  194,
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
                 "mem-20260311-000012":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  186,
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
                 "mem-20260311-000063":  {
                                             "topic":  "volumetrics activation and ogl parity fix",
                                             "status":  "done",
                                             "line":  236,
                                             "source":  "memory.log"
                                         },
```
```text
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
                                             "source":  "memory.log"
                                         },
                 "mem-20260225-000029":  {
                                             "topic":  "external-mods:reticle-selected-in-hand-gate",
                                             "status":  "done",
                                             "line":  29,
                                             "source":  "memory.log"
                                         },
                 "mem-20260311-000057":  {
                                             "topic":  "plan:PLN-20260311-0019",
                                             "status":  "done",
                                             "line":  230,
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
                 "mem-20260311-000033":  {
                                             "topic":  "plan:PLN-20260303-0012",
                                             "status":  "done",
                                             "line":  207,
                                             "source":  "memory.log"
                                         },
                 "mem-20260310-000013":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  153,
                                             "source":  "memory.log"
                                         },
                 "mem-20260311-000040":  {
                                             "topic":  "plan:PLN-20260311-0011",
                                             "status":  "open",
                                             "line":  213,
                                             "source":  "memory.log"
                                         },
                 "mem-20260311-000017":  {
                                             "topic":  "plan:PLN-20260310-0002",
                                             "status":  "done",
                                             "line":  191,
                                             "source":  "memory.log"
                                         },
                 "mem-20260311-000024":  {
                                             "topic":  "plan:PLN-20260304-0002",
                                             "status":  "done",
                                             "line":  198,
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
                 "mem-20260311-000041":  {
                                             "topic":  "external-mods:multi-use-bottle-content",
                                             "status":  "done",
                                             "line":  214,
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
```


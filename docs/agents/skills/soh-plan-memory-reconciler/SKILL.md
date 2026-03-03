---
name: soh-plan-memory-reconciler
description: Reconcile Plans.md and memory index/log states with append-only corrective guidance. Use when plan statuses and memory topics drift or when closing plan batches.
---

# SOH Plan-Memory Reconciler

1. Run reconciliation report.
- Use `scripts/reconcile-plan-memory.ps1`.
- It compares final plan status with `plan:*` memory entries.

2. Fix append-only.
- Prefer corrective memory entries with `supersedes`.
- Never edit historical lines in `memory.log`.

3. Rebuild and validate.
- Run `tools/agents/rebuild-index.ps1` and `tools/agents/validate-memory.ps1`.

## Example (PT-BR)
- "Plano esta done no ledger, mas memoria ainda open; gerar correcao append-only."

## Reference
- Read `references/reconcile-policy.md`.

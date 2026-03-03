---
name: soh-contract-delta-summarizer
description: Summarize contract deltas between git refs for runtime docs/registries. Use when preparing release notes, plan updates, or drift triage after parser/runtime changes.
---

# SOH Contract Delta Summarizer

1. Compare two refs.
- Run `scripts/summarize-contract-delta.ps1 -FromRef <A> -ToRef <B>`.
- Focus on actions/events/catalogs and runtime registries.

2. Use structured output.
- Read Added/Removed sections first.
- Use impact notes for validation checklist.

3. Keep planning concise.
- Paste summary into `Plans.md` updates and memory decisions.

## Example (PT-BR)
- "Quero saber o que mudou no contrato entre o ultimo commit estavel e HEAD."

## Reference
- Read `references/delta-rules.md`.

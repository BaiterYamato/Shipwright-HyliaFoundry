---
name: soh-docs-index-navigator
description: Build and query a compact docs index for fast file/section discovery with minimal token cost. Use when you need quick path+heading lookup before opening docs.
---

# SOH Docs Index Navigator

1. Build the index.
- Run `scripts/build-docs-index.ps1`.
- It scans docs markdown/json and records headings/contracts.

2. Query with intent.
- Run `scripts/query-docs-index.ps1 -Query <term>`.
- Prefer contract or capability filters for precise results.

3. Keep retrieval compact.
- Open only top matches first.
- Expand scope only if top matches are insufficient.

## Example (PT-BR)
- "Quero achar tudo sobre `camera.aim_profiles.v2` sem abrir a pasta docs inteira."

## Reference
- Read `references/docs-index-schema.md`.

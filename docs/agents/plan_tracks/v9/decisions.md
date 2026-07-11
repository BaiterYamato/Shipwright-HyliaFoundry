# v9 Decisions

- Delivery order is **Core first, Reference Suite second**.
- Baseline remains `apiVersion: 4`; v9 is capability-gated expansion.
- Persistence backend uses external JSON by domain (separate from vanilla save sections).
- Example source of truth remains `docs/examples/external_mods`.
- Contract/runtime failures must be isolated per mod.

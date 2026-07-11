# v10 Decisions

- Modloader remains thin; heavy systems stay in framework mods/tools.
- Baseline remains `apiVersion: 4` with capability-gated expansion.
- Packaging is deterministic by default (stable ordering/timestamps/hashes).
- Packs are assets-only (`.otr/.o2r`) and cannot include runtime logic/dependencies.
- Non-stable release channels are buildable in tooling but gated in modloader UI until v1.0.

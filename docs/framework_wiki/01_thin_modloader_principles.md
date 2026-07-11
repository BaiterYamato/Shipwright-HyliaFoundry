# 01 - Thin Modloader Principles

1. Keep the engine-facing runtime thin and capability-gated.
2. Push reusable gameplay/data surfaces into framework mods, not one-off demos.
3. Resolve composition through `dependencies[]`, `uses[]`, `provides[]`, and `exports{}` instead of hardcoded mod names.
4. Treat native code as an explicit capability boundary with trust, diagnostics, and lockfiles.
5. Let content packs stay small: they should consume frameworks, not recreate them.
6. Preserve vanilla behavior when a framework or content pack is absent.

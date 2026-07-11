# 04 - Migrations and Budgets

## Migration Notes

- The canonical catalog is breaking by design. Old kit/demo names are archived, not preserved as wrappers.
- The archive path is `docs/examples_legacy/external_mods_20260407`.
- When migrating an old package, map it to one of the six frameworks first, then move any remaining feature-specific payload into a consumer demo or reference pack.

## Old to New Examples

- Visual/world kits -> `framework_render_world`
- Narrative kits -> `framework_narrative`
- Persistence kits -> `framework_persistence`
- UI/container kits -> `framework_ui_inventory`
- AI/status/spell kits -> `framework_combat_ai`
- Devtools/native/hybrid kits -> `framework_devtools_native`

## Runtime Budgets

- Keep framework budgets conservative because they are always loaded before their consumers.
- Prefer data/json for static content and keep native code for SDK access, diagnostics, and host-side integration only.
- Use `doctor` to confirm that every expected capability is represented somewhere in the active catalog before shipping a new suite revision.

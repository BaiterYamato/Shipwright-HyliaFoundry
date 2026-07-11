# HOT_RELOAD_GUIDE

Patch v9 dev iteration supports hot reload requests and watcher definitions.

## Capabilities
- `dev.hot_reload.v1`
- `dev.console.v1`
- `dev.watchers.v1`
- `wasm.sandbox.v2`

## Core actions
- `dev.reloadAll`
- `dev.reloadTarget`
- `dev.console.exec`

## Hooks
- `onHotReloadApplied`
- `onHotReloadFailed`
- `onSandboxBudgetExceeded`
- `onSandboxPermissionDenied`

See also:
- `docs/examples/external_mods/devtools_kit`
- `docs/examples/external_mods/demo_hot_reload_playground`

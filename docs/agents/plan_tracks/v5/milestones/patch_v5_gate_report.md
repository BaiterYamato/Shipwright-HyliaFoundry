# Patch v5 Gate Report (for v6 rollout)

Date: 2026-03-03
Plan: PLN-20260303-0004

## Checklist

- API baseline confirmed at v4 (`kExternalModApiVersionMin/Max = 4`).
- v5 capabilities confirmed in runtime and exports:
  - `fx.presets.v1`
  - `states.catalog.v1`
  - `spells.catalog.v1`
- v5 actions confirmed in parser/runtime and exported registries:
  - `fx.spawnEffectSs`, `fx.spawnActorFx`, `fx.spawnPreset`, `fx.stopFx`
  - `states.applyState`, `states.clearState`, `states.hasState`
  - `player.getStateFlags`, `player.setStateFlag`, `player.clearStateFlag`,
    `player.setControlLock`, `player.setGravityScale`, `player.setBoostType`, `player.setDamageResponse`
  - `spells.castSpell`
- Dependency resolver baseline confirmed (id + versionRange checks, topological ordering, isolated failure).

## Result

Gate status: **PASS**.

v6 implementation was allowed to proceed after this gate.

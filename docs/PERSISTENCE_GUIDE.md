# PERSISTENCE_GUIDE

Patch v9 persistence is capability-gated (`apiVersion: 4`) and uses external JSON domains under:

`<save_root>/external_mods/persist/<worldSlotId>/<modId>/`

## Required capabilities
- `world.persistence.v1`
- `world.storage.v1`

Optional companions:
- `world.spawn_profiles.v1`
- `world.seeding.v1`
- `world.migrations.v1`

## Core actions
- `persist.ensureEntityGuid`
- `persist.saveEntityState`
- `persist.loadEntityState`
- `persist.deleteEntityState`
- `persist.setDomainValue`
- `persist.getDomainValue`
- `persist.runMigrations`

See also:
- `docs/EXTERNAL_MOD_DATA_DRIVEN_REFERENCE.md`
- `docs/examples/external_mods/persistence_kit`

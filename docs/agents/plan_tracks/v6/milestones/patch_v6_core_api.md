# Patch v6 Core API Milestone

Plan: PLN-20260303-0004
Track: v6

## Scope

Core API/runtime contract additions for:
- UI runtime/hud/inventory ext
- containers + processing recipes
- interactions
- actor archetypes/adapters
- AI behavior trees + sensors
- nav routes + nav bridge
- debug overlays

## Delivered in this milestone

1. Added v6 capabilities to runtime-supported set.
2. Added capability-gated manifest fields:
   - `uiScreenDefinitions`, `uiHudDefinitions`, `inventoryExtensionDefinitions`
   - `containerDefinitions`, `recipeDefinitions`, `interactionDefinitions`
   - `actorArchetypeDefinitions`, `actorAdapterDefinitions`
   - `behaviorTreeDefinitions`, `sensorDefinitions`
   - `routeDefinitions`, `navBridgeDefinitions`, `debugOverlayDefinitions`
3. Added parser action support for v6 public actions (`ui.*`, `inventoryExt.*`, `container.*`, `actors.*`, `interactions.invoke`, `ai.*`, `sense.*`, `nav.*`, `debug.*`).
4. Added runtime parser/loaders for new v6 definition files.
5. Added v6 hooks and alias support:
   - `onUiScreenOpened`, `onUiScreenClosed`, `onUiAction`
   - `onContainerSlotChanged`, `onProcessStart`, `onProcessTick`, `onProcessComplete`
   - `onArchetypeSpawn`, `onArchetypeDespawn`, `onInteraction`
   - `onBehaviorNodeChanged`, `onPathRequested`, `onPathFailed`
6. Added runtime state containers for open UI screens, inventory ext pages, container processes, nav path handles, debug overlay visibility.
7. Added process ticking and completion hook dispatch in `OnGameFrameUpdate`.
8. Added cleanup/reset for new runtime states in lifecycle paths (`Shutdown`, `DisableRuntime`, `OnLoadGame`, `OnSceneInit`, `OnPlayDestroy`).

## Validation

- Release build executed successfully (`soh.exe` produced).
- Runtime reference export regenerated after contract changes.

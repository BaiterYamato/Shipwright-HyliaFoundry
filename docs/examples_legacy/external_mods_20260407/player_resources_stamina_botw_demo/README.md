# Player Resources Stamina BOTW Demo

Targets the planned API v4 player-resource contract on this branch.

Demonstrates:
- `player.resources.v1` via `player/player_resources.json`
- `ui.resource_rings.v1` via `ui/resource_rings.json`
- save-scoped settings for enable/drain/regen plus ring visibility, scale, and opacity
- `input.bindings.v2` sprint hold map (`MOD_ACTION6` -> `Left Shift`)
- `hooks.extended.v1` notifications for depletion, recovery, and capacity changes

Notes:
- The demo wires the generic gameplay bridge tags used by the player framework:
  - `roll.start`
  - `shield.hold`
  - `climb.hold`
  - `hang.hold`
  - `swim.hold`
  - `sprint.hold`
- Capacity persists through `storageDomainId`; current stamina refills on scene enter in this demo.
- Use `setResourceCapacity` or `refillResource` from hooks/behaviors to simulate wheel upgrades, shrine rewards, or stamina pickups.

# Player Survival Needs Demo

Framework-first survival example for API v4:

- **Hunger** resource (orange bar)
- **Thirst** resource (blue bar)
- both stack **below** the yellow stamina bar from `player_resources_stamina_botw_demo`
- **Odd Mushroom** food is a custom stack consumable granted by **cutting tall grass**
- **Fresh Water** is a custom bottle content filled with an **empty bottle while inside water**
- bottle visuals and drink animation use **Lon Lon Milk placeholders**

## Defaults

- Hunger drains slowly and, when empty, blocks sprint, reduces move speed, and weakens stamina regen.
- Thirst drains faster and, when empty, blocks sprint, reduces move speed further, weakens stamina regen more, and deals periodic damage.
- Food hotkey defaults to **G** and is user-remappable.

## Notes

- Odd Mushroom keeps a **stack of 20** in mod-save but also exposes itself through the **vanilla Odd Mushroom inventory slot** when that slot is free, so it can be consumed from the inventory/C-buttons with the drink-demo animation.
- Fresh water behaves like **Lon Lon Milk** with **2 uses** per fill: `fresh_water_full.png` and `fresh_water_half.png` are shipped under `ui/assets/` and override the milk placeholder icon shown on the equipped **C-slot** for the full/half bottle states.
- The survival Odd Mushroom remains a custom consumable; it does not replace the actual adult trade quest item logic.
- The new stacked HUD is generic, so other mods can reuse the same `magic_bar_stack` layout.

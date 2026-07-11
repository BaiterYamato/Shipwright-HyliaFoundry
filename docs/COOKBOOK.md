# COOKBOOK

Quick v9 recipes:

1. Persistent entity marker
   - `persist.ensureEntityGuid`
   - `persist.saveEntityState`

2. Branching dialogue + flag
   - `narrative.startDialogue`
   - option `onChoose` -> `narrative.setFlag`

3. Intro timeline
   - `narrative.startTimeline`
   - timeline tracks with `showNotification`/FX actions

4. Dev reload loop
   - `dev.console.exec` command triggers `dev.reloadTarget`

Reference examples are under `docs/examples/external_mods/` (`persistence_kit`, `narrative_kit`, `devtools_kit` and demos).

# 02 - Framework Dependency Pattern

## Required Shape

- Frameworks use `type = framework`.
- Frameworks must declare `provides[]`, `uses[]`, `exports{}`, `releaseChannels[]`, and `manifest.lock.json`.
- Content packs use `type = content`.
- Content packs depend on frameworks with objects in `dependencies[]`:

```json
{
  "id": "com.sylian.framework.render_world",
  "versionRange": ">=0.1.0 <0.2.0"
}
```

- `dependencies[].modId` is legacy and should not be used.
- `uses[]` is the human-readable contract list that explains which framework surfaces a content pack expects.

## Canonical Families

- `render_world`
- `narrative`
- `persistence`
- `ui_inventory`
- `combat_ai`
- `devtools_native`

## Lockfiles

- Stable packages keep `manifest.lock.json` checked into source.
- The lockfile binds the mod id, version, release channel, and resolved dependencies used for packaging.

## Validation Flow

1. `modtool validate`
2. `modtool doctor`
3. `modtool report-native` for any `hybrid-v1` or `native-cpp-v1` package

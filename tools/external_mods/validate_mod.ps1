[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$ModPath = "",
    [switch]$DryRun,
    [switch]$VerboseOutput
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Fail([string]$Message) {
    Write-Error $Message
    exit 1
}

function Resolve-CapabilityPathMap {
    return @{
        "hooks.extended.v1" = "hookDefinitions"
        "behaviors.graph.v1" = "behaviorDefinitions"
        "actors.vm.v1" = "actorDefinitions"
        "actors.generic.v1" = "actorDefinitions"
        "scenes.bundle.v1" = "sceneDefinitions"
        "statuses.catalog.v1" = "statusDefinitions"
        "combat.damage.v1" = "damageDefinitions"
        "combat.targeting.v1" = "targetingDefinitions"
        "combat.projectiles.v1" = "projectileDefinitions"
        "combat.aoe.v1" = "aoeDefinitions"
        "movement.profiles.v1" = "movementDefinitions"
        "items.use_profiles.v1" = "itemUseProfiles"
        "patches.vanilla_items.v1" = "vanillaItemPatches"
        "camera.aim_profiles.v2" = "cameraDefinitions"
        "input.bindings.v2" = "inputDefinitions"
        "ui.runtime.v1" = "uiScreenDefinitions"
        "ui.hud.v1" = "uiHudDefinitions"
        "player.resources.v1" = "playerResourceDefinitions"
        "ui.resource_rings.v1" = "resourceRingDefinitions"
        "player.consumables.v1" = "playerConsumableDefinitions"
        "ui.inventory_ext.v1" = "inventoryExtensionDefinitions"
        "containers.v1" = "containerDefinitions"
        "recipes.processing.v1" = "recipeDefinitions"
        "interactions.v1" = "interactionDefinitions"
        "actors.archetypes.v1" = "actorArchetypeDefinitions"
        "actors.adapters.v1" = "actorAdapterDefinitions"
        "ai.behavior_trees.v1" = "behaviorTreeDefinitions"
        "ai.sensors.v1" = "sensorDefinitions"
        "nav.routes.v1" = "routeDefinitions"
        "nav.navmesh_bridge.v1" = "navBridgeDefinitions"
        "debug.overlay.v1" = "debugOverlayDefinitions"
        "items.state_machine.v1" = "itemStateDefinitions"
        "render.equipped_models.v1" = "equippedModelDefinitions"
        "hud.widgets.v1" = "hudWidgetDefinitions"
        "hud.reticles.v2" = "hudReticleDefinitions"
        "effects.graph.v2" = "effectGraphDefinitions"
        "combat.hit_rules.v2" = "combatHitRuleDefinitions"
        "movement.surf.v2" = "surfDefinitions"
        "actors.tags.v1" = "actorTagDefinitions"
        "world.patchsets.v1" = "worldPatchDefinitions"
        "quests.graph.v1" = "questDefinitions"
        "dialog.nodes.v1" = "dialogDefinitions"
        "sdk.generators.v1" = "sdkGeneratorDefinitions"
        "fx.presets.v1" = "fxPresetDefinitions"
        "states.catalog.v1" = "stateDefinitions"
        "spells.catalog.v1" = "spellDefinitions"
        "render.materials.v1" = "materialDefinitions"
        "render.pbr.v1" = "pbrDefinitions"
        "render.lighting.v1" = "lightingDefinitions"
        "render.postfx.v1" = "postFxDefinitions"
        "world.scenes.v1" = "sceneProfileDefinitions"
        "world.rooms.v1" = "roomProfileDefinitions"
        "assets.packs.v2" = "assetPackDefinitions"
        "assets.raw.v1" = "assetSourceDefinitions"
        "render.meshes.v1" = "meshDefinitions"
        "actors.prefabs.v1" = "prefabDefinitions"
        "world.instances.v1" = "worldInstanceDefinitions"
        "debug.render_inspector.v1" = "renderInspectorDefinitions"
        "editor.runtime.v1" = "editorRuntimeDefinitions"
        "editor.ui.v1" = "editorUiDefinitions"
        "editor.selection.v1" = "editorSelectionDefinitions"
        "editor.gizmos.v1" = "editorGizmoDefinitions"
        "editor.library.v1" = "editorLibraryDefinitions"
        "editor.projects.v1" = "editorProjectDefinitions"
        "editor.placement.v1" = "editorPlacementDefinitions"
        "assets.importer.v1" = "assetImporterDefinitions"
        "world.authoring.v1" = "worldAuthoringDefinitions"
        "world.collision_authoring.v1" = "collisionAuthoringDefinitions"
        "debug.editor_inspector.v1" = "editorInspectorDefinitions"
        "world.persistence.v1" = "worldPersistenceDefinitions"
        "world.storage.v1" = "worldStorageDefinitions"
        "world.spawn_profiles.v1" = "worldSpawnProfileDefinitions"
        "world.forage.v1" = "worldForageDefinitions"
        "world.time_weather.v1" = "worldTimeWeatherDefinitions"
        "world.seeding.v1" = "worldSeedingDefinitions"
        "world.migrations.v1" = "worldMigrationDefinitions"
        "debug.persistence_inspector.v1" = "persistenceInspectorDefinitions"
        "narrative.timeline.v1" = "narrativeTimelineDefinitions"
        "narrative.dialogue.v1" = "narrativeDialogueDefinitions"
        "narrative.quests.v1" = "narrativeQuestDefinitions"
        "narrative.flags.v1" = "narrativeFlagDefinitions"
        "debug.narrative_inspector.v1" = "narrativeInspectorDefinitions"
        "dev.hot_reload.v1" = "devHotReloadDefinitions"
        "dev.console.v1" = "devConsoleDefinitions"
        "dev.watchers.v1" = "devWatcherDefinitions"
        "wasm.sandbox.v2" = "wasmSandboxDefinitions"
        "debug.reload_inspector.v1" = "reloadInspectorDefinitions"
    }
}

function Resolve-SupportedRuntimePermissions {
    return @(
        "filesystem",
        "network",
        "process",
        "nativeinterop",
        "nativeinterop.raw",
        "engine.memory",
        "editor.livelink"
    )
}

function Resolve-ConventionalCapabilityFileMap {
    return @{
        "items.data.v2" = "items/items.json"
        "items.catalog.v1" = "items/items.json"
        "input.bindings.v2" = "config/input.json"
        "hooks.extended.v1" = "config/hooks.json"
        "actors.vm.v1" = "actors/actors.json"
        "actors.generic.v1" = "actors/actors.json"
        "behaviors.graph.v1" = "behaviors/behaviors.json"
        "scenes.bundle.v1" = "scenes/scenes.json"
        "statuses.catalog.v1" = "combat/statuses.json"
        "combat.damage.v1" = "combat/damage.json"
        "combat.targeting.v1" = "combat/targeting.json"
        "combat.projectiles.v1" = "combat/projectiles.json"
        "combat.aoe.v1" = "combat/aoe.json"
        "movement.profiles.v1" = "movement/profiles.json"
        "items.use_profiles.v1" = "items/use_profiles.json"
        "patches.vanilla_items.v1" = "patches/vanilla_items.json"
        "camera.aim_profiles.v2" = "camera/aim_profiles.json"
        "ui.runtime.v1" = "ui/screens.json"
        "ui.hud.v1" = "ui/hud.json"
        "player.resources.v1" = "player/resources.json"
        "ui.resource_rings.v1" = "ui/resource_rings.json"
        "player.consumables.v1" = "player/consumables.json"
        "ui.inventory_ext.v1" = "ui/inventory_extension.json"
        "containers.v1" = "gameplay/containers.json"
        "recipes.processing.v1" = "gameplay/recipes.json"
        "interactions.v1" = "gameplay/interactions.json"
        "actors.archetypes.v1" = "actors/archetypes.json"
        "actors.adapters.v1" = "actors/adapters.json"
        "ai.behavior_trees.v1" = "graphs/behavior_trees.json"
        "ai.sensors.v1" = "actors/sensors.json"
        "nav.routes.v1" = "world/routes.json"
        "nav.navmesh_bridge.v1" = "world/navmesh_bridge.json"
        "render.materials.v1" = "render/materials.json"
        "render.pbr.v1" = "render/pbr.json"
        "render.lighting.v1" = "render/lighting.json"
        "render.postfx.v1" = "render/postfx.json"
        "assets.packs.v2" = "assets/packs.json"
        "assets.raw.v1" = "assets/sources.json"
        "render.meshes.v1" = "render/meshes.json"
        "actors.prefabs.v1" = "prefabs/prefabs.json"
        "world.instances.v1" = "world/instances.json"
        "editor.placement.v1" = "world/placement.json"
        "world.authoring.v1" = "world/authoring.json"
        "world.persistence.v1" = "world/persistence.json"
        "world.storage.v1" = "world/storage.json"
        "world.spawn_profiles.v1" = "world/spawn_profiles.json"
        "world.forage.v1" = "world/forage.json"
        "world.time_weather.v1" = "world/time_weather.json"
        "world.seeding.v1" = "world/seeding.json"
        "world.migrations.v1" = "world/migrations.json"
        "narrative.flags.v1" = "narrative/flags.json"
        "narrative.dialogue.v1" = "narrative/dialogue.json"
        "narrative.quests.v1" = "narrative/quests.json"
        "narrative.timeline.v1" = "narrative/timelines.json"
        "dev.hot_reload.v1" = "dev/hot_reload.json"
        "dev.console.v1" = "dev/console.json"
        "dev.watchers.v1" = "dev/watchers.json"
    }
}

function Get-ManifestSchemaVersion {
    param([object]$Manifest)
    if ($null -ne $Manifest -and ($Manifest.PSObject.Properties.Name -contains "schemaVersion")) {
        return "$($Manifest.schemaVersion)".ToLowerInvariant()
    }
    return "mod.manifest.v2"
}

function Get-ModManifestPath {
    param([string]$ModDir)
    $rootManifest = Join-Path $ModDir "mod.json"
    if (Test-Path $rootManifest) { return $rootManifest }
    $splitManifest = Join-Path $ModDir "data/mod.json"
    if (Test-Path $splitManifest) { return $splitManifest }
    return ""
}

function Get-ModDataRoot {
    param(
        [string]$ModDir,
        [string]$ManifestPath
    )

    if ([string]::IsNullOrWhiteSpace($ManifestPath)) { return $ModDir }
    return Split-Path -Path $ManifestPath -Parent
}

function Get-ModRuntimeRoot {
    param(
        [string]$ModDir,
        [string]$DataRoot
    )

    $embeddedRuntime = Join-Path $ModDir "runtime"
    if (Test-Path $embeddedRuntime) {
        return $embeddedRuntime
    }

    $dataParent = Split-Path -Path $DataRoot -Parent
    if ((Split-Path -Path $dataParent -Leaf).ToLowerInvariant() -eq "mods_data") {
        $runtimeSibling = Join-Path (Join-Path (Split-Path -Path $dataParent -Parent) "mods_runtime") (Split-Path -Path $DataRoot -Leaf)
        if (Test-Path $runtimeSibling) {
            return $runtimeSibling
        }
    }

    return $ModDir
}

function Resolve-ModFilePath {
    param(
        [string]$RelativePath,
        [string]$DataRoot,
        [string]$RuntimeRoot
    )

    if ([string]::IsNullOrWhiteSpace($RelativePath)) {
        return ""
    }

    $normalized = $RelativePath.Replace('\', '/')
    $firstSegment = $normalized.Split('/')[0].ToLowerInvariant()
    $extension = [System.IO.Path]::GetExtension($normalized).ToLowerInvariant()
    $useRuntimeRoot = $false
    if (@("scripts", "native") -contains $firstSegment) {
        $useRuntimeRoot = $true
    } elseif ($firstSegment -eq "assets" -and $extension -ne ".json") {
        $useRuntimeRoot = $true
    }
    $candidates = @()
    if ($useRuntimeRoot -and -not [string]::IsNullOrWhiteSpace($RuntimeRoot)) {
        $candidates += (Join-Path $RuntimeRoot $normalized)
    }
    if (-not [string]::IsNullOrWhiteSpace($DataRoot)) {
        $candidates += (Join-Path $DataRoot $normalized)
    }
    if (-not [string]::IsNullOrWhiteSpace($RuntimeRoot)) {
        $candidates += (Join-Path $RuntimeRoot $normalized)
    }

    foreach ($candidate in $candidates | Select-Object -Unique) {
        if (Test-Path $candidate) {
            return $candidate
        }
    }

    if ($candidates.Count -gt 0) {
        return $candidates[0]
    }

    return Join-Path $DataRoot $normalized
}

function Get-ManifestReleaseChannels {
    param([object]$Manifest)

    $channels = @()
    if ($null -ne $Manifest -and ($Manifest.PSObject.Properties.Name -contains "releaseChannels")) {
        foreach ($channel in @($Manifest.releaseChannels)) {
            if ($null -ne $channel -and -not [string]::IsNullOrWhiteSpace("$channel")) {
                $channels += "$channel".ToLowerInvariant()
            }
        }
    }

    if ($channels.Count -eq 0) {
        return @("stable")
    }

    return @($channels | Select-Object -Unique)
}

function Test-ManifestLockfile {
    param(
        [object]$Report,
        [object]$Manifest,
        [string]$ModDir,
        [string[]]$ReleaseChannels
    )

    if ($ReleaseChannels -notcontains "stable") {
        return
    }

    $lockPath = Join-Path $ModDir "manifest.lock.json"
    if (-not (Test-Path $lockPath)) {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_LOCKFILE_MISSING" `
            -Message "stable mods must include manifest.lock.json" -SourcePath "manifest.lock.json" `
            -SuggestedFix "Generate a lockfile with modtool pack/report or add it manually."
        return
    }

    $lockJson = Test-JsonFile -AbsolutePath $lockPath -DisplayPath "manifest.lock.json" -Report $Report -CodePrefix "V52_FATAL_LOCKFILE"
    if ($null -eq $lockJson) {
        return
    }

    if (-not ($lockJson.PSObject.Properties.Name -contains "schemaVersion") -or "$($lockJson.schemaVersion)" -ne "mod.lockfile.v1") {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_LOCKFILE_SCHEMA" `
            -Message "manifest.lock.json must use schemaVersion mod.lockfile.v1" -SourcePath "manifest.lock.json::schemaVersion"
    }
    if (-not ($lockJson.PSObject.Properties.Name -contains "modId") -or "$($lockJson.modId)" -ne "$($Manifest.id)") {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_LOCKFILE_MODID" `
            -Message "manifest.lock.json modId must match mod.json id" -SourcePath "manifest.lock.json::modId"
    }
    if (-not ($lockJson.PSObject.Properties.Name -contains "version") -or "$($lockJson.version)" -ne "$($Manifest.version)") {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_LOCKFILE_VERSION" `
            -Message "manifest.lock.json version must match mod.json version" -SourcePath "manifest.lock.json::version"
    }
}

function Test-VersionRangeSyntax {
    param(
        [Parameter(Mandatory = $true)]
        [string]$Range
    )

    if ([string]::IsNullOrWhiteSpace($Range)) {
        return $true
    }

    $tokens = $Range.Split(" ", [System.StringSplitOptions]::RemoveEmptyEntries)
    foreach ($token in $tokens) {
        if ($token -notmatch '^(>=|>|<=|<)?\d+\.\d+\.\d+$') {
            return $false
        }
    }

    return $true
}

function New-SeveritySummary {
    return [ordered]@{
        info = 0
        warn = 0
        error = 0
        fatal = 0
    }
}

function New-ModReport {
    param(
        [string]$ModId,
        [string]$ModDir
    )

    return [pscustomobject][ordered]@{
        modId = $ModId
        path = $ModDir
        severitySummary = New-SeveritySummary
        issues = @()
    }
}

function Write-IssueLine {
    param(
        [string]$ModId,
        [string]$Severity,
        [string]$Code,
        [string]$Message
    )

    $prefix = "[validate_mod][$Severity][$ModId][$Code]"
    switch ($Severity) {
        "FATAL" { Write-Host "$prefix $Message" -ForegroundColor Red }
        "ERROR" { Write-Host "$prefix $Message" -ForegroundColor Yellow }
        "WARN" { Write-Host "$prefix $Message" -ForegroundColor DarkYellow }
        default { Write-Host "$prefix $Message" -ForegroundColor DarkGray }
    }
}

function Add-ModIssue {
    param(
        [object]$Report,
        [string]$Severity,
        [string]$Code,
        [string]$Message,
        [string]$SourcePath = "",
        [string]$RegistryId = "",
        [string]$ReferencedId = "",
        [string]$SuggestedFix = ""
    )

    $severityKey = $Severity.ToLowerInvariant()
    if (-not $Report.severitySummary.Contains($severityKey)) {
        $severityKey = "info"
        $Severity = "INFO"
    }

    $Report.severitySummary[$severityKey] = [int]$Report.severitySummary[$severityKey] + 1
    $issue = [pscustomobject][ordered]@{
        severity = $Severity
        code = $Code
        message = $Message
        sourcePath = $SourcePath
        registryId = $RegistryId
        referencedId = $ReferencedId
        suggestedFix = $SuggestedFix
    }
    $Report.issues = @($Report.issues) + $issue
    Write-IssueLine -ModId $Report.modId -Severity $Severity -Code $Code -Message $Message
}

function Test-JsonFile {
    param(
        [string]$AbsolutePath,
        [string]$DisplayPath,
        [object]$Report,
        [string]$CodePrefix,
        [string]$RegistryId = ""
    )

    try {
        return (Get-Content -Path $AbsolutePath -Raw | ConvertFrom-Json)
    } catch {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "${CodePrefix}_INVALID_JSON" `
            -Message "invalid json in ${DisplayPath}: $($_.Exception.Message)" -SourcePath $DisplayPath -RegistryId $RegistryId
        return $null
    }
}

function Get-InputBindingIds {
    param([object]$InputJson)

    $bindingIds = @()
    if ($null -eq $InputJson -or -not ($InputJson.PSObject.Properties.Name -contains "bindings")) {
        return $bindingIds
    }

    $bindings = $InputJson.bindings
    if ($bindings -is [string] -or $bindings -isnot [System.Collections.IEnumerable]) {
        return $bindingIds
    }

    foreach ($binding in $bindings) {
        if ($null -eq $binding -or $binding.PSObject -eq $null) {
            continue
        }
        if ($binding.PSObject.Properties.Name -contains "id") {
            $bindingId = "$($binding.id)".Trim()
            if (-not [string]::IsNullOrWhiteSpace($bindingId)) {
                $bindingIds += $bindingId
            }
        }
    }

    return @($bindingIds | Select-Object -Unique)
}

function Add-OnInputBindingReferenceIssues {
    param(
        [object]$Report,
        [object]$EntryJson,
        [string[]]$KnownBindingIds,
        [string]$EntryScriptPath
    )

    if ($null -eq $EntryJson -or -not ($EntryJson.PSObject.Properties.Name -contains "onInput")) {
        return
    }

    $onInput = $EntryJson.onInput
    if ($onInput -is [string] -or $onInput -isnot [System.Collections.IEnumerable]) {
        return
    }

    $index = 0
    foreach ($inputTrigger in $onInput) {
        if ($null -eq $inputTrigger -or $inputTrigger.PSObject -eq $null) {
            $index++
            continue
        }

        $bindingId = ""
        if ($inputTrigger.PSObject.Properties.Name -contains "binding") {
            $bindingId = "$($inputTrigger.binding)".Trim()
        } elseif ($inputTrigger.PSObject.Properties.Name -contains "bindingId") {
            $bindingId = "$($inputTrigger.bindingId)".Trim()
        }

        if (-not [string]::IsNullOrWhiteSpace($bindingId) -and ($KnownBindingIds -notcontains $bindingId)) {
            Add-ModIssue -Report $Report -Severity "ERROR" -Code "V52_REF_UNKNOWN_INPUT_BINDING" `
                -Message "onInput references binding '$bindingId' that is not available in the current validation scope" `
                -SourcePath "${EntryScriptPath}::onInput[$index].binding" -RegistryId "core.input.v1" -ReferencedId $bindingId `
                -SuggestedFix "Add the binding to inputDefinitions or validate together with the dependency that provides it."
        }

        $index++
    }
}

function Load-ManifestJsonFieldIfPresent {
    param(
        [object]$Report,
        [object]$Manifest,
        [string]$ModDir,
        [string]$FieldName,
        [string]$DataRoot = "",
        [string]$RuntimeRoot = ""
    )

    if (-not ($Manifest.PSObject.Properties.Name -contains $FieldName)) {
        return $null
    }

    $relativePath = "$($Manifest.$FieldName)"
    if ([string]::IsNullOrWhiteSpace($relativePath)) {
        return $null
    }

    $absolutePath = Resolve-ModFilePath -RelativePath $relativePath -DataRoot $DataRoot -RuntimeRoot $RuntimeRoot
    if (-not (Test-Path $absolutePath)) {
        return $null
    }

    return Test-JsonFile -AbsolutePath $absolutePath -DisplayPath $relativePath -Report $Report -CodePrefix "V52_FATAL_LOAD" 
}

function Get-IdsFromCatalog {
    param(
        [object]$Json,
        [string]$CollectionProperty
    )

    $ids = @()
    if ($null -eq $Json -or -not ($Json.PSObject.Properties.Name -contains $CollectionProperty)) {
        return $ids
    }

    $collection = $Json.$CollectionProperty
    if ($collection -is [string] -or $collection -isnot [System.Collections.IEnumerable]) {
        return $ids
    }

    foreach ($entry in $collection) {
        if ($null -eq $entry -or $entry.PSObject -eq $null) {
            continue
        }
        if ($entry.PSObject.Properties.Name -contains "id") {
            $id = "$($entry.id)".Trim()
            if (-not [string]::IsNullOrWhiteSpace($id)) {
                $ids += $id
            }
        }
    }

    return @($ids | Select-Object -Unique)
}

function Get-JsonFilesForReferenceScan {
    param([string]$ModDir)

    return @(Get-ChildItem -Path $ModDir -Recurse -File -Filter *.json |
        Where-Object { $_.Name -ne "manifest.lock.json" })
}

function Get-RelativePathCompat {
    param(
        [string]$BasePath,
        [string]$TargetPath
    )

    try {
        $baseResolved = (Resolve-Path -Path $BasePath).Path
        $targetResolved = (Resolve-Path -Path $TargetPath).Path
        $baseUri = [System.Uri](($baseResolved.TrimEnd('\', '/')) + [System.IO.Path]::DirectorySeparatorChar)
        $targetUri = [System.Uri]$targetResolved
        return [System.Uri]::UnescapeDataString($baseUri.MakeRelativeUri($targetUri).ToString()).Replace('/', '/')
    } catch {
        return $TargetPath
    }
}

function Test-IsBuiltinStatusReference {
    param([string]$Value)

    $normalized = "$Value".Trim().ToLowerInvariant()
    return @(
        "core:burning", "core:fire", "fire",
        "core:freeze", "freeze", "core:frozen", "core:freeze_ice_trap_no_damage",
        "core:stun", "stun",
        "core:poison", "poison",
        "core:blind", "blind",
        "core:speed", "speed",
        "core:slow", "slow",
        "core:high_jump", "highjump", "high_jump",
        "core:strength", "strength",
        "core:weakness", "weakness"
    ) -contains $normalized
}

function Scan-JsonNodeForReferences {
    param(
        [object]$Node,
        [string]$Path,
        [System.Collections.Generic.List[object]]$Results
    )

    if ($null -eq $Node) {
        return
    }

    if ($Node -is [System.Collections.IDictionary] -or $Node -is [pscustomobject]) {
        foreach ($property in $Node.PSObject.Properties) {
            $propertyName = "$($property.Name)"
            $childPath = if ([string]::IsNullOrWhiteSpace($Path)) { $propertyName } else { "$Path.$propertyName" }
            $value = $property.Value

            if ($value -is [string] -and -not [string]::IsNullOrWhiteSpace($value)) {
                switch ($propertyName) {
                    "useProfile" {
                        $Results.Add([pscustomobject]@{ registryId = "items.use_profiles.v1"; code = "V52_REF_UNKNOWN_USE_PROFILE"; value = $value; sourcePath = $childPath; allowCore = $false })
                    }
                    "status" {
                        $Results.Add([pscustomobject]@{ registryId = "statuses.catalog.v1"; code = "V52_REF_UNKNOWN_STATUS"; value = $value; sourcePath = $childPath; allowCore = $true })
                    }
                    "statusId" {
                        $Results.Add([pscustomobject]@{ registryId = "statuses.catalog.v1"; code = "V52_REF_UNKNOWN_STATUS"; value = $value; sourcePath = $childPath; allowCore = $true })
                    }
                    "spellId" {
                        $Results.Add([pscustomobject]@{ registryId = "spells.catalog.v1"; code = "V52_REF_UNKNOWN_SPELL"; value = $value; sourcePath = $childPath; allowCore = $false })
                    }
                    "postFxPresetId" {
                        $Results.Add([pscustomobject]@{ registryId = "render.postfx.v1"; code = "V52_REF_UNKNOWN_POSTFX_PRESET"; value = $value; sourcePath = $childPath; allowCore = $false })
                    }
                    "skylightProfileId" {
                        $Results.Add([pscustomobject]@{ registryId = "render.lighting.v1"; code = "V52_REF_UNKNOWN_LIGHT_PROFILE"; value = $value; sourcePath = $childPath; allowCore = $false })
                    }
                    "lightProfileId" {
                        $Results.Add([pscustomobject]@{ registryId = "render.lighting.v1"; code = "V52_REF_UNKNOWN_LIGHT_PROFILE"; value = $value; sourcePath = $childPath; allowCore = $false })
                    }
                }
            }

            Scan-JsonNodeForReferences -Node $value -Path $childPath -Results $Results
        }
        return
    }

    if ($Node -is [System.Collections.IEnumerable] -and $Node -isnot [string]) {
        $index = 0
        foreach ($item in $Node) {
            $itemPath = if ([string]::IsNullOrWhiteSpace($Path)) { "[$index]" } else { "$Path[$index]" }
            Scan-JsonNodeForReferences -Node $item -Path $itemPath -Results $Results
            $index++
        }
    }
}

function Add-GeneralReferenceIssues {
    param(
        [object]$Report,
        [string]$ModDir,
        [hashtable]$KnownIdsByRegistry
    )

    $referenceHits = New-Object 'System.Collections.Generic.List[object]'
    foreach ($jsonFile in (Get-JsonFilesForReferenceScan -ModDir $ModDir)) {
        $json = $null
        try {
            $json = Get-Content -Path $jsonFile.FullName -Raw | ConvertFrom-Json
        } catch {
            continue
        }

        $relativePath = (Get-RelativePathCompat -BasePath $ModDir -TargetPath $jsonFile.FullName).Replace('\', '/')
        Scan-JsonNodeForReferences -Node $json -Path $relativePath -Results $referenceHits
    }

    $reportedKeys = @{}
    foreach ($hit in $referenceHits) {
        $value = "$($hit.value)".Trim()
        if ([string]::IsNullOrWhiteSpace($value)) {
            continue
        }
        if ($hit.allowCore -and (Test-IsBuiltinStatusReference -Value $value)) {
            continue
        }

        $knownIds = @()
        if ($KnownIdsByRegistry.ContainsKey($hit.registryId)) {
            $knownIds = @($KnownIdsByRegistry[$hit.registryId])
        }
        if ($knownIds -contains $value) {
            continue
        }

        $dedupeKey = "$($hit.code)|$($hit.sourcePath)|$value"
        if ($reportedKeys.ContainsKey($dedupeKey)) {
            continue
        }
        $reportedKeys[$dedupeKey] = $true

        Add-ModIssue -Report $Report -Severity "ERROR" -Code $hit.code `
            -Message "reference '$value' is not available in the current validation scope" `
            -SourcePath $hit.sourcePath -RegistryId $hit.registryId -ReferencedId $value `
            -SuggestedFix "Define the referenced id in this mod or validate together with the dependency that provides it."
    }
}

function Validate-DeclaredPathField {
    param(
        [object]$Report,
        [object]$Manifest,
        [string]$ModDir,
        [string]$FieldName,
        [string]$RegistryId = "",
        [string]$SchemaVersion = "mod.manifest.v2",
        [hashtable]$ConventionalFileMap = @{},
        [string]$DataRoot = "",
        [string]$RuntimeRoot = ""
    )

    $relativePath = ""
    if ($Manifest.PSObject.Properties.Name -contains $FieldName) {
        $relativePath = "$($Manifest.$FieldName)"
    } elseif ($SchemaVersion -eq "mod.manifest.v3" -and $ConventionalFileMap.ContainsKey($RegistryId)) {
        $relativePath = "$($ConventionalFileMap[$RegistryId])"
    } else {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_MISSING_FIELD" `
            -Message "capability $RegistryId requires field $FieldName" -RegistryId $RegistryId `
            -SuggestedFix "Add $FieldName to mod.json or drop the unused capability."
        return $false
    }

    if ([string]::IsNullOrWhiteSpace($relativePath)) {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_EMPTY_FIELD" `
            -Message "field $FieldName is empty" -SourcePath "mod.json::$FieldName" -RegistryId $RegistryId `
            -SuggestedFix "Point $FieldName to a valid file or remove the field."
        return $false
    }

    $absolutePath = Resolve-ModFilePath -RelativePath $relativePath -DataRoot $DataRoot -RuntimeRoot $RuntimeRoot
    if (-not (Test-Path $absolutePath)) {
        Add-ModIssue -Report $Report -Severity "FATAL" -Code "V52_FATAL_MISSING_FILE" `
            -Message "missing file for ${FieldName}: $relativePath" -SourcePath $relativePath -RegistryId $RegistryId `
            -SuggestedFix "Create the file or remove the capability/field pair."
        return $false
    }

    if ($absolutePath.ToLowerInvariant().EndsWith(".json")) {
        $json = Test-JsonFile -AbsolutePath $absolutePath -DisplayPath $relativePath -Report $Report -CodePrefix "V52_FATAL" -RegistryId $RegistryId
        if ($null -eq $json) {
            return $false
        }
    }

    if ($VerboseOutput) {
        Write-Host "[validate_mod] $($Report.modId) OK -> $RegistryId ($relativePath)"
    }

    return $true
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$resolvedModPath = if ([string]::IsNullOrWhiteSpace($ModPath)) {
    if (Test-Path (Join-Path $resolvedRoot "x64/Release/mods_data")) {
        Join-Path $resolvedRoot "x64/Release/mods_data"
    } else {
        Join-Path $resolvedRoot "x64/Release/mods"
    }
} else {
    if ([System.IO.Path]::IsPathRooted($ModPath)) { $ModPath } else { Join-Path $resolvedRoot $ModPath }
}

if (-not (Test-Path $resolvedModPath)) {
    Fail "ModPath not found: $resolvedModPath"
}

$mods = @()
if (-not [string]::IsNullOrWhiteSpace((Get-ModManifestPath -ModDir $resolvedModPath))) {
    $mods = @((Resolve-Path $resolvedModPath).Path)
} else {
    $mods = @(Get-ChildItem -Path $resolvedModPath -Directory | Where-Object {
            -not [string]::IsNullOrWhiteSpace((Get-ModManifestPath -ModDir $_.FullName))
        } | Select-Object -ExpandProperty FullName)
}

if ($mods.Count -eq 0) {
    Fail "No mod directories found in: $resolvedModPath"
}

$capabilityPathMap = Resolve-CapabilityPathMap
$conventionalCapabilityFileMap = Resolve-ConventionalCapabilityFileMap
$pathlessCapabilities = @("native.sdk.v1", "native.raw_cpp.v1", "world.queries.v1", "dsl.components.v1", "dsl.entities.v1", "dsl.graphs.v1", "services.endpoints.v1")
$supportedRuntimePermissions = Resolve-SupportedRuntimePermissions
$reports = @()

foreach ($modDir in $mods) {
    $manifestPath = Get-ModManifestPath -ModDir $modDir
    if ([string]::IsNullOrWhiteSpace($manifestPath) -or -not (Test-Path $manifestPath)) {
        Write-Warning "Skipping $modDir (missing mod.json)"
        continue
    }

    $manifest = $null
    try {
        $manifest = Get-Content -Path $manifestPath -Raw | ConvertFrom-Json
    } catch {
        $fallbackId = Split-Path -Path $modDir -Leaf
        $report = New-ModReport -ModId $fallbackId -ModDir $modDir
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_MANIFEST_PARSE" `
            -Message "mod.json parse error: $($_.Exception.Message)" -SourcePath $manifestPath `
            -SuggestedFix "Fix invalid JSON in mod.json."
        $reports += [pscustomobject]$report
        continue
    }

    $modId = if ($manifest.PSObject.Properties.Name -contains "id" -and -not [string]::IsNullOrWhiteSpace("$($manifest.id)")) {
        "$($manifest.id)"
    } else {
        Split-Path -Path $modDir -Leaf
    }
    $report = New-ModReport -ModId $modId -ModDir $modDir
    $entryJsonForScan = $null
    $inputDefinitionsJsonForScan = $null
    $schemaVersion = Get-ManifestSchemaVersion -Manifest $manifest
    $dataRoot = Get-ModDataRoot -ModDir $modDir -ManifestPath $manifestPath
    $runtimeRoot = Get-ModRuntimeRoot -ModDir $modDir -DataRoot $dataRoot

    $manifestType = ""
    if (-not ($manifest.PSObject.Properties.Name -contains "type") -or [string]::IsNullOrWhiteSpace("$($manifest.type)")) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_TYPE_MISSING" `
            -Message "field type is required and must be framework|content|tools" -SourcePath "mod.json::type"
    } else {
        $manifestType = "$($manifest.type)".ToLowerInvariant()
        if (@("framework", "content", "tools") -notcontains $manifestType) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_TYPE_INVALID" `
                -Message "field type must be framework|content|tools (found: $manifestType)" -SourcePath "mod.json::type"
        }
    }

    $declaredReleaseChannels = Get-ManifestReleaseChannels -Manifest $manifest
    if (@($declaredReleaseChannels | Where-Object { $_ -ne "stable" }).Count -gt 0) {
        Add-ModIssue -Report $report -Severity "WARN" -Code "V52_WARN_RELEASE_CHANNEL_GATED" `
            -Message "non-stable releaseChannels are present; modloader UI keeps beta/testers gated until v1.0" `
            -SourcePath "mod.json::releaseChannels"
    }
    Test-ManifestLockfile -Report $report -Manifest $manifest -ModDir $modDir -ReleaseChannels $declaredReleaseChannels

    if (-not ($manifest.PSObject.Properties.Name -contains "apiVersion") -or $manifest.apiVersion -ne 4) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_APIVERSION" `
            -Message "apiVersion must be 4 (found: $($manifest.apiVersion))" -SourcePath "mod.json::apiVersion"
    }

    if ($manifestType -eq "framework") {
        if (-not ($manifest.PSObject.Properties.Name -contains "provides")) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FRAMEWORK_PROVIDES_MISSING" `
                -Message "framework mods must declare provides[]" -SourcePath "mod.json::provides"
        }
        if (-not ($manifest.PSObject.Properties.Name -contains "exports") -or $null -eq $manifest.exports -or
            @($manifest.exports.PSObject.Properties).Count -eq 0) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FRAMEWORK_EXPORTS_MISSING" `
                -Message "framework mods must declare exports{}" -SourcePath "mod.json::exports"
        }
    }
    if (@("framework", "content") -contains $manifestType -and -not ($manifest.PSObject.Properties.Name -contains "uses")) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_USES_MISSING" `
            -Message "framework/content mods must declare uses[]" -SourcePath "mod.json::uses"
    }

    $hasRuntime = ($manifest.PSObject.Properties.Name -contains "runtime") -and ($null -ne $manifest.runtime)
    $runtimeType = ""
    if ($hasRuntime -and ($manifest.runtime.PSObject.Properties.Name -contains "type")) {
        $runtimeType = "$($manifest.runtime.type)"
    }
    $runtimeType = "$runtimeType".ToLowerInvariant()
    $runtimeTypeAliases = @{
        "data-only" = "data-only"
        "data-only-v1" = "data-only"
        "wasm" = "wasm3-v1"
        "wasm3-v1" = "wasm3-v1"
        "native" = "native-cpp-v1"
        "native-cpp-v1" = "native-cpp-v1"
        "hybrid" = "hybrid-v1"
        "hybrid-v1" = "hybrid-v1"
    }
    if ($runtimeTypeAliases.ContainsKey($runtimeType)) {
        $runtimeType = $runtimeTypeAliases[$runtimeType]
    }
    if (-not $hasRuntime -or @("data-only", "wasm3-v1", "native-cpp-v1", "hybrid-v1") -notcontains $runtimeType) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_RUNTIME_TYPE" `
            -Message "runtime.type must be data-only|wasm|native|hybrid or the explicit v1 tokens" -SourcePath "mod.json::runtime.type"
    }

    $runtimeModule = ""
    $runtimeEntryLibrary = ""
    if ($hasRuntime -and ($manifest.runtime.PSObject.Properties.Name -contains "module")) {
        $runtimeModule = "$($manifest.runtime.module)"
    }
    if ($hasRuntime -and ($manifest.runtime.PSObject.Properties.Name -contains "entryLibrary")) {
        $runtimeEntryLibrary = "$($manifest.runtime.entryLibrary)"
    }
    $requiresEntryScript = @("wasm3-v1", "hybrid-v1") -contains $runtimeType
    $requiresRuntimeModule = @("wasm3-v1", "hybrid-v1") -contains $runtimeType
    $requiresEntryLibrary = @("native-cpp-v1", "hybrid-v1") -contains $runtimeType

    $entryScript = ""
    if ($manifest.PSObject.Properties.Name -contains "entryScript") {
        $entryScript = "$($manifest.entryScript)"
    }
    if ([string]::IsNullOrWhiteSpace($entryScript) -and $schemaVersion -eq "mod.manifest.v3" -and $requiresEntryScript) {
        foreach ($candidate in @("scripts/init.json", "scripts/entry.json")) {
            if (Test-Path (Resolve-ModFilePath -RelativePath $candidate -DataRoot $dataRoot -RuntimeRoot $runtimeRoot)) {
                $entryScript = $candidate
                break
            }
        }
    }
    if ($requiresEntryScript -and [string]::IsNullOrWhiteSpace($entryScript)) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYSCRIPT_MISSING" `
            -Message "missing entryScript" -SourcePath "mod.json::entryScript"
    } elseif (-not [string]::IsNullOrWhiteSpace($entryScript)) {
        $entryScriptPath = Resolve-ModFilePath -RelativePath $entryScript -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
        if (-not (Test-Path $entryScriptPath)) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYSCRIPT_FILE" `
                -Message "missing entryScript file: $entryScript" -SourcePath $entryScript `
                -SuggestedFix "Create the file or correct entryScript in mod.json."
        } else {
            $entryJson = Test-JsonFile -AbsolutePath $entryScriptPath -DisplayPath $entryScript -Report $report -CodePrefix "V52_FATAL_ENTRYSCRIPT"
            $entryJsonForScan = $entryJson
            if ($null -ne $entryJson) {
                if (-not ($entryJson.PSObject.Properties.Name -contains "apiVersion")) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYSCRIPT_APIVERSION_MISSING" `
                        -Message "entryScript missing apiVersion" -SourcePath $entryScript
                } elseif ($manifest.PSObject.Properties.Name -contains "apiVersion" -and $entryJson.apiVersion -ne $manifest.apiVersion) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYSCRIPT_APIVERSION_MISMATCH" `
                        -Message "entryScript apiVersion mismatch (manifest=$($manifest.apiVersion), script=$($entryJson.apiVersion))" `
                        -SourcePath $entryScript
                }
            }
        }
    }

    if ($requiresRuntimeModule) {
        if ([string]::IsNullOrWhiteSpace($runtimeModule) -and $schemaVersion -eq "mod.manifest.v3") {
            foreach ($candidate in @("scripts/module.wasm", "scripts/module.wat", "scripts/noop.wat")) {
                if (Test-Path (Resolve-ModFilePath -RelativePath $candidate -DataRoot $dataRoot -RuntimeRoot $runtimeRoot)) {
                    $runtimeModule = $candidate
                    break
                }
            }
        }
        if ([string]::IsNullOrWhiteSpace($runtimeModule)) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_RUNTIME_MODULE_MISSING" `
                -Message "runtime.module is required for $runtimeType" -SourcePath "mod.json::runtime.module"
        } else {
            $runtimeModulePath = Resolve-ModFilePath -RelativePath $runtimeModule -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
            if (-not (Test-Path $runtimeModulePath)) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_RUNTIME_MODULE_FILE" `
                    -Message "missing runtime module: $runtimeModule" -SourcePath $runtimeModule
            } elseif (@(".wasm", ".wat") -notcontains [System.IO.Path]::GetExtension($runtimeModulePath).ToLowerInvariant()) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_RUNTIME_MODULE_EXT" `
                    -Message "runtime.module must end with .wasm or .wat" -SourcePath $runtimeModule
            }
        }
    }

    if ($requiresEntryLibrary) {
        if ([string]::IsNullOrWhiteSpace($runtimeEntryLibrary) -and $schemaVersion -eq "mod.manifest.v3") {
            $defaultLibrary = "native/win64/plugin.dll"
            if (Test-Path (Resolve-ModFilePath -RelativePath $defaultLibrary -DataRoot $dataRoot -RuntimeRoot $runtimeRoot)) {
                $runtimeEntryLibrary = $defaultLibrary
            }
        }
        if ([string]::IsNullOrWhiteSpace($runtimeEntryLibrary)) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYLIBRARY_MISSING" `
                -Message "runtime.entryLibrary is required for $runtimeType" -SourcePath "mod.json::runtime.entryLibrary"
        } else {
            $runtimeEntryLibraryPath = Resolve-ModFilePath -RelativePath $runtimeEntryLibrary -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
            if (-not (Test-Path $runtimeEntryLibraryPath)) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYLIBRARY_FILE" `
                    -Message "missing runtime entryLibrary: $runtimeEntryLibrary" -SourcePath $runtimeEntryLibrary
            } elseif ([System.IO.Path]::GetExtension($runtimeEntryLibraryPath).ToLowerInvariant() -ne ".dll") {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_ENTRYLIBRARY_EXT" `
                    -Message "runtime.entryLibrary must end with .dll" -SourcePath $runtimeEntryLibrary
            }
        }
    }

    foreach ($optionalJsonField in @("itemDefinitions", "inputDefinitions")) {
        if ($manifest.PSObject.Properties.Name -contains $optionalJsonField) {
            $relativePath = "$($manifest.$optionalJsonField)"
            if ([string]::IsNullOrWhiteSpace($relativePath)) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_EMPTY_FIELD" `
                    -Message "field $optionalJsonField is empty" -SourcePath "mod.json::$optionalJsonField" `
                    -SuggestedFix "Remove the field if unused or point it at a valid file."
                continue
            }

            $absolutePath = Resolve-ModFilePath -RelativePath $relativePath -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
            if (-not (Test-Path $absolutePath)) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_MISSING_FILE" `
                    -Message "missing $optionalJsonField file: $relativePath" -SourcePath $relativePath
                continue
            }

            $validatedJson = Test-JsonFile -AbsolutePath $absolutePath -DisplayPath $relativePath -Report $report -CodePrefix "V52_FATAL"
            if ($optionalJsonField -eq "inputDefinitions") {
                $inputDefinitionsJsonForScan = $validatedJson
            }
        }
    }

    Add-OnInputBindingReferenceIssues -Report $report -EntryJson $entryJsonForScan `
        -KnownBindingIds (Get-InputBindingIds -InputJson $inputDefinitionsJsonForScan) -EntryScriptPath $entryScript
    $knownIdsByRegistry = @{
        "items.use_profiles.v1" = Get-IdsFromCatalog -Json (Load-ManifestJsonFieldIfPresent -Report $report -Manifest $manifest -ModDir $modDir -FieldName "itemUseProfiles" -DataRoot $dataRoot -RuntimeRoot $runtimeRoot) -CollectionProperty "useProfiles"
        "statuses.catalog.v1" = Get-IdsFromCatalog -Json (Load-ManifestJsonFieldIfPresent -Report $report -Manifest $manifest -ModDir $modDir -FieldName "statusDefinitions" -DataRoot $dataRoot -RuntimeRoot $runtimeRoot) -CollectionProperty "statuses"
        "spells.catalog.v1" = Get-IdsFromCatalog -Json (Load-ManifestJsonFieldIfPresent -Report $report -Manifest $manifest -ModDir $modDir -FieldName "spellDefinitions" -DataRoot $dataRoot -RuntimeRoot $runtimeRoot) -CollectionProperty "spells"
        "render.postfx.v1" = Get-IdsFromCatalog -Json (Load-ManifestJsonFieldIfPresent -Report $report -Manifest $manifest -ModDir $modDir -FieldName "postFxDefinitions" -DataRoot $dataRoot -RuntimeRoot $runtimeRoot) -CollectionProperty "presets"
        "render.lighting.v1" = Get-IdsFromCatalog -Json (Load-ManifestJsonFieldIfPresent -Report $report -Manifest $manifest -ModDir $modDir -FieldName "lightingDefinitions" -DataRoot $dataRoot -RuntimeRoot $runtimeRoot) -CollectionProperty "profiles"
    }
    Add-GeneralReferenceIssues -Report $report -ModDir $modDir -KnownIdsByRegistry $knownIdsByRegistry

    foreach ($fieldName in @("provides", "uses")) {
        if ($manifest.PSObject.Properties.Name -contains $fieldName) {
            $fieldValue = $manifest.$fieldName
            if ($fieldValue -is [string] -or $fieldValue -isnot [System.Collections.IEnumerable]) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FIELD_TYPE" `
                    -Message "field $fieldName must be array" -SourcePath "mod.json::$fieldName"
            } else {
                $entryIndex = 0
                foreach ($entry in $fieldValue) {
                    if ($null -eq $entry -or [string]::IsNullOrWhiteSpace("$entry")) {
                        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FIELD_ENTRY" `
                            -Message "field $fieldName`[$entryIndex`] must be non-empty string" -SourcePath "mod.json::$fieldName"
                    }
                    $entryIndex++
                }
            }
        }
    }

    if (($manifest.PSObject.Properties.Name -contains "files") -and $null -ne $manifest.files) {
        if ($manifest.files.PSObject.Properties.Name -contains "optional") {
            $optionalFiles = $manifest.files.optional
            if ($optionalFiles -is [string] -or $optionalFiles -isnot [System.Collections.IEnumerable]) {
                Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FILES_OPTIONAL_TYPE" `
                    -Message "files.optional must be array" -SourcePath "mod.json::files.optional"
            } else {
                $optionalIndex = 0
                foreach ($optionalPath in $optionalFiles) {
                    if ($null -eq $optionalPath -or [string]::IsNullOrWhiteSpace("$optionalPath")) {
                        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_FILES_OPTIONAL_ENTRY" `
                            -Message "files.optional[$optionalIndex] must be non-empty string" -SourcePath "mod.json::files.optional"
                    }
                    $optionalIndex++
                }
            }
        }
    }

    if (($manifest.PSObject.Properties.Name -contains "settings") -and $null -ne $manifest.settings) {
        $settings = $manifest.settings
        if ($settings -is [string]) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_TYPE" `
                -Message "settings must be object" -SourcePath "mod.json::settings"
        } else {
            if ($settings.PSObject.Properties.Name -contains "schemaVersion") {
                if ($settings.schemaVersion -ne 1) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_SCHEMA_VERSION" `
                        -Message "settings.schemaVersion must be 1" -SourcePath "mod.json::settings.schemaVersion"
                }
            }
            if ($settings.PSObject.Properties.Name -contains "defaultDomain") {
                $defaultDomain = "$($settings.defaultDomain)".ToLowerInvariant()
                if (@("global", "save", "session") -notcontains $defaultDomain) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_DEFAULT_DOMAIN" `
                        -Message "settings.defaultDomain must be global|save|session" -SourcePath "mod.json::settings.defaultDomain"
                }
            }
            if ($settings.PSObject.Properties.Name -contains "schemaFile") {
                $settingsSchemaPath = "$($settings.schemaFile)"
                if ([string]::IsNullOrWhiteSpace($settingsSchemaPath)) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_SCHEMAFILE_EMPTY" `
                        -Message "settings.schemaFile is empty" -SourcePath "mod.json::settings.schemaFile"
                } else {
                    $settingsSchemaAbsolute = Resolve-ModFilePath -RelativePath $settingsSchemaPath -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
                    if (-not (Test-Path $settingsSchemaAbsolute)) {
                        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_SCHEMAFILE_MISSING" `
                            -Message "missing settings schema file: $settingsSchemaPath" -SourcePath $settingsSchemaPath
                    } else {
                        $settingsSchema = Test-JsonFile -AbsolutePath $settingsSchemaAbsolute -DisplayPath $settingsSchemaPath -Report $report -CodePrefix "V52_FATAL_SETTINGS_SCHEMA"
                        if ($null -ne $settingsSchema -and $settingsSchema.version -ne 1) {
                            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_SETTINGS_SCHEMA_VERSION" `
                                -Message "settings schema version must be 1" -SourcePath $settingsSchemaPath
                        }
                    }
                }
            }
        }
    }

    if ($manifest.PSObject.Properties.Name -contains "dependencies") {
        if ($null -eq $manifest.dependencies) {
            # allowed
        } elseif ($manifest.dependencies -is [string] -or $manifest.dependencies -isnot [System.Collections.IEnumerable]) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCIES_TYPE" `
                -Message "dependencies must be array" -SourcePath "mod.json::dependencies"
        } else {
            $depIndex = 0
            foreach ($dep in $manifest.dependencies) {
                $depId = ""
                $depRange = ""

                if ($dep -is [string]) {
                    $depId = $dep.Trim()
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_STRING_LEGACY" `
                        -Message "dependencies[$depIndex] must use object form { id, versionRange }; string form is legacy" -SourcePath "mod.json::dependencies"
                } elseif ($dep -ne $null -and $dep.PSObject -ne $null) {
                    if ($dep.PSObject.Properties.Name -contains "id") { $depId = "$($dep.id)".Trim() }
                    if ($dep.PSObject.Properties.Name -contains "modId") {
                        $legacyDepId = "$($dep.modId)".Trim()
                        if (-not [string]::IsNullOrWhiteSpace($legacyDepId)) {
                            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_MODID_LEGACY" `
                                -Message "dependencies[$depIndex] uses legacy field modId; use id instead" -SourcePath "mod.json::dependencies"
                            if ([string]::IsNullOrWhiteSpace($depId)) {
                                $depId = $legacyDepId
                            }
                        }
                    }
                    if ($dep.PSObject.Properties.Name -contains "versionRange") {
                        $depRange = "$($dep.versionRange)"
                    } elseif ($dep.PSObject.Properties.Name -contains "version") {
                        $depRange = "$($dep.version)"
                    }
                } else {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_ENTRY_TYPE" `
                        -Message "dependencies[$depIndex] must be string or object" -SourcePath "mod.json::dependencies"
                    $depIndex++
                    continue
                }

                if ([string]::IsNullOrWhiteSpace($depId)) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_ID" `
                        -Message "dependencies[$depIndex] missing id/modId" -SourcePath "mod.json::dependencies"
                }
                if ([string]::IsNullOrWhiteSpace($depRange)) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_RANGE_MISSING" `
                        -Message "dependencies[$depIndex] must declare versionRange" -SourcePath "mod.json::dependencies"
                }
                if (-not (Test-VersionRangeSyntax -Range $depRange)) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_DEPENDENCY_RANGE" `
                        -Message "dependencies[$depIndex] invalid versionRange syntax: '$depRange'" -SourcePath "mod.json::dependencies"
                }
                $depIndex++
            }
        }
    }

    $caps = @()
    if ($manifest.PSObject.Properties.Name -contains "capabilities") {
        $capabilitiesValue = $manifest.capabilities
        if ($capabilitiesValue -is [string] -or ($null -ne $capabilitiesValue -and $capabilitiesValue -isnot [System.Collections.IEnumerable])) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_CAPABILITIES_TYPE" `
                -Message "capabilities must be array" -SourcePath "mod.json::capabilities"
        } else {
            foreach ($cap in $capabilitiesValue) {
                if ($null -ne $cap -and -not [string]::IsNullOrWhiteSpace("$cap")) {
                    $caps += "$cap".ToLowerInvariant()
                }
            }
        }
    }

    $capabilityRationales = $null
    if ($manifest.PSObject.Properties.Name -contains "capabilityRationales") {
        $capabilityRationales = $manifest.capabilityRationales
    }
    foreach ($cap in $caps) {
        if ($null -eq $capabilityRationales -or -not ($capabilityRationales.PSObject.Properties.Name -contains $cap) -or [string]::IsNullOrWhiteSpace("$($capabilityRationales.$cap)")) {
            Add-ModIssue -Report $report -Severity "WARN" -Code "V52_WARN_CAPABILITY_RATIONALE" `
                -Message "missing capability rationale for $cap" -SourcePath "mod.json::capabilityRationales" -RegistryId $cap
        }
    }

    $permissions = @()
    if ($manifest.PSObject.Properties.Name -contains "permissions") {
        if ($manifest.permissions -is [string] -or $manifest.permissions -isnot [System.Collections.IEnumerable]) {
            Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_PERMISSIONS_TYPE" `
                -Message "permissions must be array" -SourcePath "mod.json::permissions"
        } else {
            foreach ($permission in $manifest.permissions) {
                $normalizedPermission = "$permission".ToLowerInvariant()
                if ([string]::IsNullOrWhiteSpace($normalizedPermission)) {
                    continue
                }
                if ($supportedRuntimePermissions -notcontains $normalizedPermission) {
                    Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_PERMISSION_UNSUPPORTED" `
                        -Message "unsupported permission '$normalizedPermission'" -SourcePath "mod.json::permissions"
                    continue
                }
                $permissions += $normalizedPermission
            }
        }
    }

    $permissionRationales = $null
    if ($manifest.PSObject.Properties.Name -contains "permissionRationales") {
        $permissionRationales = $manifest.permissionRationales
    }
    foreach ($permission in $permissions) {
        if ($null -eq $permissionRationales -or -not ($permissionRationales.PSObject.Properties.Name -contains $permission) -or [string]::IsNullOrWhiteSpace("$($permissionRationales.$permission)")) {
            Add-ModIssue -Report $report -Severity "WARN" -Code "V52_WARN_PERMISSION_RATIONALE" `
                -Message "missing permission rationale for $permission" -SourcePath "mod.json::permissionRationales"
        }
    }

    $hasCameraV1 = $caps -contains "camera.aim_profiles.v1"
    $hasCameraV2 = $caps -contains "camera.aim_profiles.v2"
    $hasCameraDefinitions = $manifest.PSObject.Properties.Name -contains "cameraDefinitions"
    if (($manifest.PSObject.Properties.Name -contains "apiVersion") -and $manifest.apiVersion -eq 4 -and $hasCameraV1) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_CAMERA_V1_LEGACY" `
            -Message "capability camera.aim_profiles.v1 is legacy in apiVersion 4; use camera.aim_profiles.v2" `
            -SourcePath "mod.json::capabilities"
    }
    if (($manifest.PSObject.Properties.Name -contains "apiVersion") -and $manifest.apiVersion -eq 4 -and $hasCameraDefinitions -and -not $hasCameraV2) {
        Add-ModIssue -Report $report -Severity "FATAL" -Code "V52_FATAL_CAMERA_CAPABILITY_MISMATCH" `
            -Message "cameraDefinitions requires capability camera.aim_profiles.v2" -SourcePath "mod.json::cameraDefinitions"
    }

    foreach ($cap in $caps) {
        if ($pathlessCapabilities -contains $cap) {
            continue
        }
        if (-not $capabilityPathMap.ContainsKey($cap)) {
            Add-ModIssue -Report $report -Severity "INFO" -Code "V52_INFO_UNKNOWN_CAPABILITY" `
                -Message "capability '$cap' is not checked by validate_mod (skipping path validation)" -SourcePath "mod.json::capabilities"
            continue
        }
        $fieldName = $capabilityPathMap[$cap]
        $null = Validate-DeclaredPathField -Report $report -Manifest $manifest -ModDir $modDir -FieldName $fieldName `
            -RegistryId $cap -SchemaVersion $schemaVersion -ConventionalFileMap $conventionalCapabilityFileMap `
            -DataRoot $dataRoot -RuntimeRoot $runtimeRoot
    }

    $reports += [pscustomobject]$report
}

$summary = New-SeveritySummary
foreach ($report in $reports) {
    $summary.info += [int]$report.severitySummary.info
    $summary.warn += [int]$report.severitySummary.warn
    $summary.error += [int]$report.severitySummary.error
    $summary.fatal += [int]$report.severitySummary.fatal
}

$reportRoot = Join-Path $resolvedRoot "build/modtool_reports"
if (-not (Test-Path $reportRoot)) {
    New-Item -ItemType Directory -Path $reportRoot -Force | Out-Null
}
$reportPath = Join-Path $reportRoot "validate_mod_report.json"
$reportEnvelope = [ordered]@{
    schemaVersion = "tool.validate_report.v1"
    command = "validate_mod"
    generatedUtc = (Get-Date).ToUniversalTime().ToString("yyyy-MM-ddTHH:mm:ssZ")
    success = ($summary.fatal -eq 0)
    targetPath = $resolvedModPath
    severitySummary = $summary
    mods = $reports
}
($reportEnvelope | ConvertTo-Json -Depth 12) | Set-Content -Path $reportPath -Encoding UTF8

Write-Host "validate_mod report -> $reportPath"
if ($summary.fatal -gt 0) {
    Write-Host "validate_mod failed: fatal=$($summary.fatal) warn=$($summary.warn) error=$($summary.error) info=$($summary.info)" -ForegroundColor Red
    exit 1
}

Write-Host "validate_mod passed: mods=$($reports.Count) warn=$($summary.warn) error=$($summary.error) info=$($summary.info)" -ForegroundColor Green
exit 0

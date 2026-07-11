[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("init", "init-native", "build-sdk-sample", "validate", "pack", "sign", "sign-native", "doctor", "doctor-native", "diff", "report", "report-native")]
    [string]$Command,
    [string]$RepoRoot = ".",
    [string]$Path = "",
    [ValidateSet("framework", "content", "pack", "native")]
    [string]$Template = "content",
    [string]$Family = "",
    [string]$OutDir = "",
    [string]$Name = "",
    [string]$Id = "",
    [string]$Version = "0.1.0",
    [ValidateSet("stable", "beta", "testers")]
    [string]$ReleaseChannel = "stable",
    [string]$ArtifactA = "",
    [string]$ArtifactB = "",
    [switch]$IncludePdb,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"
Add-Type -AssemblyName System.IO.Compression
Add-Type -AssemblyName System.IO.Compression.FileSystem
$DeterministicTimestampIso = "2000-01-01T00:00:00Z"

function New-Report([string]$Schema) {
    return [ordered]@{
        schemaVersion = $Schema
        command = $Command
        generatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
        success = $true
        errors = @()
        warnings = @()
        data = [ordered]@{}
    }
}

function Add-Error([System.Collections.IDictionary]$Report, [string]$Message) {
    $Report.errors += $Message
    $Report.success = $false
}

function Add-Warn([System.Collections.IDictionary]$Report, [string]$Message) {
    $Report.warnings += $Message
}

function Resolve-Abs([string]$Base, [string]$Child) {
    if ([string]::IsNullOrWhiteSpace($Child)) { return $Base }
    if ([System.IO.Path]::IsPathRooted($Child)) { return $Child }
    return (Join-Path $Base $Child)
}

function Get-Rel([string]$Root, [string]$FullPath) {
    if ([string]::IsNullOrWhiteSpace($Root) -or [string]::IsNullOrWhiteSpace($FullPath)) {
        return $FullPath.Replace("\", "/")
    }

    $pathType = [System.IO.Path]
    if ($pathType.GetMethods() | Where-Object { $_.Name -eq "GetRelativePath" }) {
        return [System.IO.Path]::GetRelativePath($Root, $FullPath).Replace("\", "/")
    }

    $rootFull = [System.IO.Path]::GetFullPath($Root)
    $full = [System.IO.Path]::GetFullPath($FullPath)
    if (-not $rootFull.EndsWith([System.IO.Path]::DirectorySeparatorChar.ToString())) {
        $rootFull += [System.IO.Path]::DirectorySeparatorChar
    }
    $rootUri = New-Object System.Uri($rootFull)
    $fullUri = New-Object System.Uri($full)
    $relativeUri = $rootUri.MakeRelativeUri($fullUri)
    return [System.Uri]::UnescapeDataString($relativeUri.ToString()).Replace("\", "/")
}

function Get-ModManifestPath([string]$Target) {
    $rootManifest = Join-Path $Target "mod.json"
    if (Test-Path $rootManifest) { return $rootManifest }
    $splitManifest = Join-Path $Target "data/mod.json"
    if (Test-Path $splitManifest) { return $splitManifest }
    return ""
}

function Get-ModDataRoot([string]$Target, [string]$ManifestPath) {
    if ([string]::IsNullOrWhiteSpace($ManifestPath)) { return $Target }
    return Split-Path -Path $ManifestPath -Parent
}

function Get-ModRuntimeRoot([string]$Target, [string]$DataRoot) {
    $embeddedRuntime = Join-Path $Target "runtime"
    if (Test-Path $embeddedRuntime) { return $embeddedRuntime }

    $dataParent = Split-Path -Path $DataRoot -Parent
    if ((Split-Path -Path $dataParent -Leaf).ToLowerInvariant() -eq "mods_data") {
        $runtimeSibling = Join-Path (Join-Path (Split-Path -Path $dataParent -Parent) "mods_runtime") (Split-Path -Path $DataRoot -Leaf)
        if (Test-Path $runtimeSibling) { return $runtimeSibling }
    }

    return ""
}

function Get-ModDirs([string]$Target) {
    if (-not [string]::IsNullOrWhiteSpace((Get-ModManifestPath -Target $Target))) { return @($Target) }
    return @(Get-ChildItem -Path $Target -Directory | Where-Object {
            -not [string]::IsNullOrWhiteSpace((Get-ModManifestPath -Target $_.FullName))
        } | ForEach-Object { $_.FullName })
}

function Get-PackDirs([string]$Target) {
    if (Test-Path (Join-Path $Target "pack.json")) { return @($Target) }
    return @(Get-ChildItem -Path $Target -Directory | Where-Object { Test-Path (Join-Path $_.FullName "pack.json") } | ForEach-Object { $_.FullName })
}

function Read-JsonFile([string]$JsonPath) {
    try { return Get-Content -Path $JsonPath -Raw -Encoding UTF8 | ConvertFrom-Json } catch { return $null }
}

function Write-JsonFile([string]$JsonPath, $Object) {
    $parent = Split-Path -Path $JsonPath -Parent
    if (-not (Test-Path $parent)) { New-Item -ItemType Directory -Path $parent -Force | Out-Null }
    Set-Content -Path $JsonPath -Value ($Object | ConvertTo-Json -Depth 32) -Encoding UTF8
}

function Set-ObjectProperty {
    param(
        [Parameter(Mandatory = $true)]
        [object]$Object,
        [Parameter(Mandatory = $true)]
        [string]$Name,
        $Value
    )

    if ($Object.PSObject.Properties.Name -contains $Name) {
        $Object.$Name = $Value
    } else {
        $Object | Add-Member -NotePropertyName $Name -NotePropertyValue $Value -Force
    }
}

function New-CanonicalCapabilityRationales([string]$ModName, [string[]]$Capabilities) {
    $map = [ordered]@{}
    foreach ($cap in @($Capabilities)) {
        $map[$cap] = "Declares $cap as part of $ModName in the framework-first API v4 catalog."
    }
    return $map
}

function New-CanonicalPermissionRationales([string]$ModName, [string[]]$Permissions) {
    $map = [ordered]@{}
    foreach ($permission in @($Permissions)) {
        $map[$permission] = "Required by $ModName for the configured runtime and diagnostics surface."
    }
    return $map
}

function New-InitLockfile([string]$ModId, [string]$Version, $Dependencies) {
    return [ordered]@{
        schemaVersion = "mod.lockfile.v1"
        generatedUtc = $DeterministicTimestampIso
        modId = $ModId
        version = $Version
        releaseChannel = "stable"
        resolvedDependencies = @($Dependencies)
        resolvedLoadOrder = @($ModId)
    }
}

function New-CanonicalRuntimeWasm() {
    return [ordered]@{
        type = "wasm3-v1"
        module = "scripts/noop.wat"
        maxMemoryKb = 1024
        maxCallMs = 2
        maxFrameBudgetMs = 2
        maxHookCallsPerFrame = 256
        maxActorInstances = 64
        maxActiveStatuses = 256
    }
}

function New-CanonicalRuntimeHybrid([string]$NativeMode = "sdk") {
    return [ordered]@{
        type = "hybrid-v1"
        module = "scripts/noop.wat"
        entryLibrary = "native/win64/plugin.dll"
        abiVersion = 1
        nativeMode = $NativeMode
        reloadPolicy = "manual"
        buildIdPolicy = "ignore"
        threadModel = "main_thread"
        maxMemoryKb = 1024
        maxCallMs = 2
        maxFrameBudgetMs = 2
        maxHookCallsPerFrame = 256
        maxActorInstances = 64
        maxActiveStatuses = 256
    }
}

function New-FamilyDependency([string]$Id) {
    return [ordered]@{
        id = $Id
        versionRange = ">=0.1.0 <0.2.0"
    }
}

function Get-CanonicalFamilyPreset([string]$TemplateName, [string]$FamilyName) {
    $normalizedFamily = "$FamilyName".Trim().ToLowerInvariant()
    if ([string]::IsNullOrWhiteSpace($normalizedFamily)) {
        return $null
    }

    switch ($TemplateName) {
        "framework" {
            switch ($normalizedFamily) {
                "render_world" {
                    return [ordered]@{
                        provide = "framework.render_world"
                        capabilities = @("native.sdk.v1", "assets.raw.v1", "render.meshes.v1", "actors.prefabs.v1", "world.instances.v1", "editor.placement.v1", "world.authoring.v1")
                        permissions = @("nativeinterop")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                    }
                }
                "narrative" {
                    return [ordered]@{
                        provide = "framework.narrative"
                        capabilities = @("native.sdk.v1", "narrative.flags.v1", "narrative.dialogue.v1", "narrative.quests.v1", "narrative.timeline.v1")
                        permissions = @("nativeinterop")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                    }
                }
                "persistence" {
                    return [ordered]@{
                        provide = "framework.persistence"
                        capabilities = @("native.sdk.v1", "world.persistence.v1", "world.storage.v1", "world.spawn_profiles.v1", "world.time_weather.v1", "world.seeding.v1", "world.migrations.v1")
                        permissions = @("nativeinterop")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                    }
                }
                "ui_inventory" {
                    return [ordered]@{
                        provide = "framework.ui_inventory"
                        capabilities = @("native.sdk.v1", "ui.runtime.v1", "ui.hud.v1", "ui.inventory_ext.v1", "containers.v1", "recipes.processing.v1", "interactions.v1")
                        permissions = @("nativeinterop")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                    }
                }
                "combat_ai" {
                    return [ordered]@{
                        provide = "framework.combat_ai"
                        capabilities = @("native.sdk.v1", "actors.archetypes.v1", "actors.adapters.v1", "ai.behavior_trees.v1", "ai.sensors.v1", "nav.routes.v1", "nav.navmesh_bridge.v1", "statuses.catalog.v1", "combat.targeting.v1", "combat.aoe.v1", "fx.presets.v1", "states.catalog.v1", "spells.catalog.v1", "items.use_profiles.v1")
                        permissions = @("nativeinterop")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                    }
                }
                "devtools_native" {
                    return [ordered]@{
                        provide = "framework.devtools_native"
                        capabilities = @("native.sdk.v1", "native.raw_cpp.v1", "assets.raw.v1", "dev.hot_reload.v1", "dev.console.v1", "dev.watchers.v1", "wasm.sandbox.v2", "debug.reload_inspector.v1")
                        permissions = @("nativeinterop", "nativeinterop.raw", "process")
                        runtime = New-CanonicalRuntimeHybrid "raw"
                    }
                }
            }
        }
        "content" {
            switch ($normalizedFamily) {
                "render_world" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.render_world")
                        uses = @("framework.render_world")
                        capabilities = @("render.meshes.v1", "actors.prefabs.v1", "world.instances.v1", "editor.placement.v1", "world.authoring.v1")
                        runtime = New-CanonicalRuntimeWasm
                        permissions = @()
                    }
                }
                "narrative" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.narrative")
                        uses = @("framework.narrative")
                        capabilities = @("narrative.flags.v1", "narrative.quests.v1")
                        runtime = New-CanonicalRuntimeWasm
                        permissions = @()
                    }
                }
                "persistence" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.persistence")
                        uses = @("framework.persistence")
                        capabilities = @("world.persistence.v1", "world.storage.v1")
                        runtime = New-CanonicalRuntimeWasm
                        permissions = @()
                    }
                }
                "ui_inventory" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.ui_inventory")
                        uses = @("framework.ui_inventory")
                        capabilities = @("ui.runtime.v1", "containers.v1", "recipes.processing.v1", "interactions.v1")
                        runtime = New-CanonicalRuntimeWasm
                        permissions = @()
                    }
                }
                "combat_ai" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.combat_ai")
                        uses = @("framework.combat_ai")
                        capabilities = @("actors.archetypes.v1", "ai.behavior_trees.v1", "combat.targeting.v1")
                        runtime = New-CanonicalRuntimeWasm
                        permissions = @()
                    }
                }
                "devtools_native" {
                    return [ordered]@{
                        dependencies = @(New-FamilyDependency "com.sylian.framework.devtools_native")
                        uses = @("framework.devtools_native")
                        capabilities = @("native.sdk.v1", "hooks.extended.v1")
                        runtime = New-CanonicalRuntimeHybrid "sdk"
                        permissions = @("nativeinterop")
                    }
                }
            }
        }
    }

    return $null
}

function Apply-InitFamilyPreset {
    param(
        [string]$Dest,
        [string]$TemplateName,
        [string]$FamilyName,
        [string]$ModId,
        [string]$ModName,
        [string]$VersionValue
    )

    $manifestPath = Join-Path $Dest "mod.json"
    if (-not (Test-Path $manifestPath)) {
        return
    }

    $preset = Get-CanonicalFamilyPreset -TemplateName $TemplateName -FamilyName $FamilyName
    $defaultProvide = "framework." + (($ModId.Split(".") | Select-Object -Last 1) -replace "[^a-z0-9_]+", "_")

    if ($TemplateName -eq "framework") {
        $provide = if ($null -ne $preset) { "$($preset.provide)" } else { $defaultProvide }
        $caps = @($(if ($null -ne $preset) { $preset.capabilities } else { "native.sdk.v1" }))
        $permissions = @($(if ($null -ne $preset) { $preset.permissions } else { "nativeinterop" }))
        $runtime = if ($null -ne $preset) { $preset.runtime } else { New-CanonicalRuntimeHybrid "sdk" }

        $manifest = [ordered]@{
            id = $ModId
            name = $ModName
            version = $VersionValue
            type = "framework"
            apiVersion = 4
            uiCategory = "core_api"
            loadPriority = 400
            entryScript = "scripts/init.json"
            runtime = $runtime
            itemDefinitions = "items/items.json"
            inputDefinitions = "config/input.json"
            engineVersionRange = ">=9.1.2 <10.0.0"
            releaseChannels = @("stable")
            capabilities = $caps
            capabilityRationales = (New-CanonicalCapabilityRationales -ModName $ModName -Capabilities $caps)
            permissions = $permissions
            permissionRationales = (New-CanonicalPermissionRationales -ModName $ModName -Permissions $permissions)
            dependencies = @()
            provides = @($provide)
            uses = @()
            exports = [ordered]@{ framework = @($provide); capabilities = $caps }
            manifestNotes = "Use -Family render_world|narrative|persistence|ui_inventory|combat_ai|devtools_native to stamp a canonical preset."
        }
    } elseif ($TemplateName -eq "content") {
        $caps = @($(if ($null -ne $preset) { $preset.capabilities } else { @() }))
        $permissions = @($(if ($null -ne $preset) { $preset.permissions } else { @() }))
        $runtime = if ($null -ne $preset) { $preset.runtime } else { New-CanonicalRuntimeWasm }
        $uses = @($(if ($null -ne $preset) { $preset.uses } else { @() }))
        $dependencies = @($(if ($null -ne $preset) { $preset.dependencies } else { @() }))

        $manifest = [ordered]@{
            id = $ModId
            name = $ModName
            version = $VersionValue
            type = "content"
            apiVersion = 4
            uiCategory = "mod"
            loadPriority = 300
            entryScript = "scripts/init.json"
            runtime = $runtime
            itemDefinitions = "items/items.json"
            inputDefinitions = "config/input.json"
            engineVersionRange = ">=9.1.2 <10.0.0"
            releaseChannels = @("stable")
            capabilities = $caps
            capabilityRationales = (New-CanonicalCapabilityRationales -ModName $ModName -Capabilities $caps)
            permissions = $permissions
            permissionRationales = (New-CanonicalPermissionRationales -ModName $ModName -Permissions $permissions)
            dependencies = $dependencies
            uses = $uses
            exports = [ordered]@{}
            manifestNotes = "Replace the example dependency with the framework family you want, or use -Family render_world|narrative|persistence|ui_inventory|combat_ai|devtools_native."
        }
    } else {
        return
    }

    Write-JsonFile -JsonPath $manifestPath -Object $manifest
    Write-JsonFile -JsonPath (Join-Path $Dest "manifest.lock.json") -Object (New-InitLockfile -ModId $ModId -Version $VersionValue -Dependencies $manifest.dependencies)
}

function Get-CanonicalCapabilityExpectations {
    return @(
        "native.sdk.v1",
        "native.raw_cpp.v1",
        "assets.raw.v1",
        "assets.packs.v2",
        "render.meshes.v1",
        "actors.prefabs.v1",
        "world.instances.v1",
        "editor.placement.v1",
        "world.authoring.v1",
        "render.materials.v1",
        "render.pbr.v1",
        "render.lighting.v1",
        "render.postfx.v1",
        "world.scenes.v1",
        "world.rooms.v1",
        "debug.render_inspector.v1",
        "narrative.flags.v1",
        "narrative.dialogue.v1",
        "narrative.quests.v1",
        "narrative.timeline.v1",
        "debug.narrative_inspector.v1",
        "world.persistence.v1",
        "world.storage.v1",
        "world.spawn_profiles.v1",
        "world.time_weather.v1",
        "world.seeding.v1",
        "world.migrations.v1",
        "debug.persistence_inspector.v1",
        "ui.runtime.v1",
        "ui.hud.v1",
        "ui.inventory_ext.v1",
        "containers.v1",
        "recipes.processing.v1",
        "interactions.v1",
        "actors.archetypes.v1",
        "actors.adapters.v1",
        "ai.behavior_trees.v1",
        "ai.sensors.v1",
        "nav.routes.v1",
        "nav.navmesh_bridge.v1",
        "debug.overlay.v1",
        "statuses.catalog.v1",
        "combat.targeting.v1",
        "combat.aoe.v1",
        "fx.presets.v1",
        "states.catalog.v1",
        "spells.catalog.v1",
        "items.use_profiles.v1",
        "dev.hot_reload.v1",
        "dev.console.v1",
        "dev.watchers.v1",
        "wasm.sandbox.v2",
        "debug.reload_inspector.v1",
        "hooks.extended.v1",
        "actors.generic.v1",
        "actors.vm.v1",
        "input.bindings.v2",
        "player.resources.v1",
        "ui.resource_rings.v1",
        "player.consumables.v1",
        "world.forage.v1",
        "combat.damage.v1",
        "movement.profiles.v1",
        "camera.aim_profiles.v2",
        "world.queries.v1",
        "behaviors.graph.v1"
    )
}

function New-DeterministicZip([string]$Root, [string]$OutputPath) {
    if (Test-Path $OutputPath) { Remove-Item -Path $OutputPath -Force }
    $outParent = Split-Path -Path $OutputPath -Parent
    if (-not (Test-Path $outParent)) { New-Item -ItemType Directory -Path $outParent -Force | Out-Null }

    $fixed = [DateTimeOffset]::new(2000, 1, 1, 0, 0, 0, [TimeSpan]::Zero)
    $files = Get-ChildItem -Path $Root -Recurse -File | ForEach-Object { $_.FullName } | Sort-Object
    $stream = [System.IO.File]::Open($OutputPath, [System.IO.FileMode]::CreateNew)
    try {
        $zip = New-Object System.IO.Compression.ZipArchive($stream, [System.IO.Compression.ZipArchiveMode]::Create, $false)
        try {
            foreach ($file in $files) {
                $entryName = Get-Rel -Root $Root -FullPath $file
                $entry = $zip.CreateEntry($entryName, [System.IO.Compression.CompressionLevel]::Optimal)
                $entry.LastWriteTime = $fixed
                $out = $entry.Open()
                try {
                    $in = [System.IO.File]::OpenRead($file)
                    try { $in.CopyTo($out) } finally { $in.Dispose() }
                } finally { $out.Dispose() }
            }
        } finally { $zip.Dispose() }
    } finally { $stream.Dispose() }
}

function Invoke-InitCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $templatesRoot = Join-Path $ResolvedRoot "tools/external_mods/templates"
    $templateDir = switch ($Template) {
        "framework" { Join-Path $templatesRoot "framework_mod" }
        "content" { Join-Path $templatesRoot "content_mod" }
        "pack" { Join-Path $templatesRoot "pack" }
        "native" { Join-Path $templatesRoot "native_mod" }
    }
    if (-not (Test-Path $templateDir)) {
        Add-Error $Report "Template missing: $templateDir"
        return
    }
    $dest = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path)) { $dest = Join-Path $ResolvedRoot ("new_" + $Template + "_mod") }
    if (Test-Path $dest) {
        Add-Error $Report "Target already exists: $dest"
        return
    }
    $modName = if ([string]::IsNullOrWhiteSpace($Name)) { Split-Path -Path $dest -Leaf } else { $Name }
    $modId = if ([string]::IsNullOrWhiteSpace($Id)) { "com.example." + ($modName.ToLowerInvariant() -replace "[^a-z0-9]+", "_").Trim("_") } else { $Id }
    $supportedFamilies = @("render_world", "narrative", "persistence", "ui_inventory", "combat_ai", "devtools_native")
    if (-not [string]::IsNullOrWhiteSpace($Family) -and ($supportedFamilies -notcontains $Family.ToLowerInvariant())) {
        Add-Error $Report "Unsupported -Family '$Family'. Expected one of: $($supportedFamilies -join ', ')"
        return
    }
    if (-not $DryRun) {
        New-Item -ItemType Directory -Path $dest -Force | Out-Null
        Copy-Item -Path (Join-Path $templateDir "*") -Destination $dest -Recurse -Force
        Get-ChildItem -Path $dest -Recurse -File | ForEach-Object {
            $ext = $_.Extension.ToLowerInvariant()
            if (@(".json", ".md", ".txt", ".wat", ".toml", ".cpp", ".c", ".h", ".hpp", ".cmake") -contains $ext -or $_.Name -eq "CMakeLists.txt") {
                $content = Get-Content -Path $_.FullName -Raw -Encoding UTF8
                $content = $content.Replace("__MOD_ID__", $modId).Replace("__MOD_NAME__", $modName).Replace("__VERSION__", $Version)
                Set-Content -Path $_.FullName -Value $content -Encoding UTF8
            }
        }
        Apply-InitFamilyPreset -Dest $dest -TemplateName $Template -FamilyName $Family -ModId $modId -ModName $modName -VersionValue $Version
    }
    $Report.data.targetPath = $dest
    $Report.data.modId = $modId
    $Report.data.modName = $modName
    $Report.data.family = $Family
}

function Invoke-ValidateCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $target = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path)) { $target = Join-Path $ResolvedRoot "docs/examples/external_mods" }
    if (-not (Test-Path $target)) { Add-Error $Report "Target not found: $target"; return }

    $validateScript = Join-Path $ResolvedRoot "tools/external_mods/validate_mod.ps1"
    $mods = @(Get-ModDirs -Target $target)
    foreach ($modDir in $mods) {
        $manifest = Read-JsonFile -JsonPath (Get-ModManifestPath -Target $modDir)
        if ($null -eq $manifest) { Add-Error $Report "[validate] invalid mod.json: $modDir"; continue }
        $modId = if ($manifest.id) { "$($manifest.id)" } else { Split-Path -Path $modDir -Leaf }
        if ($manifest.PSObject.Properties.Name -contains "releaseChannels") {
            $channels = @($manifest.releaseChannels | ForEach-Object { "$_".ToLowerInvariant() })
            if (@($channels | Where-Object { $_ -ne "stable" }).Count -gt 0) {
                Add-Warn $Report "[validate][$modId] non-stable releaseChannels are gated by UI until v1.0"
            }
        }
        $declaredPermissions = @()
        if ($manifest.PSObject.Properties.Name -contains "permissions") {
            $declaredPermissions = @($manifest.permissions | ForEach-Object { "$_".ToLowerInvariant() })
        }
        if ($declaredPermissions.Count -gt 0) {
            $permissionRationales = $null
            if ($manifest.PSObject.Properties.Name -contains "permissionRationales") {
                $permissionRationales = $manifest.permissionRationales
            }
            foreach ($permission in $declaredPermissions) {
                $hasRationale = $false
                if ($null -ne $permissionRationales -and
                    $permissionRationales.PSObject.Properties.Name -contains $permission -and
                    -not [string]::IsNullOrWhiteSpace("$($permissionRationales.$permission)")) {
                    $hasRationale = $true
                }
                if (-not $hasRationale) {
                    Add-Warn $Report "[validate][$modId] missing permission rationale for $permission"
                }
            }
        }
        if (Test-Path $validateScript) {
            & $validateScript -RepoRoot $ResolvedRoot -ModPath $modDir -VerboseOutput:$true *> $null
            if ($LASTEXITCODE -ne 0) {
                Add-Error $Report "[validate][$modId] validate_mod failed"
            }
        }
    }

    $packs = @(Get-PackDirs -Target $target)
    foreach ($packDir in $packs) {
        $pack = Read-JsonFile -JsonPath (Join-Path $packDir "pack.json")
        if ($null -eq $pack) { Add-Error $Report "[validate] invalid pack.json: $packDir"; continue }
        $packId = if ($pack.id) { "$($pack.id)" } else { Split-Path -Path $packDir -Leaf }
        foreach ($field in @("id", "name", "version")) {
            if (-not ($pack.PSObject.Properties.Name -contains $field) -or [string]::IsNullOrWhiteSpace("$($pack.$field)")) {
                Add-Error $Report "[validate][$packId] missing field $field"
            }
        }
        foreach ($bad in @("runtime", "entryScript", "dependencies")) {
            if ($pack.PSObject.Properties.Name -contains $bad) {
                Add-Error $Report "[validate][$packId] packs are assets-only; field '$bad' is not allowed"
            }
        }
    }

    $Report.data.targetPath = $target
    $Report.data.modCount = $mods.Count
    $Report.data.packCount = $packs.Count
}

function Invoke-PackCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $target = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path) -or -not (Test-Path $target)) { Add-Error $Report "pack requires valid -Path"; return }
    $outBase = if ([string]::IsNullOrWhiteSpace($OutDir)) { Join-Path $ResolvedRoot "build/modtool" } else { Resolve-Abs -Base $ResolvedRoot -Child $OutDir }

    $modManifestPath = Get-ModManifestPath -Target $target
    $packManifestPath = Join-Path $target "pack.json"
    if ((Test-Path $modManifestPath) -and (Test-Path $packManifestPath)) { Add-Error $Report "Path contains both mod.json and pack.json"; return }

    if (Test-Path $modManifestPath) {
        $manifest = Read-JsonFile -JsonPath $modManifestPath
        if ($null -eq $manifest) { Add-Error $Report "Invalid mod manifest"; return }
        $modId = "$($manifest.id)"; $modVersion = "$($manifest.version)"
        if ([string]::IsNullOrWhiteSpace($modId) -or [string]::IsNullOrWhiteSpace($modVersion)) { Add-Error $Report "mod.json needs id/version"; return }

        $manifestChannels = @("stable")
        if ($manifest.PSObject.Properties.Name -contains "releaseChannels") {
            $manifestChannels = @($manifest.releaseChannels | ForEach-Object { "$_".ToLowerInvariant() })
            if ($manifestChannels.Count -eq 0) {
                $manifestChannels = @("stable")
            }
        }
        if (-not ($manifestChannels -contains $ReleaseChannel)) {
            Add-Error $Report "Requested release channel '$ReleaseChannel' is not declared by mod manifest (releaseChannels)."
            return
        }

        $lockPath = Join-Path $target "manifest.lock.json"
        $lock = [ordered]@{
            schemaVersion = "mod.lockfile.v1"
            generatedUtc = $DeterministicTimestampIso
            modId = $modId
            version = $modVersion
            releaseChannel = $ReleaseChannel
            resolvedDependencies = if ($manifest.PSObject.Properties.Name -contains "dependencies") { @($manifest.dependencies) } else { @() }
            resolvedLoadOrder = @($modId)
        }
        if (-not $DryRun) { Set-Content -Path $lockPath -Value ($lock | ConvertTo-Json -Depth 16) -Encoding UTF8 }
        $artifactSuffix = if ($ReleaseChannel -eq "stable") { "" } else { "_" + $ReleaseChannel }
        $artifact = Join-Path $outBase (("{0}_{1}{2}.zip" -f ($modId -replace "[^a-zA-Z0-9._-]", "_"), $modVersion, $artifactSuffix))
        $packRoot = $target
        $manifestDataRoot = Get-ModDataRoot -Target $target -ManifestPath $modManifestPath
        $manifestRuntimeRoot = Get-ModRuntimeRoot -Target $target -DataRoot $manifestDataRoot
        $requiresSplitPack = ($modManifestPath -like "*\data\mod.json") -or ((Split-Path -Path (Split-Path -Path $manifestDataRoot -Parent) -Leaf).ToLowerInvariant() -eq "mods_data")
        $tempPackRoot = ""
        if ($requiresSplitPack -and -not $DryRun) {
            $tempPackRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("modpack_" + [Guid]::NewGuid().ToString("N"))
            New-Item -ItemType Directory -Path (Join-Path $tempPackRoot "data") -Force | Out-Null
            Copy-Item -Path (Join-Path $manifestDataRoot "*") -Destination (Join-Path $tempPackRoot "data") -Recurse -Force
            if (-not [string]::IsNullOrWhiteSpace($manifestRuntimeRoot) -and (Test-Path $manifestRuntimeRoot)) {
                New-Item -ItemType Directory -Path (Join-Path $tempPackRoot "runtime") -Force | Out-Null
                Copy-Item -Path (Join-Path $manifestRuntimeRoot "*") -Destination (Join-Path $tempPackRoot "runtime") -Recurse -Force
            }
            $packRoot = $tempPackRoot
        }
        if (-not $DryRun) {
            try {
                New-DeterministicZip -Root $packRoot -OutputPath $artifact
            } finally {
                if (-not [string]::IsNullOrWhiteSpace($tempPackRoot) -and (Test-Path $tempPackRoot)) {
                    Remove-Item -Path $tempPackRoot -Recurse -Force
                }
            }
        }
        $Report.data.packageType = "mod"; $Report.data.artifactPath = $artifact; $Report.data.lockfilePath = $lockPath
        $Report.data.releaseChannel = $ReleaseChannel
        $Report.data.packRoot = $packRoot
        $Report.data.includePdb = [bool]$IncludePdb
        if ($IncludePdb) {
            $nativePdbs = @(Get-ChildItem -Path $packRoot -Recurse -File -Filter *.pdb -ErrorAction SilentlyContinue)
            if ($nativePdbs.Count -eq 0) {
                Add-Warn $Report "[pack][$modId] -IncludePdb requested but no .pdb files were found under the mod directory"
            }
        }
        if (-not $DryRun -and (Test-Path $artifact)) { $Report.data.artifactSha256 = (Get-FileHash -Path $artifact -Algorithm SHA256).Hash.ToLowerInvariant() }
        return
    }

    if (Test-Path $packManifestPath) {
        $pack = Read-JsonFile -JsonPath $packManifestPath
        if ($null -eq $pack) { Add-Error $Report "Invalid pack manifest"; return }
        $packId = "$($pack.id)"; $packVersion = "$($pack.version)"
        if ([string]::IsNullOrWhiteSpace($packId) -or [string]::IsNullOrWhiteSpace($packVersion)) { Add-Error $Report "pack.json needs id/version"; return }
        $artifact = Join-Path $outBase (("{0}_{1}.o2r" -f ($packId -replace "[^a-zA-Z0-9._-]", "_"), $packVersion))
        if (-not $DryRun) { New-DeterministicZip -Root $target -OutputPath $artifact }
        $Report.data.packageType = "pack"; $Report.data.artifactPath = $artifact
        if (-not $DryRun -and (Test-Path $artifact)) { $Report.data.artifactSha256 = (Get-FileHash -Path $artifact -Algorithm SHA256).Hash.ToLowerInvariant() }
        return
    }

    Add-Error $Report "Path does not contain mod.json or pack.json"
}

function Invoke-SignCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $artifact = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path) -or -not (Test-Path $artifact)) { Add-Error $Report "sign requires valid artifact path"; return }
    $hash = (Get-FileHash -Path $artifact -Algorithm SHA256).Hash.ToLowerInvariant()
    $sig = [ordered]@{ schemaVersion = "security.signing.v1"; artifactPath = $artifact; artifactSha256 = $hash; signedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ"); method = "hash-only"; signer = "local-placeholder" }
    $sigPath = "$artifact.sig.json"
    if (-not $DryRun) { Set-Content -Path $sigPath -Value ($sig | ConvertTo-Json -Depth 16) -Encoding UTF8 }
    $Report.data.signaturePath = $sigPath
}

function Invoke-DoctorCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $target = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path)) { $target = Join-Path $ResolvedRoot "docs/examples/external_mods" }
    if (-not (Test-Path $target)) { Add-Error $Report "Target not found: $target"; return }
    $mods = @()
    foreach ($modDir in @(Get-ModDirs -Target $target)) {
        $m = Read-JsonFile -JsonPath (Get-ModManifestPath -Target $modDir)
        if ($null -eq $m) { continue }
        $mods += [PSCustomObject]@{ id = "$($m.id)"; dir = $modDir; manifest = $m }
    }
    ($mods | Group-Object -Property id | Where-Object { $_.Count -gt 1 -and -not [string]::IsNullOrWhiteSpace($_.Name) }) | ForEach-Object {
        Add-Error $Report "[doctor] duplicate mod id '$($_.Name)'"
    }
    $known = @{}
    $mods | ForEach-Object { if (-not [string]::IsNullOrWhiteSpace($_.id)) { $known[$_.id] = $true } }
    foreach ($mod in $mods) {
        $manifestType = if ($mod.manifest.PSObject.Properties.Name -contains "type") { "$($mod.manifest.type)".ToLowerInvariant() } else { "" }
        if ([string]::IsNullOrWhiteSpace($manifestType)) {
            Add-Error $Report "[doctor][$($mod.id)] missing field 'type'"
        }
        $releaseChannels = @("stable")
        if ($mod.manifest.PSObject.Properties.Name -contains "releaseChannels") {
            $releaseChannels = @($mod.manifest.releaseChannels | ForEach-Object { "$_".ToLowerInvariant() })
            if ($releaseChannels.Count -eq 0) {
                $releaseChannels = @("stable")
            }
        }
        if ($releaseChannels -contains "stable") {
            $lockPath = Join-Path $mod.dir "manifest.lock.json"
            if (-not (Test-Path $lockPath)) {
                Add-Error $Report "[doctor][$($mod.id)] stable mod is missing manifest.lock.json"
            }
        }
        if ($manifestType -eq "framework") {
            if (-not ($mod.manifest.PSObject.Properties.Name -contains "provides") -or @($mod.manifest.provides).Count -eq 0) {
                Add-Error $Report "[doctor][$($mod.id)] framework is missing provides[]"
            }
            $hasExports = ($mod.manifest.PSObject.Properties.Name -contains "exports") -and $null -ne $mod.manifest.exports -and @($mod.manifest.exports.PSObject.Properties).Count -gt 0
            if (-not $hasExports) {
                Add-Error $Report "[doctor][$($mod.id)] framework is missing exports{}"
            }
        }
        if (@("framework", "content") -contains $manifestType -and -not ($mod.manifest.PSObject.Properties.Name -contains "uses")) {
            Add-Warn $Report "[doctor][$($mod.id)] framework/content mod should declare uses[]"
        }
        $declaredCapabilities = @()
        if ($mod.manifest.PSObject.Properties.Name -contains "capabilities") {
            $declaredCapabilities = @($mod.manifest.capabilities | ForEach-Object { "$_".ToLowerInvariant() })
        }
        if ($declaredCapabilities.Count -gt 0) {
            $rationales = $null
            if ($mod.manifest.PSObject.Properties.Name -contains "capabilityRationales") {
                $rationales = $mod.manifest.capabilityRationales
            }
            foreach ($capability in $declaredCapabilities) {
                if ($null -eq $rationales -or -not ($rationales.PSObject.Properties.Name -contains $capability) -or [string]::IsNullOrWhiteSpace("$($rationales.$capability)")) {
                    Add-Warn $Report "[doctor][$($mod.id)] missing capability rationale for '$capability'"
                }
            }
        }
        $declaredPermissions = @()
        if ($mod.manifest.PSObject.Properties.Name -contains "permissions") {
            $declaredPermissions = @($mod.manifest.permissions | ForEach-Object { "$_".ToLowerInvariant() })
        }
        if ($declaredPermissions.Count -gt 0) {
            $permissionRationales = $null
            if ($mod.manifest.PSObject.Properties.Name -contains "permissionRationales") {
                $permissionRationales = $mod.manifest.permissionRationales
            }
            foreach ($permission in $declaredPermissions) {
                if ($null -eq $permissionRationales -or -not ($permissionRationales.PSObject.Properties.Name -contains $permission) -or [string]::IsNullOrWhiteSpace("$($permissionRationales.$permission)")) {
                    Add-Warn $Report "[doctor][$($mod.id)] missing permission rationale for '$permission'"
                }
            }
        }
        if ($mod.manifest.PSObject.Properties.Name -contains "dependencies") {
            foreach ($dep in @($mod.manifest.dependencies)) {
                $depId = if ($dep -is [string]) { $dep } elseif ($dep.PSObject.Properties.Name -contains "id") { "$($dep.id)" } elseif ($dep.PSObject.Properties.Name -contains "modId") { "$($dep.modId)" } else { "" }
                if ($dep -is [string]) {
                    Add-Warn $Report "[doctor][$($mod.id)] dependency '$depId' uses legacy string form; use { id, versionRange }"
                } elseif ($dep -ne $null -and $dep.PSObject -ne $null -and $dep.PSObject.Properties.Name -contains "modId") {
                    Add-Warn $Report "[doctor][$($mod.id)] dependency '$depId' uses legacy modId field; use id"
                }
                if (-not [string]::IsNullOrWhiteSpace($depId) -and -not $known.ContainsKey($depId)) {
                    Add-Warn $Report "[doctor][$($mod.id)] missing dependency '$depId'"
                }
            }
        }
    }
    $orderedMods = @($mods | Sort-Object -Property @{ Expression = {
                if ($_.manifest.PSObject.Properties.Name -contains "loadPriority" -and $null -ne $_.manifest.loadPriority) {
                    [int]$_.manifest.loadPriority
                } else {
                    0
                }
            }; Descending = $true },
                                         @{ Expression = { $_.id }; Descending = $false })
    $Report.data.loadOrder = @()
    $index = 0
    foreach ($mod in $orderedMods) {
        $depIds = @()
        if ($mod.manifest.PSObject.Properties.Name -contains "dependencies") {
            foreach ($dep in @($mod.manifest.dependencies)) {
                if ($dep -is [string]) {
                    if (-not [string]::IsNullOrWhiteSpace($dep)) { $depIds += "$dep" }
                } elseif ($dep -ne $null -and $dep.PSObject -ne $null) {
                    if ($dep.PSObject.Properties.Name -contains "id" -and -not [string]::IsNullOrWhiteSpace("$($dep.id)")) {
                        $depIds += "$($dep.id)"
                    } elseif ($dep.PSObject.Properties.Name -contains "modId" -and -not [string]::IsNullOrWhiteSpace("$($dep.modId)")) {
                        $depIds += "$($dep.modId)"
                    }
                }
            }
        }
        $resolvedPriority = if ($mod.manifest.PSObject.Properties.Name -contains "loadPriority" -and $null -ne $mod.manifest.loadPriority) {
            [int]$mod.manifest.loadPriority
        } else {
            0
        }
        $Report.data.loadOrder += [ordered]@{
            rank = $index
            modId = $mod.id
            loadPriority = $resolvedPriority
            dependencies = @($depIds)
            reason = "Sorted by loadPriority (desc), then mod id (asc)."
        }
        $index++
    }

    $registryOwners = @{}
    foreach ($mod in $mods) {
        if (-not ($mod.manifest.PSObject.Properties.Name -contains "exports")) { continue }
        $exports = $mod.manifest.exports
        if ($null -eq $exports -or $exports -isnot [System.Management.Automation.PSCustomObject]) { continue }
        foreach ($namespaceProp in $exports.PSObject.Properties) {
            $namespaceName = "$($namespaceProp.Name)"
            if ($namespaceName -eq "capabilities") {
                continue
            }
            $namespaceValue = $namespaceProp.Value
            $keys = @()
            if ($namespaceValue -is [System.Collections.IEnumerable] -and $namespaceValue -isnot [string]) {
                foreach ($keyValue in @($namespaceValue)) {
                    if ($null -ne $keyValue -and -not [string]::IsNullOrWhiteSpace("$keyValue")) {
                        $keys += "$keyValue"
                    }
                }
            } elseif ($namespaceValue -is [System.Management.Automation.PSCustomObject]) {
                foreach ($keyProp in $namespaceValue.PSObject.Properties) {
                    if (-not [string]::IsNullOrWhiteSpace("$($keyProp.Name)")) {
                        $keys += "$($keyProp.Name)"
                    }
                }
            } elseif ($null -ne $namespaceValue -and -not [string]::IsNullOrWhiteSpace("$namespaceValue")) {
                $keys += "$namespaceValue"
            }
            foreach ($key in $keys) {
                $registryKey = "$namespaceName::$key"
                if (-not $registryOwners.ContainsKey($registryKey)) { $registryOwners[$registryKey] = @() }
                $registryOwners[$registryKey] += $mod.id
            }
        }
    }

    $registryConflicts = @()
    foreach ($entry in $registryOwners.GetEnumerator()) {
        $owners = @($entry.Value | Sort-Object -Unique)
        if ($owners.Count -gt 1) {
            $registryConflicts += [ordered]@{
                key = $entry.Key
                owners = $owners
                winnerRule = "Highest loadPriority then lexicographically smallest mod id."
            }
            Add-Warn $Report "[doctor] registry conflict '$($entry.Key)' exported by: $($owners -join ', ')"
        }
    }
    $Report.data.registryConflicts = $registryConflicts

    $capabilityOwners = @{}
    foreach ($mod in $mods) {
        if (-not ($mod.manifest.PSObject.Properties.Name -contains "capabilities")) { continue }
        foreach ($capability in @($mod.manifest.capabilities)) {
            $capabilityKey = "$capability".ToLowerInvariant()
            if ([string]::IsNullOrWhiteSpace($capabilityKey)) { continue }
            if (-not $capabilityOwners.ContainsKey($capabilityKey)) { $capabilityOwners[$capabilityKey] = @() }
            $capabilityOwners[$capabilityKey] += $mod.id
        }
    }

    $coverageGaps = @()
    foreach ($expectedCapability in @(Get-CanonicalCapabilityExpectations)) {
        if (-not $capabilityOwners.ContainsKey($expectedCapability)) {
            $coverageGaps += $expectedCapability
            Add-Warn $Report "[doctor] canonical capability '$expectedCapability' is not covered by the active catalog"
        }
    }
    $Report.data.capabilityCoverage = [ordered]@{
        owners = $capabilityOwners
        expected = @(Get-CanonicalCapabilityExpectations)
        missing = $coverageGaps
    }

    $packs = @()
    foreach ($packDir in @(Get-PackDirs -Target $target)) {
        $pack = Read-JsonFile -JsonPath (Join-Path $packDir "pack.json")
        if ($null -eq $pack) { continue }
        $packId = if ($pack.id) { "$($pack.id)" } else { Split-Path -Path $packDir -Leaf }
        $packs += [PSCustomObject]@{ id = $packId; dir = $packDir; manifest = $pack }
    }

    $assetOwners = @{}
    foreach ($pack in $packs) {
        $assetFiles = @(Get-ChildItem -Path $pack.dir -Recurse -File | Where-Object {
                $_.Name -ne "pack.json" -and $_.FullName -notmatch "\\.git\\"
            })
        foreach ($assetFile in $assetFiles) {
            $relative = Get-Rel -Root $pack.dir -FullPath $assetFile.FullName
            if (-not $assetOwners.ContainsKey($relative)) { $assetOwners[$relative] = @() }
            $assetOwners[$relative] += $pack.id
        }
    }

    $packConflicts = @()
    foreach ($entry in $assetOwners.GetEnumerator()) {
        $owners = @($entry.Value | Sort-Object -Unique)
        if ($owners.Count -gt 1) {
            $packConflicts += [ordered]@{
                relativePath = $entry.Key
                packs = $owners
                winnerRule = "User pack load order in mod menu decides final winner."
            }
            Add-Warn $Report "[doctor] pack override collision '$($entry.Key)' present in: $($owners -join ', ')"
        }
    }
    $Report.data.packOverrideConflicts = $packConflicts
    $Report.data.targetPath = $target
    $Report.data.modCount = $mods.Count
    $Report.data.packCount = $packs.Count
}

function Invoke-DoctorNativeCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $target = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path)) { $target = Join-Path $ResolvedRoot "docs/examples/external_mods" }
    if (-not (Test-Path $target)) { Add-Error $Report "Target not found: $target"; return }

    $nativeMods = @()
    foreach ($modDir in @(Get-ModDirs -Target $target)) {
        $manifest = Read-JsonFile -JsonPath (Get-ModManifestPath -Target $modDir)
        if ($null -eq $manifest -or -not ($manifest.PSObject.Properties.Name -contains "runtime")) { continue }
        $runtimeType = if ($manifest.runtime.PSObject.Properties.Name -contains "type") { "$($manifest.runtime.type)".ToLowerInvariant() } else { "" }
        if (@("native-cpp-v1", "hybrid-v1") -notcontains $runtimeType) { continue }

        $entryLibrary = if ($manifest.runtime.PSObject.Properties.Name -contains "entryLibrary") { "$($manifest.runtime.entryLibrary)" } else { "" }
        $nativeMode = if ($manifest.runtime.PSObject.Properties.Name -contains "nativeMode") { "$($manifest.runtime.nativeMode)".ToLowerInvariant() } else { "sdk" }
        $permissions = @()
        if ($manifest.PSObject.Properties.Name -contains "permissions") {
            $permissions = @($manifest.permissions | ForEach-Object { "$_".ToLowerInvariant() })
        }
        $libraryPath = if ([string]::IsNullOrWhiteSpace($entryLibrary)) { "" } else { Join-Path $modDir $entryLibrary }
        $pdbPath = if ([string]::IsNullOrWhiteSpace($libraryPath)) { "" } else { [System.IO.Path]::ChangeExtension($libraryPath, ".pdb") }
        $hasPdb = (-not [string]::IsNullOrWhiteSpace($pdbPath)) -and (Test-Path $pdbPath)

        if ($permissions -notcontains "nativeinterop") {
            Add-Warn $Report "[doctor-native][$($manifest.id)] native runtime should declare permission 'nativeinterop'"
        }
        if ($nativeMode -eq "raw" -and $permissions -notcontains "nativeinterop.raw") {
            Add-Warn $Report "[doctor-native][$($manifest.id)] raw native runtime should declare permission 'nativeinterop.raw'"
        }
        if ([string]::IsNullOrWhiteSpace($entryLibrary)) {
            Add-Error $Report "[doctor-native][$($manifest.id)] runtime.entryLibrary is missing"
        } elseif (-not (Test-Path $libraryPath)) {
            Add-Error $Report "[doctor-native][$($manifest.id)] native library not found: $entryLibrary"
        } elseif ([System.IO.Path]::GetExtension($libraryPath).ToLowerInvariant() -ne ".dll") {
            Add-Error $Report "[doctor-native][$($manifest.id)] runtime.entryLibrary must end with .dll"
        }
        if (-not $hasPdb) {
            Add-Warn $Report "[doctor-native][$($manifest.id)] no PDB found next to native library"
        }

        $nativeMods += [ordered]@{
            modId = "$($manifest.id)"
            runtimeType = $runtimeType
            nativeMode = $nativeMode
            entryLibrary = $entryLibrary
            pdbPresent = $hasPdb
        }
    }

    $Report.data.nativeMods = $nativeMods
}

function Invoke-BuildSdkSampleCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $target = Resolve-Abs -Base $ResolvedRoot -Child $Path
    if ([string]::IsNullOrWhiteSpace($Path) -or -not (Test-Path $target)) {
        Add-Error $Report "build-sdk-sample requires -Path pointing to a native mod directory"
        return
    }

    $pluginRoot = Join-Path $target "native/plugin"
    $cmakeFile = Join-Path $pluginRoot "CMakeLists.txt"
    if (-not (Test-Path $cmakeFile)) {
        Add-Error $Report "Native sample project not found: $cmakeFile"
        return
    }
    if (-not (Get-Command cmake -ErrorAction SilentlyContinue)) {
        Add-Error $Report "cmake not found in PATH"
        return
    }

    $buildDir = Join-Path $pluginRoot "build"
    $configureArgs = @("-S", $pluginRoot, "-B", $buildDir)
    $buildArgs = @("--build", $buildDir, "--config", "Release")
    if (-not $DryRun) {
        & cmake @configureArgs *> $null
        if ($LASTEXITCODE -ne 0) {
            Add-Error $Report "cmake configure failed for native sample"
            return
        }
        & cmake @buildArgs *> $null
        if ($LASTEXITCODE -ne 0) {
            Add-Error $Report "cmake build failed for native sample"
            return
        }
    }

    $Report.data.nativeBuild = [ordered]@{
        pluginRoot = $pluginRoot
        buildDir = $buildDir
        outputDir = (Join-Path $target "native/win64")
    }
}

function Invoke-DiffCommand([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    if ([string]::IsNullOrWhiteSpace($ArtifactA) -or [string]::IsNullOrWhiteSpace($ArtifactB)) { Add-Error $Report "diff requires -ArtifactA and -ArtifactB"; return }
    $a = Resolve-Abs -Base $ResolvedRoot -Child $ArtifactA
    $b = Resolve-Abs -Base $ResolvedRoot -Child $ArtifactB
    if (-not (Test-Path $a) -or -not (Test-Path $b)) { Add-Error $Report "Artifacts not found"; return }
    $aHash = (Get-FileHash -Path $a -Algorithm SHA256).Hash.ToLowerInvariant()
    $bHash = (Get-FileHash -Path $b -Algorithm SHA256).Hash.ToLowerInvariant()
    $Report.data.artifactA = $a
    $Report.data.artifactB = $b
    $Report.data.shaA = $aHash
    $Report.data.shaB = $bHash
    $Report.data.changed = @($aHash -ne $bHash)
}

function Invoke-GenerateContractDocs([string]$ResolvedRoot, [System.Collections.IDictionary]$Report) {
    $generatorScript = Join-Path $ResolvedRoot "tools/external_mods/generate_contract_docs.ps1"
    if (-not (Test-Path $generatorScript)) {
        Add-Warn $Report "[report] docs generator not found: $generatorScript"
        return
    }

    & $generatorScript -RepoRoot $ResolvedRoot *> $null
    if ($LASTEXITCODE -ne 0) {
        Add-Warn $Report "[report] generate_contract_docs failed"
        return
    }

    $Report.data.generatedDocs = [ordered]@{
        outDir = "docs/generated"
        contractIndex = "docs/generated/contract_index.json"
        registryIndex = "docs/generated/registry_index.json"
    }
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$report = switch ($Command) {
    "validate" { New-Report -Schema "tool.validate_report.v1" }
    "doctor" { New-Report -Schema "tool.doctor_report.v1" }
    "doctor-native" { New-Report -Schema "tool.doctor_report.v1" }
    "diff" { New-Report -Schema "tool.diff_report.v1" }
    default { New-Report -Schema "tool.modtool_report.v1" }
}

switch ($Command) {
    "init" { Invoke-InitCommand -ResolvedRoot $resolvedRoot -Report $report }
    "init-native" { $Template = "native"; Invoke-InitCommand -ResolvedRoot $resolvedRoot -Report $report }
    "build-sdk-sample" { Invoke-BuildSdkSampleCommand -ResolvedRoot $resolvedRoot -Report $report }
    "validate" { Invoke-ValidateCommand -ResolvedRoot $resolvedRoot -Report $report }
    "pack" { Invoke-PackCommand -ResolvedRoot $resolvedRoot -Report $report }
    "sign" { Invoke-SignCommand -ResolvedRoot $resolvedRoot -Report $report }
    "sign-native" { Invoke-SignCommand -ResolvedRoot $resolvedRoot -Report $report }
    "doctor" { Invoke-DoctorCommand -ResolvedRoot $resolvedRoot -Report $report }
    "doctor-native" { Invoke-DoctorNativeCommand -ResolvedRoot $resolvedRoot -Report $report }
    "diff" { Invoke-DiffCommand -ResolvedRoot $resolvedRoot -Report $report }
    "report" {
        Invoke-ValidateCommand -ResolvedRoot $resolvedRoot -Report $report
        Invoke-DoctorCommand -ResolvedRoot $resolvedRoot -Report $report
        Invoke-GenerateContractDocs -ResolvedRoot $resolvedRoot -Report $report
    }
    "report-native" {
        Invoke-ValidateCommand -ResolvedRoot $resolvedRoot -Report $report
        Invoke-DoctorCommand -ResolvedRoot $resolvedRoot -Report $report
        Invoke-DoctorNativeCommand -ResolvedRoot $resolvedRoot -Report $report
        Invoke-GenerateContractDocs -ResolvedRoot $resolvedRoot -Report $report
    }
}

$outFile = ""
if (-not [string]::IsNullOrWhiteSpace($OutDir)) {
    $resolvedOut = Resolve-Abs -Base $resolvedRoot -Child $OutDir
    if (-not (Test-Path $resolvedOut)) { New-Item -ItemType Directory -Path $resolvedOut -Force | Out-Null }
    $outFile = Join-Path $resolvedOut (($Command + "_report.json"))
    Set-Content -Path $outFile -Value ($report | ConvertTo-Json -Depth 32) -Encoding UTF8
}

$report | ConvertTo-Json -Depth 32 | Write-Output
if ($report.success) { exit 0 } else { exit 1 }

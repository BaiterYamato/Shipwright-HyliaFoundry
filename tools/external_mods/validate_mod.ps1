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

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$resolvedModPath = if ([string]::IsNullOrWhiteSpace($ModPath)) {
    Join-Path $resolvedRoot "x64/Release/mods"
} else {
    if ([System.IO.Path]::IsPathRooted($ModPath)) { $ModPath } else { Join-Path $resolvedRoot $ModPath }
}

if (-not (Test-Path $resolvedModPath)) {
    Fail "ModPath not found: $resolvedModPath"
}

$mods = @()
if (Test-Path (Join-Path $resolvedModPath "mod.json")) {
    $mods = @((Resolve-Path $resolvedModPath).Path)
} else {
    $mods = @(Get-ChildItem -Path $resolvedModPath -Directory | Select-Object -ExpandProperty FullName)
}

if ($mods.Count -eq 0) {
    Fail "No mod directories found in: $resolvedModPath"
}

$capabilityPathMap = Resolve-CapabilityPathMap
$failed = 0

foreach ($modDir in $mods) {
    $manifestPath = Join-Path $modDir "mod.json"
    if (-not (Test-Path $manifestPath)) {
        Write-Warning "Skipping $modDir (missing mod.json)"
        continue
    }

    try {
        $manifest = Get-Content -Path $manifestPath -Raw | ConvertFrom-Json
    } catch {
        Write-Error "[validate_mod] $manifestPath parse error: $($_.Exception.Message)"
        $failed++
        continue
    }

    $modId = if ($manifest.id) { "$($manifest.id)" } else { Split-Path -Path $modDir -Leaf }
    if ($manifest.apiVersion -ne 4) {
        Write-Error "[validate_mod] $modId apiVersion must be 4 (found: $($manifest.apiVersion))"
        $failed++
        continue
    }

    $hasRuntime = ($manifest.PSObject.Properties.Name -contains "runtime") -and ($null -ne $manifest.runtime)
    $runtimeType = ""
    if ($hasRuntime -and ($manifest.runtime.PSObject.Properties.Name -contains "type")) {
        $runtimeType = "$($manifest.runtime.type)"
    }
    if (-not $hasRuntime -or $runtimeType -notin @("wasm3-v1", "hybrid-v1", "native-v1", "data-only")) {
        Write-Error "[validate_mod] $modId runtime.type must be wasm3-v1|hybrid-v1|native-v1|data-only"
        $failed++
        continue
    }

    if ($runtimeType -eq "data-only") {
        foreach ($jsonFile in @(Get-ChildItem -LiteralPath $modDir -File -Recurse -Filter "*.json")) {
            try { $null = Get-Content -LiteralPath $jsonFile.FullName -Raw | ConvertFrom-Json }
            catch {
                Write-Host "[validate_mod] $modId invalid json: $($jsonFile.FullName)" -ForegroundColor Red
                $failed++
            }
        }
        continue
    }

    $entryScript = ""
    if ($manifest.PSObject.Properties.Name -contains "entryScript") {
        $entryScript = "$($manifest.entryScript)"
    }
    if ([string]::IsNullOrWhiteSpace($entryScript)) {
        Write-Error "[validate_mod] $modId missing entryScript"
        $failed++
        continue
    }

    $entryScriptPath = Join-Path $modDir $entryScript
    if (-not (Test-Path $entryScriptPath)) {
        Write-Error "[validate_mod] $modId missing entryScript file: $entryScript"
        $failed++
        continue
    }

    try {
        $entryJson = Get-Content -Path $entryScriptPath -Raw | ConvertFrom-Json
    } catch {
        Write-Error "[validate_mod] $modId invalid json in entryScript ${entryScript}: $($_.Exception.Message)"
        $failed++
        continue
    }

    if (-not ($entryJson.PSObject.Properties.Name -contains "apiVersion")) {
        Write-Error "[validate_mod] $modId entryScript missing apiVersion"
        $failed++
        continue
    }
    if ($entryJson.apiVersion -ne $manifest.apiVersion) {
        Write-Error "[validate_mod] $modId entryScript apiVersion mismatch (manifest=$($manifest.apiVersion), script=$($entryJson.apiVersion))"
        $failed++
        continue
    }

    $knownInputBindings = @()
    if (($manifest.PSObject.Properties.Name -contains "inputDefinitions") -and
        -not [string]::IsNullOrWhiteSpace("$($manifest.inputDefinitions)")) {
        $inputPath = Join-Path $modDir "$($manifest.inputDefinitions)"
        if (Test-Path -LiteralPath $inputPath) {
            try {
                $inputJson = Get-Content -LiteralPath $inputPath -Raw | ConvertFrom-Json
                $knownInputBindings = @($inputJson.bindings | ForEach-Object { "$($_.id)" } | Where-Object {
                    -not [string]::IsNullOrWhiteSpace($_)
                })
            }
            catch { }
        }
    }
    if ($entryJson.PSObject.Properties.Name -contains "onInput") {
        $inputIndex = 0
        foreach ($trigger in @($entryJson.onInput)) {
            $bindingId = if ($null -ne $trigger -and ($trigger.PSObject.Properties.Name -contains "binding")) {
                "$($trigger.binding)"
            }
            elseif ($null -ne $trigger -and ($trigger.PSObject.Properties.Name -contains "bindingId")) {
                "$($trigger.bindingId)"
            }
            else { "" }

            if (-not [string]::IsNullOrWhiteSpace($bindingId) -and $knownInputBindings -notcontains $bindingId) {
                Write-Host "[validate_mod] $modId onInput[$inputIndex] references unknown input binding: $bindingId" -ForegroundColor Red
                $failed++
            }
            $inputIndex++
        }
    }

    if ($manifest.PSObject.Properties.Name -contains "dependencies") {
        if ($null -eq $manifest.dependencies) {
            # accept null/empty
        } elseif ($manifest.dependencies -isnot [System.Collections.IEnumerable]) {
            Write-Error "[validate_mod] $modId dependencies must be array"
            $failed++
            continue
        } else {
            $depIndex = 0
            foreach ($dep in $manifest.dependencies) {
                $depId = ""
                $depRange = ""

                if ($dep -is [string]) {
                    $depId = $dep.Trim()
                } elseif ($dep -ne $null -and $dep.PSObject -ne $null) {
                    if ($dep.PSObject.Properties.Name -contains "id") { $depId = "$($dep.id)".Trim() }
                    if ([string]::IsNullOrWhiteSpace($depId) -and ($dep.PSObject.Properties.Name -contains "modId")) {
                        $depId = "$($dep.modId)".Trim()
                    }

                    if ($dep.PSObject.Properties.Name -contains "versionRange") {
                        $depRange = "$($dep.versionRange)"
                    } elseif ($dep.PSObject.Properties.Name -contains "version") {
                        $depRange = "$($dep.version)"
                    }
                } else {
                    Write-Error "[validate_mod] $modId dependencies[$depIndex] must be string or object"
                    $failed++
                    continue
                }

                if ([string]::IsNullOrWhiteSpace($depId)) {
                    Write-Error "[validate_mod] $modId dependencies[$depIndex] missing id/modId"
                    $failed++
                    continue
                }

                if (-not (Test-VersionRangeSyntax -Range $depRange)) {
                    Write-Error "[validate_mod] $modId dependencies[$depIndex] invalid versionRange syntax: '$depRange'"
                    $failed++
                    continue
                }

                $depIndex++
            }
        }
    }

    $caps = @()
    $capabilitiesValue = $null
    if ($manifest.PSObject.Properties.Name -contains "capabilities") {
        $capabilitiesValue = $manifest.capabilities
    }
    if ($capabilitiesValue -is [System.Collections.IEnumerable]) {
        foreach ($cap in $capabilitiesValue) {
            if ($null -ne $cap -and -not [string]::IsNullOrWhiteSpace("$cap")) {
                $caps += "$cap".ToLowerInvariant()
            }
        }
    }

    $hasCameraV1 = $caps -contains "camera.aim_profiles.v1"
    $hasCameraV2 = $caps -contains "camera.aim_profiles.v2"
    $hasCameraDefinitions = $manifest.PSObject.Properties.Name -contains "cameraDefinitions"
    if ($manifest.apiVersion -eq 4 -and $hasCameraV1) {
        Write-Error "[validate_mod] $modId capability camera.aim_profiles.v1 is legacy in apiVersion 4; use camera.aim_profiles.v2"
        $failed++
        continue
    }
    if ($manifest.apiVersion -eq 4 -and $hasCameraDefinitions -and -not $hasCameraV2) {
        Write-Error "[validate_mod] $modId cameraDefinitions requires capability camera.aim_profiles.v2"
        $failed++
        continue
    }

    foreach ($cap in $caps) {
        if (-not $capabilityPathMap.ContainsKey($cap)) {
            continue
        }

        $fieldName = $capabilityPathMap[$cap]
        if (-not ($manifest.PSObject.Properties.Name -contains $fieldName)) {
            Write-Error "[validate_mod] $modId capability $cap requires field $fieldName"
            $failed++
            continue
        }

        $relative = "$($manifest.$fieldName)"
        if ([string]::IsNullOrWhiteSpace($relative)) {
            Write-Error "[validate_mod] $modId field $fieldName is empty"
            $failed++
            continue
        }

        $absolute = Join-Path $modDir $relative
        if (-not (Test-Path $absolute)) {
            Write-Error "[validate_mod] $modId missing file for ${fieldName}: $relative"
            $failed++
            continue
        }

        $parsedCapabilityJson = $null
        if ($absolute.ToLowerInvariant().EndsWith(".json")) {
            try {
                $parsedCapabilityJson = Get-Content -Path $absolute -Raw | ConvertFrom-Json
            } catch {
                Write-Error "[validate_mod] $modId invalid json in ${relative}: $($_.Exception.Message)"
                $failed++
                continue
            }
        }

        if ($cap -eq "assets.raw.v1" -and $null -ne $parsedCapabilityJson -and
            ($parsedCapabilityJson.PSObject.Properties.Name -contains "sources")) {
            $sourceIndex = 0
            foreach ($source in @($parsedCapabilityJson.sources)) {
                $kind = if ($null -ne $source -and ($source.PSObject.Properties.Name -contains "kind")) {
                    "$($source.kind)".Trim().ToLowerInvariant()
                } else { "" }
                if ($kind -notin @("image", "audio", "model", "data", "archive")) {
                    Write-Host "[validate_mod] $modId ${relative} sources[$sourceIndex].kind '$kind' is not canonical; expected image|audio|model|data|archive" -ForegroundColor Red
                    $failed++
                }
                $sourceIndex++
            }
        }

        if ($VerboseOutput) {
            Write-Host "[validate_mod] $modId OK -> $cap ($relative)"
        }
    }

    if (($manifest.PSObject.Properties.Name -contains "effectGraphDefinitions") -and
        ($manifest.PSObject.Properties.Name -contains "combatHitRuleDefinitions")) {
        try {
            $graphsJson = Get-Content -LiteralPath (Join-Path $modDir "$($manifest.effectGraphDefinitions)") -Raw | ConvertFrom-Json
            $rulesJson = Get-Content -LiteralPath (Join-Path $modDir "$($manifest.combatHitRuleDefinitions)") -Raw | ConvertFrom-Json
            if ($graphsJson.schemaVersion -ne 1 -or $rulesJson.schemaVersion -ne 1) {
                throw "effect graphs and hit rules must use schemaVersion 1"
            }
            $graphIds = @($graphsJson.graphs | ForEach-Object { "$($_.id)" })
            foreach ($graph in @($graphsJson.graphs)) {
                $nodeIds = @($graph.nodes | ForEach-Object { "$($_.id)" })
                if ($nodeIds.Count -eq 0 -or $nodeIds.Count -gt 64 -or $nodeIds -notcontains "$($graph.entryNodeId)") {
                    throw "graph '$($graph.id)' must have 1..64 nodes and a valid entryNodeId"
                }
                foreach ($node in @($graph.nodes)) {
                    foreach ($next in @($node.next)) {
                        if ($nodeIds -notcontains "$next") { throw "graph '$($graph.id)' references unknown node '$next'" }
                    }
                }
            }
            foreach ($rule in @($rulesJson.rules)) {
                if ($graphIds -notcontains "$($rule.graphId)") {
                    throw "hit rule '$($rule.id)' references unknown graph '$($rule.graphId)'"
                }
                if ("$($rule.trigger)" -ne "hammer_ground_impact") {
                    throw "hit rule '$($rule.id)' has unsupported trigger '$($rule.trigger)'"
                }
            }
        }
        catch {
            Write-Host "[validate_mod] $modId M6 contract validation failed: $($_.Exception.Message)" -ForegroundColor Red
            $failed++
        }
    }

    if (($manifest.PSObject.Properties.Name -contains "lightingDefinitions") -and
        (($manifest.PSObject.Properties.Name -contains "sceneProfileDefinitions") -or
         ($manifest.PSObject.Properties.Name -contains "roomProfileDefinitions"))) {
        try {
            $lightingPath = Join-Path $modDir "$($manifest.lightingDefinitions)"
            $lightingJson = Get-Content -LiteralPath $lightingPath -Raw | ConvertFrom-Json
            $knownLightingIds = @($lightingJson.profiles | ForEach-Object { "$($_.id)" })

            foreach ($profileField in @("sceneProfileDefinitions", "roomProfileDefinitions")) {
                if (-not ($manifest.PSObject.Properties.Name -contains $profileField)) { continue }
                $profilePath = Join-Path $modDir "$($manifest.$profileField)"
                $profileJson = Get-Content -LiteralPath $profilePath -Raw | ConvertFrom-Json
                $profileIndex = 0
                foreach ($profile in @($profileJson.profiles)) {
                    if ($null -ne $profile -and ($profile.PSObject.Properties.Name -contains "skylightProfileId")) {
                        $lightingId = "$($profile.skylightProfileId)"
                        if (-not [string]::IsNullOrWhiteSpace($lightingId) -and $knownLightingIds -notcontains $lightingId) {
                            Write-Host "[validate_mod] $modId $profileField profiles[$profileIndex] references unknown skylightProfileId: $lightingId" -ForegroundColor Red
                            $failed++
                        }
                    }
                    $profileIndex++
                }
            }
        }
        catch {
            Write-Host "[validate_mod] $modId lighting reference validation failed: $($_.Exception.Message)" -ForegroundColor Red
            $failed++
        }
    }
}

if ($failed -gt 0) {
    Write-Error "validate_mod failed with $failed error(s)"
    exit 1
}

Write-Host "validate_mod passed ($($mods.Count) mod path(s))"
exit 0

param(
    [string]$SourceRoot = "docs/examples/external_mods",
    [string]$RuntimeRoot = "x64/Release",
    [switch]$Clean
)

$ErrorActionPreference = "Stop"

function Read-Manifest([string]$ManifestPath) {
    try {
        return Get-Content -Path $ManifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    } catch {
        Write-Warning "Invalid mod.json at $ManifestPath. $($_.Exception.Message)"
        return $null
    }
}

function Get-ManifestPath([string]$PackageDir) {
    $rootManifest = Join-Path $PackageDir "mod.json"
    if (Test-Path $rootManifest) { return $rootManifest }
    $splitManifest = Join-Path $PackageDir "data/mod.json"
    if (Test-Path $splitManifest) { return $splitManifest }
    return ""
}

function Get-BucketRoot {
    param(
        [Parameter(Mandatory = $true)]
        [object]$Manifest,
        [Parameter(Mandatory = $true)]
        [string]$FrameworksRoot,
        [Parameter(Mandatory = $true)]
        [string]$ModsRoot
    )

    $manifestType = if ($null -ne $Manifest -and $Manifest.PSObject.Properties.Name -contains "type") {
        "$($Manifest.type)".Trim().ToLowerInvariant()
    } else {
        ""
    }

    if ($manifestType -eq "framework") {
        return $FrameworksRoot
    }

    return $ModsRoot
}

if (-not (Test-Path $SourceRoot)) {
    throw "Source root not found: $SourceRoot"
}

$resolvedSourceRoot = (Resolve-Path $SourceRoot).Path
New-Item -ItemType Directory -Force -Path $RuntimeRoot | Out-Null

$legacyModsRoot = Join-Path $RuntimeRoot "mods"
$frameworksRoot = Join-Path $legacyModsRoot "frameworks"
$resourcePacksRoot = Join-Path $legacyModsRoot "resourcepacks"
$modsBucketRoot = Join-Path $legacyModsRoot "mods"
$runtimeDataRoot = Join-Path $RuntimeRoot "mods_data"
$runtimeBinaryRoot = Join-Path $RuntimeRoot "mods_runtime"

if ($Clean) {
    foreach ($bucket in @($frameworksRoot, $modsBucketRoot, $resourcePacksRoot, $runtimeDataRoot, $runtimeBinaryRoot)) {
        if (Test-Path $bucket) {
            Remove-Item -Recurse -Force $bucket
        }
    }
}

New-Item -ItemType Directory -Force -Path $frameworksRoot | Out-Null
New-Item -ItemType Directory -Force -Path $resourcePacksRoot | Out-Null
New-Item -ItemType Directory -Force -Path $modsBucketRoot | Out-Null
New-Item -ItemType Directory -Force -Path $runtimeDataRoot | Out-Null
New-Item -ItemType Directory -Force -Path $runtimeBinaryRoot | Out-Null

$modDirs = @(Get-ChildItem -Path $resolvedSourceRoot -Directory | Where-Object {
        -not [string]::IsNullOrWhiteSpace((Get-ManifestPath -PackageDir $_.FullName))
    } | Sort-Object Name)

foreach ($modDir in $modDirs) {
    $manifestPath = Get-ManifestPath -PackageDir $modDir.FullName
    $manifest = Read-Manifest -ManifestPath $manifestPath
    if ($null -eq $manifest) {
        continue
    }

    $modId = if ($manifest.PSObject.Properties.Name -contains "id") { "$($manifest.id)" } else { $modDir.Name }
    $isSplitPackage = $manifestPath -like "*\data\mod.json"

    if ($isSplitPackage) {
        $dataDestination = Join-Path $runtimeDataRoot $modDir.Name
        $runtimeDestination = Join-Path $runtimeBinaryRoot $modDir.Name

        if (Test-Path $dataDestination) { Remove-Item -Recurse -Force $dataDestination }
        if (Test-Path $runtimeDestination) { Remove-Item -Recurse -Force $runtimeDestination }

        New-Item -ItemType Directory -Force -Path $dataDestination | Out-Null
        Copy-Item -Recurse -Force (Join-Path $modDir.FullName "data\*") $dataDestination

        if (Test-Path (Join-Path $modDir.FullName "runtime")) {
            New-Item -ItemType Directory -Force -Path $runtimeDestination | Out-Null
            Copy-Item -Recurse -Force (Join-Path $modDir.FullName "runtime\*") $runtimeDestination
        }

        Write-Host "Synced $($modDir.Name) [split] -> $modId"
        continue
    }

    $bucketRoot = Get-BucketRoot -Manifest $manifest -FrameworksRoot $frameworksRoot -ModsRoot $modsBucketRoot
    $destination = Join-Path $bucketRoot $modDir.Name
    $legacyFlatDestination = Join-Path $legacyModsRoot $modDir.Name

    if (Test-Path $legacyFlatDestination) {
        Remove-Item -Recurse -Force $legacyFlatDestination
    }
    if (Test-Path $destination) {
        Remove-Item -Recurse -Force $destination
    }

    New-Item -ItemType Directory -Force -Path $destination | Out-Null
    Copy-Item -Recurse -Force (Join-Path $modDir.FullName "*") $destination

    $bucketName = Split-Path -Path $bucketRoot -Leaf
    Write-Host "Synced $($modDir.Name) [$bucketName] -> $modId"
}

Write-Host "External mod catalog synchronized."

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun,
    [switch]$SkipContentHash
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-ManifestPath([string]$PackageRoot) {
    $rootManifest = Join-Path $PackageRoot "mod.json"
    if (Test-Path -LiteralPath $rootManifest) { return $rootManifest }

    $splitManifest = Join-Path $PackageRoot "data\mod.json"
    if (Test-Path -LiteralPath $splitManifest) { return $splitManifest }
    return ""
}

function Get-RelativeFileHashes([string]$Root) {
    if (-not (Test-Path -LiteralPath $Root)) { return @{} }

    $resolvedRoot = (Resolve-Path -LiteralPath $Root).Path.TrimEnd('\')
    $hashes = @{}
    foreach ($file in @(Get-ChildItem -LiteralPath $resolvedRoot -File -Recurse | Sort-Object FullName)) {
        $relative = $file.FullName.Substring($resolvedRoot.Length).TrimStart('\').Replace('\', '/')
        $hashes[$relative] = (Get-FileHash -LiteralPath $file.FullName -Algorithm SHA256).Hash
    }
    return $hashes
}

function Add-TreeDiffIssues([string]$Label, [string]$Source, [string]$Destination, [ref]$Issues) {
    $sourceHashes = Get-RelativeFileHashes -Root $Source
    $destinationHashes = Get-RelativeFileHashes -Root $Destination
    $allPaths = @($sourceHashes.Keys + $destinationHashes.Keys | Sort-Object -Unique)

    foreach ($relative in $allPaths) {
        if (-not $sourceHashes.ContainsKey($relative)) {
            $Issues.Value += "$Label contains stale runtime file: $relative"
        }
        elseif (-not $destinationHashes.ContainsKey($relative)) {
            $Issues.Value += "$Label is missing runtime file: $relative"
        }
        elseif ($sourceHashes[$relative] -ne $destinationHashes[$relative]) {
            $Issues.Value += "$Label content differs: $relative"
        }
    }
}

$resolvedRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$sourceRoot = Join-Path $resolvedRoot "docs\examples\external_mods"
$releaseRoot = Join-Path $resolvedRoot "x64\Release"
$legacyModsRoot = Join-Path $releaseRoot "mods"

if (-not (Test-Path -LiteralPath $sourceRoot)) {
    Write-Error "Missing source directory: $sourceRoot"
    exit 1
}
if (-not (Test-Path -LiteralPath $legacyModsRoot)) {
    Write-Error "Missing runtime mods directory: $legacyModsRoot"
    exit 1
}

$issues = @()
$packages = @(Get-ChildItem -LiteralPath $sourceRoot -Directory | Sort-Object Name | Where-Object {
    -not [string]::IsNullOrWhiteSpace((Get-ManifestPath -PackageRoot $_.FullName))
})

foreach ($package in $packages) {
    $manifestPath = Get-ManifestPath -PackageRoot $package.FullName
    try {
        $manifest = Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
    }
    catch {
        $issues += "Invalid source manifest: $manifestPath"
        continue
    }

    if ($manifest.apiVersion -ne 4) {
        $issues += "apiVersion != 4 in source package: $($package.Name)"
    }

    $isSplit = $manifestPath.EndsWith("data\mod.json", [StringComparison]::OrdinalIgnoreCase)
    $manifestType = if ($manifest.PSObject.Properties.Name -contains "type") {
        "$($manifest.type)".Trim().ToLowerInvariant()
    } else { "" }

    if ($isSplit) {
        $sourceData = Join-Path $package.FullName "data"
        $runtimeData = Join-Path (Join-Path $releaseRoot "mods_data") $package.Name
        if (-not (Test-Path -LiteralPath $runtimeData)) {
            $issues += "Missing runtime data package: $($package.Name)"
        }
        elseif (-not $SkipContentHash) {
            Add-TreeDiffIssues -Label "$($package.Name) [data]" -Source $sourceData -Destination $runtimeData -Issues ([ref]$issues)
        }

        $sourceRuntime = Join-Path $package.FullName "runtime"
        if (Test-Path -LiteralPath $sourceRuntime) {
            $runtimeBinary = Join-Path (Join-Path $releaseRoot "mods_runtime") $package.Name
            if (-not (Test-Path -LiteralPath $runtimeBinary)) {
                $issues += "Missing runtime binary package: $($package.Name)"
            }
            elseif (-not $SkipContentHash) {
                Add-TreeDiffIssues -Label "$($package.Name) [runtime]" -Source $sourceRuntime -Destination $runtimeBinary -Issues ([ref]$issues)
            }
        }
        continue
    }

    $bucket = if ($manifestType -eq "framework") { "frameworks" } else { "mods" }
    $bucketDestination = Join-Path (Join-Path $legacyModsRoot $bucket) $package.Name
    $flatDestination = Join-Path $legacyModsRoot $package.Name
    $destination = if (Test-Path -LiteralPath $bucketDestination) {
        $bucketDestination
    }
    elseif (Test-Path -LiteralPath $flatDestination) {
        # Backward-compatible with the pre-bucket sync layout while old catalogs migrate.
        $flatDestination
    }
    else { "" }

    if ([string]::IsNullOrWhiteSpace($destination)) {
        $issues += "Missing runtime package: $($package.Name) (expected bucket: $bucket)"
        continue
    }

    $runtimeManifestPath = Join-Path $destination "mod.json"
    try {
        $runtimeManifest = Get-Content -LiteralPath $runtimeManifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
        if ($runtimeManifest.apiVersion -ne 4) {
            $issues += "apiVersion != 4 in runtime package: $($package.Name)"
        }
        if ("$($runtimeManifest.id)" -ne "$($manifest.id)") {
            $issues += "Manifest id mismatch in runtime package: $($package.Name)"
        }
    }
    catch {
        $issues += "Invalid runtime manifest: $runtimeManifestPath"
        continue
    }

    if (-not $SkipContentHash) {
        Add-TreeDiffIssues -Label $package.Name -Source $package.FullName -Destination $destination -Issues ([ref]$issues)
    }
}

$prefix = if ($DryRun) { "[DryRun] " } else { "" }
Write-Host "${prefix}Bucket-aware demo validation: packages=$($packages.Count) issues=$($issues.Count)"
if ($issues.Count -gt 0) {
    $issues | ForEach-Object { Write-Host "${prefix}$_" }
    if (-not $DryRun) { exit 1 }
}

if ($DryRun) { exit 0 }
Write-Host "Demo validation passed."
exit 0

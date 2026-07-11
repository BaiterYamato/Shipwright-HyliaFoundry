[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$OutDir = "docs/generated",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

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

function Read-JsonFile([string]$Path) {
    try {
        return Get-Content -Path $Path -Raw -Encoding UTF8 | ConvertFrom-Json
    } catch {
        return $null
    }
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$runtimeContractsDir = Join-Path $resolvedRoot "docs/runtime_contract"
if (-not (Test-Path $runtimeContractsDir)) {
    Write-Error "runtime_contract directory not found: $runtimeContractsDir"
    exit 1
}

$resolvedOut = Resolve-Abs -Base $resolvedRoot -Child $OutDir
$contractsOutDir = Join-Path $resolvedOut "contracts"

$generatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
$contractFiles = @(Get-ChildItem -Path $runtimeContractsDir -File -Filter *.json | Sort-Object -Property Name)
$contractEntries = @()
$contractPaths = @()

foreach ($contractFile in $contractFiles) {
    $relativePath = Get-Rel -Root $resolvedRoot -FullPath $contractFile.FullName
    $contractPaths += $relativePath

    $json = Read-JsonFile -Path $contractFile.FullName
    $schemaVersion = ""
    $description = ""
    $topLevelKeys = @()
    if ($null -ne $json) {
        if ($json.PSObject.Properties.Name -contains "schemaVersion" -and $null -ne $json.schemaVersion) {
            $schemaVersion = "$($json.schemaVersion)"
        }
        if ($json.PSObject.Properties.Name -contains "description" -and $null -ne $json.description) {
            $description = "$($json.description)"
        }
        $topLevelKeys = @($json.PSObject.Properties.Name | Sort-Object)
    }

    $contractEntries += [ordered]@{
        path = $relativePath
        schemaVersion = $schemaVersion
        description = $description
        topLevelKeys = @($topLevelKeys)
    }
}

$contractIndex = [ordered]@{
    schemaVersion = "docs.contract_index.v1"
    generatedUtc = $generatedUtc
    contracts = @($contractPaths)
    entries = @($contractEntries)
}

$registryFiles = @(
    "docs/actions.json",
    "docs/events.json",
    "docs/catalogs.json"
)
$registryEntries = @()
foreach ($registryFile in $registryFiles) {
    $fullPath = Join-Path $resolvedRoot $registryFile
    if (-not (Test-Path $fullPath)) {
        continue
    }

    $json = Read-JsonFile -Path $fullPath
    $counts = [ordered]@{}
    if ($null -ne $json) {
        foreach ($property in $json.PSObject.Properties) {
            if ($property.Value -is [System.Collections.IEnumerable] -and $property.Value -isnot [string]) {
                $counts[$property.Name] = @($property.Value).Count
            }
        }
    }
    $registryEntries += [ordered]@{
        path = $registryFile.Replace("\", "/")
        counts = $counts
    }
}

$registryIndex = [ordered]@{
    schemaVersion = "docs.registry_index.v1"
    generatedUtc = $generatedUtc
    registries = @($registryEntries | ForEach-Object { $_.path })
    entries = @($registryEntries)
    notes = @(
        "Use tools/external_mods/export_runtime_reference.ps1 to refresh runtime-derived registries.",
        "Use tools/external_mods/modtool.ps1 doctor to inspect conflicts and ownership resolution."
    )
}

$contractIndexMd = New-Object System.Collections.Generic.List[string]
$contractIndexMd.Add("# Runtime Contract Index")
$contractIndexMd.Add("")
$contractIndexMd.Add("Generated UTC: $generatedUtc")
$contractIndexMd.Add("")
$contractIndexMd.Add("| Contract | Schema | Description |")
$contractIndexMd.Add("|---|---|---|")
foreach ($entry in $contractEntries) {
    $description = if ([string]::IsNullOrWhiteSpace($entry.description)) { "-" } else { $entry.description }
    $schema = if ([string]::IsNullOrWhiteSpace($entry.schemaVersion)) { "-" } else { $entry.schemaVersion }
    $contractIndexMd.Add("| $($entry.path) | $schema | $description |")
}

$registryIndexMd = New-Object System.Collections.Generic.List[string]
$registryIndexMd.Add("# Runtime Registry Index")
$registryIndexMd.Add("")
$registryIndexMd.Add("Generated UTC: $generatedUtc")
$registryIndexMd.Add("")
$registryIndexMd.Add("| Registry | Collection counts |")
$registryIndexMd.Add("|---|---|")
foreach ($entry in $registryEntries) {
    $parts = @()
    foreach ($kv in $entry.counts.GetEnumerator()) {
        $parts += "$($kv.Key)=$($kv.Value)"
    }
    $countsText = if ($parts.Count -gt 0) { ($parts -join ", ") } else { "-" }
    $registryIndexMd.Add("| $($entry.path) | $countsText |")
}

if (-not $DryRun) {
    New-Item -ItemType Directory -Path $resolvedOut -Force | Out-Null
    New-Item -ItemType Directory -Path $contractsOutDir -Force | Out-Null

    Set-Content -Path (Join-Path $resolvedOut "contract_index.json") -Value ($contractIndex | ConvertTo-Json -Depth 32) -Encoding UTF8
    Set-Content -Path (Join-Path $resolvedOut "registry_index.json") -Value ($registryIndex | ConvertTo-Json -Depth 32) -Encoding UTF8
    Set-Content -Path (Join-Path $resolvedOut "CONTRACT_INDEX.md") -Value ($contractIndexMd -join "`n") -Encoding UTF8
    Set-Content -Path (Join-Path $resolvedOut "REGISTRY_INDEX.md") -Value ($registryIndexMd -join "`n") -Encoding UTF8

    foreach ($entry in $contractEntries) {
        $contractName = [System.IO.Path]::GetFileNameWithoutExtension($entry.path)
        $schemaText = if ([string]::IsNullOrWhiteSpace($entry.schemaVersion)) { "-" } else { $entry.schemaVersion }
        $pageLines = New-Object System.Collections.Generic.List[string]
        $pageLines.Add("# $contractName")
        $pageLines.Add("")
        $pageLines.Add(('- Path: `{0}`' -f $entry.path))
        $pageLines.Add("- Schema: $schemaText")
        if (-not [string]::IsNullOrWhiteSpace($entry.description)) {
            $pageLines.Add("- Description: $($entry.description)")
        }
        $pageLines.Add("")
        $pageLines.Add("## Top-level keys")
        if ($entry.topLevelKeys.Count -eq 0) {
            $pageLines.Add("- (none)")
        } else {
            foreach ($key in $entry.topLevelKeys) {
                $pageLines.Add(('- `{0}`' -f $key))
            }
        }
        Set-Content -Path (Join-Path $contractsOutDir ($contractName + ".md")) -Value ($pageLines -join "`n") -Encoding UTF8
    }
}

$report = [ordered]@{
    schemaVersion = "tool.modtool_report.v1"
    command = "generate-contract-docs"
    generatedUtc = $generatedUtc
    success = $true
    output = [ordered]@{
        outDir = (Get-Rel -Root $resolvedRoot -FullPath $resolvedOut)
        contractCount = $contractEntries.Count
        registryCount = $registryEntries.Count
        contractIndex = (Join-Path $resolvedOut "contract_index.json")
        registryIndex = (Join-Path $resolvedOut "registry_index.json")
    }
}

$report | ConvertTo-Json -Depth 32 | Write-Output
exit 0

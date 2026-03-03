[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [Parameter(Mandatory = $true)]
    [string]$FromRef,
    [string]$ToRef = "HEAD",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$contractFiles = @(
    "docs/actions.json",
    "docs/events.json",
    "docs/catalogs.json",
    "docs/runtime_contract/actions.registry.json",
    "docs/runtime_contract/conditions.registry.json"
)

function Get-JsonAtRef {
    param([string]$Ref, [string]$Path)
    $raw = git -C $resolvedRoot show "$Ref`:$Path" 2>$null
    if ($LASTEXITCODE -ne 0 -or [string]::IsNullOrWhiteSpace(($raw -join "`n"))) {
        return $null
    }
    try {
        return (($raw -join "`n") | ConvertFrom-Json)
    }
    catch {
        return $null
    }
}

function Normalize-Set {
    param([string]$Path,[object]$Obj)
    $items = New-Object System.Collections.Generic.HashSet[string]
    if ($null -eq $Obj) { return $items }

    switch ($Path) {
        "docs/actions.json" {
            foreach ($a in @($Obj.actions)) {
                if ($null -ne $a -and $null -ne $a.name) {
                    $value = "{0}:{1}" -f ([string]$a.category), ([string]$a.name)
                    [void]$items.Add($value)
                }
            }
        }
        "docs/events.json" {
            foreach ($h in @($Obj.hooks)) { if (-not [string]::IsNullOrWhiteSpace([string]$h)) { [void]$items.Add("hook:" + [string]$h) } }
            foreach ($e in @($Obj.behaviorEvents)) { if (-not [string]::IsNullOrWhiteSpace([string]$e)) { [void]$items.Add("behavior:" + [string]$e) } }
        }
        "docs/catalogs.json" {
            foreach ($c in @($Obj.capabilities)) {
                if ($null -ne $c -and $null -ne $c.id) { [void]$items.Add([string]$c.id) }
            }
        }
        default {
            if ($Obj.PSObject.Properties.Name -contains 'actions') {
                foreach ($a in @($Obj.actions)) {
                    if ($null -ne $a -and $null -ne $a.name) { [void]$items.Add([string]$a.name) }
                }
            }
            if ($Obj.PSObject.Properties.Name -contains 'conditions') {
                foreach ($c in @($Obj.conditions)) {
                    if ($null -ne $c -and $null -ne $c.name) { [void]$items.Add([string]$c.name) }
                }
            }
        }
    }

    return $items
}

$result = [ordered]@{
    fromRef = $FromRef
    toRef = $ToRef
    generatedUtc = [DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ')
    files = @()
    impact = @()
}

foreach ($path in $contractFiles) {
    $fromObj = Get-JsonAtRef -Ref $FromRef -Path $path
    $toObj = Get-JsonAtRef -Ref $ToRef -Path $path

    $fromSet = Normalize-Set -Path $path -Obj $fromObj
    $toSet = Normalize-Set -Path $path -Obj $toObj

    $added = @($toSet | Where-Object { -not $fromSet.Contains($_) } | Sort-Object)
    $removed = @($fromSet | Where-Object { -not $toSet.Contains($_) } | Sort-Object)

    if ($added.Count -gt 0 -or $removed.Count -gt 0) {
        $result.files += [ordered]@{
            path = $path
            added = $added
            removed = $removed
        }
        if ($added.Count -gt 0) {
            $result.impact += "Update docs/tests for added contract symbols in $path"
        }
        if ($removed.Count -gt 0) {
            $result.impact += "Review migration notes for removed symbols in $path"
        }
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Delta files with changes: $($result.files.Count)"
    exit 0
}

$result | ConvertTo-Json -Depth 8
exit 0

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$docs = @(
    "docs\MODDING.md",
    "docs\MOD_SDK_MVP.md",
    "docs\EXTERNAL_MOD_BEHAVIOR_GRAPH_V1.md",
    "docs\EXTERNAL_MOD_MANAGER_REFERENCE.md",
    "README.md"
)

$legacyTokens = @(
    "igniteFrontTarget",
    "freezeFrontTarget",
    "freezeOnMeleeHit",
    "apiVersion: 3",
    "apiVersion = 3",
    "API v3"
)

$issues = @()
foreach ($doc in $docs) {
    $path = Join-Path $RepoRoot $doc
    if (-not (Test-Path $path)) {
        continue
    }
    $content = Get-Content -Raw -Path $path -Encoding UTF8
    foreach ($token in $legacyTokens) {
        if ($content -match [regex]::Escape($token)) {
            $issues += "$doc contains legacy token: $token"
        }
    }
    if ($content -notmatch "apiVersion") {
        $issues += "$doc does not mention apiVersion contract."
    }
    if (($content -notmatch "apiVersion[^0-9]*4") -and ($content -notmatch "API v4")) {
        $issues += "$doc does not mention v4 contract baseline."
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Drift issues detected: $($issues.Count)"
    if ($issues.Count -gt 0) {
        $issues | ForEach-Object { Write-Host "[DryRun] $_" }
    }
    exit 0
}

if ($issues.Count -gt 0) {
    $issues | ForEach-Object { Write-Error $_ }
    exit 1
}

Write-Host "Doc drift check passed."
exit 0

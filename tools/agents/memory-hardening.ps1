[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$Apply,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$reconcileScript = Join-Path $resolvedRoot "docs/agents/skills/soh-plan-memory-reconciler/scripts/reconcile-plan-memory.ps1"
$rebuildScript = Join-Path $resolvedRoot "tools/agents/rebuild-index.ps1"
$validateScript = Join-Path $resolvedRoot "tools/agents/validate-memory.ps1"

if (-not (Test-Path $reconcileScript)) {
    Write-Error "Missing reconcile script: $reconcileScript"
    exit 1
}
if (-not (Test-Path $rebuildScript)) {
    Write-Error "Missing rebuild script: $rebuildScript"
    exit 1
}
if (-not (Test-Path $validateScript)) {
    Write-Error "Missing validate script: $validateScript"
    exit 1
}

$reconcileArgs = @('-ExecutionPolicy','Bypass','-File',$reconcileScript,'-RepoRoot',$resolvedRoot)
if ($Apply) { $reconcileArgs += '-Apply' }
if ($DryRun) { $reconcileArgs += '-DryRun' }

$reconcileOutput = & powershell @reconcileArgs
if ($LASTEXITCODE -ne 0) {
    Write-Error "reconcile-plan-memory failed."
    exit 1
}

if (-not $DryRun) {
    & powershell -ExecutionPolicy Bypass -File $rebuildScript
    if ($LASTEXITCODE -ne 0) {
        Write-Error "rebuild-index failed."
        exit 1
    }

    & powershell -ExecutionPolicy Bypass -File $validateScript
    if ($LASTEXITCODE -ne 0) {
        Write-Error "validate-memory failed."
        exit 1
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Memory hardening pipeline succeeded."
} else {
    Write-Host "Memory hardening pipeline succeeded."
}

if ($reconcileOutput) {
    $reconcileOutput | Write-Output
}

exit 0

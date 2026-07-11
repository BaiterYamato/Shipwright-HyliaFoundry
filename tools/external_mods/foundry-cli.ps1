[CmdletBinding()]
param(
    [Parameter(Mandatory = $true, Position = 0)]
    [ValidateSet("validate", "sync", "migrate-v3-to-v4", "export-runtime-reference", "generate-contract-docs", "modtool")]
    [string]$Command,
    [string]$RepoRoot = ".",
    [string]$Path = "",
    [string]$Subcommand = "",
    [ValidateSet("stable", "beta", "testers")]
    [string]$ReleaseChannel = "stable",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$scriptsRoot = Join-Path $resolvedRoot "tools/external_mods"

switch ($Command) {
    "validate" {
        $validateScript = Join-Path $scriptsRoot "validate_mod.ps1"
        & $validateScript -RepoRoot $resolvedRoot -ModPath $Path -DryRun:$DryRun
        exit $LASTEXITCODE
    }
    "sync" {
        $syncScript = Join-Path $scriptsRoot "sync_examples_to_runtime.ps1"
        & $syncScript -SourceRoot "docs/examples/external_mods" -RuntimeRoot "x64/Release/mods" -Clean:$true
        exit $LASTEXITCODE
    }
    "migrate-v3-to-v4" {
        $migrateScript = Join-Path $scriptsRoot "migrate_mods_v3_to_v4.ps1"
        $modsRoot = if ([string]::IsNullOrWhiteSpace($Path)) { "docs/examples/external_mods" } else { $Path }
        & $migrateScript -RepoRoot $resolvedRoot -ModsRoot $modsRoot -DryRun:$DryRun
        exit $LASTEXITCODE
    }
    "export-runtime-reference" {
        $exportScript = Join-Path $scriptsRoot "export_runtime_reference.ps1"
        & $exportScript -RepoRoot $resolvedRoot -DryRun:$DryRun
        exit $LASTEXITCODE
    }
    "generate-contract-docs" {
        $generateScript = Join-Path $scriptsRoot "generate_contract_docs.ps1"
        & $generateScript -RepoRoot $resolvedRoot -DryRun:$DryRun
        exit $LASTEXITCODE
    }
    "modtool" {
        $modtoolScript = Join-Path $scriptsRoot "modtool.ps1"
        if (-not (Test-Path $modtoolScript)) {
            Write-Error "modtool.ps1 not found at: $modtoolScript"
            exit 1
        }
        if ([string]::IsNullOrWhiteSpace($Subcommand)) {
            Write-Error "Subcommand is required for modtool. Example: foundry-cli.ps1 modtool -Subcommand validate -Path docs/examples/external_mods"
            exit 1
        }
        & $modtoolScript $Subcommand -RepoRoot $resolvedRoot -Path $Path -ReleaseChannel $ReleaseChannel -DryRun:$DryRun
        exit $LASTEXITCODE
    }
}

Write-Error "Unknown command: $Command"
exit 1

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$DryRun,
    [long]$LogOffset = -1,
    [datetime]$LogSince = [datetime]::MinValue,
    [switch]$RequireLog
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$checks = @()

function Add-Check {
    param([string]$Name, [bool]$Ok, [string]$Detail)
    $script:checks += [PSCustomObject]@{
        name = $Name
        ok = $Ok
        detail = $Detail
    }
}

function Read-LogFromOffset([string]$Path, [long]$Offset) {
    $stream = [System.IO.File]::Open($Path, [System.IO.FileMode]::Open, [System.IO.FileAccess]::Read,
        [System.IO.FileShare]::ReadWrite)
    try {
        if ($Offset -gt $stream.Length) {
            throw "Log offset $Offset exceeds current log length $($stream.Length); the log was rotated or truncated."
        }
        $null = $stream.Seek([Math]::Max(0, $Offset), [System.IO.SeekOrigin]::Begin)
        $reader = [System.IO.StreamReader]::new($stream, [System.Text.Encoding]::UTF8, $true, 4096, $true)
        try { return $reader.ReadToEnd() } finally { $reader.Dispose() }
    }
    finally { $stream.Dispose() }
}

function Get-LinesSince([string]$Content, [datetime]$Since, [datetime]$LogDate) {
    $result = @()
    foreach ($line in @($Content -split "`r?`n")) {
        if ($line -notmatch '^\[(\d{2}:\d{2}:\d{2}(?:\.\d{1,7})?)\]') { continue }
        $time = [timespan]::Zero
        if (-not [timespan]::TryParse($Matches[1], [ref]$time)) { continue }
        $timestamp = $LogDate.Date.Add($time)
        if ($timestamp -ge $Since) { $result += $line }
    }
    return $result
}

$resolvedRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$examples = Join-Path $resolvedRoot "docs\examples\external_mods"
$runtimeMods = Join-Path $resolvedRoot "x64\Release\mods"
$logPath = Join-Path $resolvedRoot "x64\Release\logs\Ship of Harkinian.log"

Add-Check -Name "examples_dir" -Ok (Test-Path -LiteralPath $examples) -Detail $examples
Add-Check -Name "runtime_mods_dir" -Ok (Test-Path -LiteralPath $runtimeMods) -Detail $runtimeMods

if (Test-Path -LiteralPath $examples) {
    $mods = @(Get-ChildItem -LiteralPath $examples -Directory)
    Add-Check -Name "examples_count" -Ok ($mods.Count -gt 0) -Detail "count=$($mods.Count)"
}

if (Test-Path -LiteralPath $logPath) {
    $logItem = Get-Item -LiteralPath $logPath
    $effectiveOffset = $LogOffset
    if ($effectiveOffset -lt 0 -and $LogSince -eq [datetime]::MinValue) {
        # Never turn historical log entries into current failures by default.
        $effectiveOffset = $logItem.Length
    }

    $content = if ($effectiveOffset -ge 0) {
        Read-LogFromOffset -Path $logPath -Offset $effectiveOffset
    }
    else {
        Get-Content -LiteralPath $logPath -Raw -Encoding UTF8
    }

    $lines = @($content -split "`r?`n")
    if ($LogSince -ne [datetime]::MinValue) {
        $lines = @(Get-LinesSince -Content $content -Since $LogSince -LogDate $logItem.LastWriteTime)
    }

    $critical = @($lines | Where-Object {
        $_ -match '(?i)\[critical\]|Runtime disabled for|Missing or invalid field'
    })
    $window = if ($effectiveOffset -ge 0) { "offset=$effectiveOffset" } else { "since=$($LogSince.ToString('o'))" }
    Add-Check -Name "log_blocker_scan" -Ok ($critical.Count -eq 0) -Detail "$window hits=$($critical.Count)"
    if ($critical.Count -gt 0) {
        Add-Check -Name "log_blocker_first" -Ok $false -Detail $critical[0]
    }
}
elseif ($RequireLog) {
    Add-Check -Name "log_blocker_scan" -Ok $false -Detail "required log not found: $logPath"
}
else {
    Add-Check -Name "log_blocker_scan" -Ok $true -Detail "log not found (skipped)"
}

if ($DryRun) {
    Write-Host "[DryRun] Smoke checks prepared: $($checks.Count)"
    $checks | ConvertTo-Json -Depth 4
    exit 0
}

$checks | ConvertTo-Json -Depth 4
if (@($checks | Where-Object { -not $_.ok }).Count -gt 0) { exit 1 }
exit 0

[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [switch]$SkipBuild,
    [switch]$SkipRuntimeParity,
    [string]$RuntimeCommand = "",
    [int]$RuntimeTimeoutSeconds = 45,
    [string[]]$ExpectedFailureModIds = @("com.sylian.reference.dependency_matrix")
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -LiteralPath $RepoRoot).Path
$powershell = (Get-Process -Id $PID).Path
$validator = Join-Path $resolvedRoot "tools\external_mods\validate_mod.ps1"
$demoValidator = Join-Path $resolvedRoot "docs\agents\skills\soh-demo-sync-validator\scripts\validate-demos.ps1"
$docDrift = Join-Path $resolvedRoot "docs\agents\skills\soh-doc-runtime-drift-guard\scripts\check-doc-drift.ps1"
$lockCheck = Join-Path $resolvedRoot "docs\agents\skills\soh-build-test-windows\scripts\check-build-lock.ps1"
$smoke = Join-Path $resolvedRoot "docs\agents\skills\soh-smoke-regression-matrix\scripts\run-smoke.ps1"
$examplesRoot = Join-Path $resolvedRoot "docs\examples\external_mods"
$logPath = Join-Path $resolvedRoot "x64\Release\logs\Ship of Harkinian.log"
$failures = @()

function Invoke-GateScript {
    param(
        [string]$Name,
        [string]$ScriptPath,
        [string[]]$Arguments,
        [int]$ExpectedExitCode = 0
    )

    Write-Host "[qa] $Name"
    & $powershell -NoProfile -ExecutionPolicy Bypass -File $ScriptPath @Arguments
    $actual = $LASTEXITCODE
    if ($actual -ne $ExpectedExitCode) {
        $script:failures += "$Name exited $actual; expected $ExpectedExitCode"
        Write-Host "[qa][FAIL] $Name exited $actual; expected $ExpectedExitCode" -ForegroundColor Red
        return $false
    }
    Write-Host "[qa][PASS] $Name" -ForegroundColor Green
    return $true
}

function Get-PackageManifest([string]$PackageRoot) {
    $rootManifest = Join-Path $PackageRoot "mod.json"
    if (Test-Path -LiteralPath $rootManifest) { return $rootManifest }
    $splitManifest = Join-Path $PackageRoot "data\mod.json"
    if (Test-Path -LiteralPath $splitManifest) { return $splitManifest }
    return ""
}

function Write-JsonFile([string]$Path, [object]$Value) {
    $parent = Split-Path -Path $Path -Parent
    $null = New-Item -ItemType Directory -Force -Path $parent
    $Value | ConvertTo-Json -Depth 12 | Set-Content -LiteralPath $Path -Encoding UTF8
}

Push-Location $resolvedRoot
try {
    $null = Invoke-GateScript -Name "doc/runtime drift" -ScriptPath $docDrift -Arguments @("-RepoRoot", $resolvedRoot)

    foreach ($package in @(Get-ChildItem -LiteralPath $examplesRoot -Directory | Sort-Object Name)) {
        $manifestPath = Get-PackageManifest -PackageRoot $package.FullName
        if ([string]::IsNullOrWhiteSpace($manifestPath)) { continue }

        $manifest = Get-Content -LiteralPath $manifestPath -Raw -Encoding UTF8 | ConvertFrom-Json
        $modId = "$($manifest.id)"
        $expectedFailure = $ExpectedFailureModIds -contains $modId
        $validatorRoot = if ($manifestPath.EndsWith("data\mod.json", [StringComparison]::OrdinalIgnoreCase)) {
            Split-Path -Path $manifestPath -Parent
        } else { $package.FullName }

        $null = Invoke-GateScript -Name "validate $modId" -ScriptPath $validator `
            -Arguments @("-RepoRoot", $resolvedRoot, "-ModPath", $validatorRoot) `
            -ExpectedExitCode $(if ($expectedFailure) { 1 } else { 0 })
    }

    if (-not $SkipRuntimeParity) {
        $null = Invoke-GateScript -Name "bucket-aware demo parity" -ScriptPath $demoValidator `
            -Arguments @("-RepoRoot", $resolvedRoot)
    }

    $faultRoot = Join-Path ([System.IO.Path]::GetTempPath()) ("soh-qa-faults-" + [guid]::NewGuid().ToString("N"))
    try {
        $validManifest = [ordered]@{
            id = "com.sylian.qa.valid"
            name = "QA valid"
            version = "1.0.0"
            apiVersion = 4
            entryScript = "scripts/init.json"
            runtime = [ordered]@{ type = "wasm3-v1"; module = "scripts/noop.wat" }
            capabilities = @()
        }
        $entry = [ordered]@{ apiVersion = 4; onInit = @() }

        $validRoot = Join-Path $faultRoot "valid"
        Write-JsonFile -Path (Join-Path $validRoot "mod.json") -Value $validManifest
        Write-JsonFile -Path (Join-Path $validRoot "scripts\init.json") -Value $entry
        Set-Content -LiteralPath (Join-Path $validRoot "scripts\noop.wat") -Value "(module)" -Encoding UTF8
        $null = Invoke-GateScript -Name "fault baseline valid" -ScriptPath $validator `
            -Arguments @("-RepoRoot", $resolvedRoot, "-ModPath", $validRoot)

        $apiRoot = Join-Path $faultRoot "invalid_api"
        $invalidApiManifest = $validManifest | ConvertTo-Json -Depth 12 | ConvertFrom-Json
        $invalidApiManifest.id = "com.sylian.qa.invalid_api"
        $invalidApiManifest.apiVersion = 3
        Write-JsonFile -Path (Join-Path $apiRoot "mod.json") -Value $invalidApiManifest
        Write-JsonFile -Path (Join-Path $apiRoot "scripts\init.json") -Value $entry
        $null = Invoke-GateScript -Name "fault invalid api" -ScriptPath $validator `
            -Arguments @("-RepoRoot", $resolvedRoot, "-ModPath", $apiRoot) -ExpectedExitCode 1

        $missingEntryRoot = Join-Path $faultRoot "missing_entry"
        $missingEntryManifest = $validManifest | ConvertTo-Json -Depth 12 | ConvertFrom-Json
        $missingEntryManifest.id = "com.sylian.qa.missing_entry"
        Write-JsonFile -Path (Join-Path $missingEntryRoot "mod.json") -Value $missingEntryManifest
        $null = Invoke-GateScript -Name "fault missing entry" -ScriptPath $validator `
            -Arguments @("-RepoRoot", $resolvedRoot, "-ModPath", $missingEntryRoot) -ExpectedExitCode 1

        $missingCapabilityRoot = Join-Path $faultRoot "missing_capability_file"
        $missingCapabilityManifest = $validManifest | ConvertTo-Json -Depth 12 | ConvertFrom-Json
        $missingCapabilityManifest.id = "com.sylian.qa.missing_capability_file"
        $missingCapabilityManifest.capabilities = @("items.state_machine.v1")
        $missingCapabilityManifest | Add-Member -NotePropertyName "itemStateDefinitions" `
            -NotePropertyValue "items/item_states.json"
        Write-JsonFile -Path (Join-Path $missingCapabilityRoot "mod.json") -Value $missingCapabilityManifest
        Write-JsonFile -Path (Join-Path $missingCapabilityRoot "scripts\init.json") -Value $entry
        $null = Invoke-GateScript -Name "fault missing capability file" -ScriptPath $validator `
            -Arguments @("-RepoRoot", $resolvedRoot, "-ModPath", $missingCapabilityRoot) -ExpectedExitCode 1
    }
    finally {
        if (Test-Path -LiteralPath $faultRoot) { Remove-Item -LiteralPath $faultRoot -Recurse -Force }
    }

    if (-not $SkipBuild) {
        $null = Invoke-GateScript -Name "build lock" -ScriptPath $lockCheck -Arguments @("-RepoRoot", $resolvedRoot)
        Write-Host "[qa] deterministic Release build (/m:1)"
        & cmake --build (Join-Path $resolvedRoot "build/x64") --config Release --target soh -- /m:1
        if ($LASTEXITCODE -ne 0) { $failures += "Release build failed with exit $LASTEXITCODE" }
    }

    if (-not [string]::IsNullOrWhiteSpace($RuntimeCommand)) {
        $logOffset = if (Test-Path -LiteralPath $logPath) { (Get-Item -LiteralPath $logPath).Length } else { 0 }
        Write-Host "[qa] runtime command (timeout=${RuntimeTimeoutSeconds}s)"
        $process = Start-Process -FilePath "cmd.exe" -ArgumentList @("/d", "/s", "/c", $RuntimeCommand) `
            -WorkingDirectory $resolvedRoot -WindowStyle Hidden -PassThru
        if (-not $process.WaitForExit($RuntimeTimeoutSeconds * 1000)) {
            $process.Kill()
            $failures += "Runtime command timed out after ${RuntimeTimeoutSeconds}s"
        }
        elseif ($process.ExitCode -ne 0) {
            $failures += "Runtime command exited $($process.ExitCode)"
        }
        $null = Invoke-GateScript -Name "new runtime log blockers" -ScriptPath $smoke `
            -Arguments @("-RepoRoot", $resolvedRoot, "-LogOffset", "$logOffset", "-RequireLog")
    }
    else {
        Write-Host "[qa][SKIP] runtime smoke; provide -RuntimeCommand to scan only newly written log bytes"
    }
}
finally {
    Pop-Location
}

if ($failures.Count -gt 0) {
    $failures | ForEach-Object { Write-Host "[qa][FAIL] $_" -ForegroundColor Red }
    exit 1
}

Write-Host "[qa] all selected gates passed" -ForegroundColor Green
exit 0

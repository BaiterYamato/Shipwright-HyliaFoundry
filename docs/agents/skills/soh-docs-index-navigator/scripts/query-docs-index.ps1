[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$IndexPath = "docs/.cache/docs.index.json",
    [string]$Query = "",
    [string]$Contract = "",
    [string]$PathContains = "",
    [int]$Limit = 10,
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

if ($Limit -lt 1) {
    Write-Error "Limit must be >= 1."
    exit 1
}

if ([string]::IsNullOrWhiteSpace($Query) -and [string]::IsNullOrWhiteSpace($Contract) -and [string]::IsNullOrWhiteSpace($PathContains)) {
    Write-Error "Provide at least one filter: -Query, -Contract, or -PathContains."
    exit 1
}

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$resolvedIndex = Join-Path $resolvedRoot $IndexPath
if (-not (Test-Path $resolvedIndex)) {
    Write-Error "Docs index not found: $resolvedIndex. Run build-docs-index.ps1 first."
    exit 1
}

$index = Get-Content -Raw -Path $resolvedIndex -Encoding UTF8 | ConvertFrom-Json
$results = New-Object System.Collections.Generic.List[object]

$queryLc = $Query.ToLowerInvariant()
$contractLc = $Contract.ToLowerInvariant()
$pathLc = $PathContains.ToLowerInvariant()

foreach ($doc in @($index.documents)) {
    $score = 0
    $pathText = ([string]$doc.path).ToLowerInvariant()
    $titleText = ([string]$doc.title).ToLowerInvariant()
    $headingsText = (@($doc.headings) -join ' ').ToLowerInvariant()
    $contractsText = (@($doc.contracts) -join ' ').ToLowerInvariant()

    if (-not [string]::IsNullOrWhiteSpace($Query)) {
        if ($titleText.Contains($queryLc)) { $score += 6 }
        if ($headingsText.Contains($queryLc)) { $score += 3 }
        if ($pathText.Contains($queryLc)) { $score += 2 }
        if ($contractsText.Contains($queryLc)) { $score += 4 }
    }

    if (-not [string]::IsNullOrWhiteSpace($Contract)) {
        if (@($doc.contracts | ForEach-Object { ([string]$_).ToLowerInvariant() }) -contains $contractLc) {
            $score += 10
        } else {
            continue
        }
    }

    if (-not [string]::IsNullOrWhiteSpace($PathContains) -and -not $pathText.Contains($pathLc)) {
        continue
    }

    if ($score -gt 0) {
        $results.Add([ordered]@{
            score = $score
            path = [string]$doc.path
            title = [string]$doc.title
            contracts = @($doc.contracts)
            headings = @($doc.headings | Select-Object -First 6)
        })
    }
}

$ordered = @(
    $results |
    Sort-Object -Property @{ Expression = 'score'; Descending = $true }, @{ Expression = 'path'; Descending = $false } |
    Select-Object -First $Limit
)

if ($DryRun) {
    Write-Host "[DryRun] Matches: $($ordered.Count)"
    exit 0
}

$ordered | ConvertTo-Json -Depth 8
exit 0

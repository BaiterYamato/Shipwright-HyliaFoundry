[CmdletBinding()]
param(
    [string]$RepoRoot = ".",
    [string]$OutputPath = "docs/.cache/docs.index.json",
    [switch]$DryRun
)

Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

$resolvedRoot = (Resolve-Path -Path $RepoRoot).Path
$docsRoot = Join-Path $resolvedRoot "docs"
if (-not (Test-Path $docsRoot)) {
    Write-Error "Docs directory not found: $docsRoot"
    exit 1
}

$files = Get-ChildItem -Path $docsRoot -Recurse -File | Where-Object {
    $_.Extension -in @('.md', '.json')
}

$documents = New-Object System.Collections.Generic.List[object]
$contractRegex = [regex]'\b[a-z][a-z0-9_.-]+\.v\d+\b'
$apiRegex = [regex]'apiVersion\s*[:=]\s*"?(\d+)"?'
$rootUri = New-Object System.Uri(($resolvedRoot.TrimEnd('\') + '\'))

foreach ($file in $files) {
    $relativePath = $rootUri.MakeRelativeUri((New-Object System.Uri($file.FullName))).ToString().Replace('\\', '/')
    $content = Get-Content -Raw -Path $file.FullName -Encoding UTF8

    $headings = New-Object System.Collections.Generic.List[string]
    if ($file.Extension -eq '.md') {
        foreach ($line in (Get-Content -Path $file.FullName -Encoding UTF8)) {
            if ($line -match '^#{1,6}\s+(.+)$') {
                $headings.Add($Matches[1].Trim())
            }
        }
    }

    $title = if ($headings.Count -gt 0) { $headings[0] } else { [IO.Path]::GetFileNameWithoutExtension($file.Name) }

    $contracts = New-Object System.Collections.Generic.HashSet[string]
    foreach ($match in $contractRegex.Matches($content)) {
        [void]$contracts.Add($match.Value)
    }

    $apiMentions = New-Object System.Collections.Generic.HashSet[string]
    foreach ($match in $apiRegex.Matches($content)) {
        [void]$apiMentions.Add($match.Groups[1].Value)
    }

    $tags = New-Object System.Collections.Generic.HashSet[string]
    foreach ($segment in $relativePath.Split('/')) {
        if (-not [string]::IsNullOrWhiteSpace($segment)) {
            [void]$tags.Add($segment.ToLowerInvariant())
        }
    }

    $documents.Add([ordered]@{
        path = $relativePath
        title = $title
        headings = @($headings)
        contracts = @($contracts | Sort-Object)
        apiMentions = @($apiMentions | Sort-Object)
        tags = @($tags | Sort-Object)
        modifiedUtc = $file.LastWriteTimeUtc.ToString('yyyy-MM-ddTHH:mm:ssZ')
    })
}

$index = [ordered]@{
    schemaVersion = 1
    generatedUtc = [DateTime]::UtcNow.ToString('yyyy-MM-ddTHH:mm:ssZ')
    root = "docs"
    documents = @($documents | Sort-Object path)
}

$resolvedOutput = Join-Path $resolvedRoot $OutputPath
$parent = Split-Path -Parent $resolvedOutput
if (-not (Test-Path $parent)) {
    if (-not $DryRun) {
        New-Item -ItemType Directory -Force -Path $parent | Out-Null
    }
}

if ($DryRun) {
    Write-Host "[DryRun] Docs indexed: $($documents.Count)"
    Write-Host "[DryRun] Output: $resolvedOutput"
    exit 0
}

$index | ConvertTo-Json -Depth 10 | Set-Content -Path $resolvedOutput -Encoding UTF8
Write-Host "Docs index generated: $resolvedOutput ($($documents.Count) docs)"
exit 0

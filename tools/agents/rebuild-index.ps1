Set-StrictMode -Version Latest
$ErrorActionPreference = "Stop"

function Get-AgentsPaths {
    $repoRoot = Resolve-Path (Join-Path $PSScriptRoot "..\..")
    $agentsDir = Join-Path $repoRoot "docs\agents"
    return @{
        RepoRoot = $repoRoot
        AgentsDir = $agentsDir
        Log = Join-Path $agentsDir "memory.log"
        Index = Join-Path $agentsDir "memory.index.json"
        Manifest = Join-Path $agentsDir "archive.manifest.json"
        Plans = Join-Path $agentsDir "Plans.md"
    }
}

function Add-IndexListValue {
    param(
        [hashtable]$Table,
        [string]$Key,
        [string]$Value
    )

    if (-not $Table.ContainsKey($Key)) {
        $Table[$Key] = New-Object System.Collections.ArrayList
    }
    [void]$Table[$Key].Add($Value)
}

function Read-JsonLinesFile {
    param(
        [string]$Path
    )

    $entries = @()
    if (-not (Test-Path $Path)) {
        return $entries
    }

    $lineNo = 0
    foreach ($line in Get-Content -Path $Path -Encoding UTF8) {
        $lineNo++
        if ([string]::IsNullOrWhiteSpace($line)) {
            continue
        }
        $obj = $line | ConvertFrom-Json
        $entries += [PSCustomObject]@{
            Entry = $obj
            Line = $lineNo
            Source = [IO.Path]::GetFileName($Path)
        }
    }

    return $entries
}

$paths = Get-AgentsPaths

if (-not (Test-Path $paths.AgentsDir)) {
    throw "Agents directory not found: $($paths.AgentsDir)"
}

$byId = @{}
$byTag = @{}
$byTopic = @{}
$byStatus = @{
    open = (New-Object System.Collections.ArrayList)
    done = (New-Object System.Collections.ArrayList)
    deprecated = (New-Object System.Collections.ArrayList)
}
$ranges = New-Object System.Collections.ArrayList
$duplicateIds = New-Object System.Collections.ArrayList
$danglingSupersedes = New-Object System.Collections.ArrayList
$danglingPlanTopics = New-Object System.Collections.ArrayList

$currentEntries = Read-JsonLinesFile -Path $paths.Log
$allEntryIds = @{}
$planIds = New-Object System.Collections.Generic.HashSet[string]
$supersededIds = New-Object System.Collections.Generic.HashSet[string]

if (Test-Path $paths.Plans) {
    foreach ($line in (Get-Content -Path $paths.Plans -Encoding UTF8)) {
        if ($line -match "^\#\# \[(PLN-\d{8}-\d{4})\]") {
            [void]$planIds.Add($Matches[1])
        }
    }
}

foreach ($row in $currentEntries) {
    foreach ($sup in @($row.Entry.supersedes)) {
        $supId = [string]$sup
        if (-not [string]::IsNullOrWhiteSpace($supId)) {
            [void]$supersededIds.Add($supId)
        }
    }
}

foreach ($row in $currentEntries) {
    $entry = $row.Entry
    $id = [string]$entry.id

    if ([string]::IsNullOrWhiteSpace($id)) {
        continue
    }

    if ($allEntryIds.ContainsKey($id)) {
        [void]$duplicateIds.Add($id)
    }
    $allEntryIds[$id] = $true

    $byId[$id] = [ordered]@{
        topic = [string]$entry.topic
        status = [string]$entry.status
        line = [int]$row.Line
        source = [string]$row.Source
    }

    Add-IndexListValue -Table $byTopic -Key ([string]$entry.topic) -Value $id
    Add-IndexListValue -Table $byStatus -Key ([string]$entry.status) -Value $id

    foreach ($tag in @($entry.tags)) {
        $tagValue = [string]$tag
        if (-not [string]::IsNullOrWhiteSpace($tagValue)) {
            Add-IndexListValue -Table $byTag -Key $tagValue -Value $id
        }
    }

    $topic = [string]$entry.topic
    $status = [string]$entry.status
    if ($topic.StartsWith("plan:")) {
        $rawPlanTopic = $topic.Substring(5)
        $normalizedPlanId = $rawPlanTopic
        if ($rawPlanTopic -match "^(PLN-\d{8}-\d{4})(?:-.+)?$") {
            $normalizedPlanId = $Matches[1]
        }

        # Consistency only flags open plan topics. Historical done/deprecated entries
        # may legally reference finalized/superseded plan aliases.
        if ($status -eq "open" -and -not $supersededIds.Contains($id) -and -not [string]::IsNullOrWhiteSpace($normalizedPlanId) -and -not $planIds.Contains($normalizedPlanId)) {
            [void]$danglingPlanTopics.Add([ordered]@{
                id = $id
                topic = $topic
                normalizedPlanId = $normalizedPlanId
                reason = "Open plan topic not found in Plans.md"
            })
        }
    }
}

foreach ($row in $currentEntries) {
    $entry = $row.Entry
    $entryId = [string]$entry.id
    foreach ($sup in @($entry.supersedes)) {
        $supId = [string]$sup
        if ([string]::IsNullOrWhiteSpace($supId)) { continue }
        if (-not $allEntryIds.ContainsKey($supId)) {
            [void]$danglingSupersedes.Add([ordered]@{
                id = $entryId
                supersedes = $supId
                reason = "Referenced id not found in active memory.log (may exist in archive)"
            })
        }
    }
}

if (Test-Path $paths.Manifest) {
    $manifest = Get-Content -Path $paths.Manifest -Raw -Encoding UTF8 | ConvertFrom-Json
    foreach ($segment in @($manifest.segments)) {
        [void]$ranges.Add([ordered]@{
            name = [string]$segment.name
            fromId = [string]$segment.fromId
            toId = [string]$segment.toId
            count = [int]$segment.count
            sha256 = [string]$segment.sha256
        })
    }
}

$openCount = @($byStatus.open).Count
$doneCount = @($byStatus.done).Count
$deprecatedCount = @($byStatus.deprecated).Count
$entryCount = $byId.Keys.Count

$index = [ordered]@{
    version = 2
    schemaVersion = 2
    lastUpdatedUtc = [DateTime]::UtcNow.ToString("yyyy-MM-ddTHH:mm:ssZ")
    stats = [ordered]@{
        entryCount = $entryCount
        openCount = $openCount
        doneCount = $doneCount
        deprecatedCount = $deprecatedCount
        topicCount = $byTopic.Keys.Count
        tagCount = $byTag.Keys.Count
    }
    consistency = [ordered]@{
        duplicateIds = @($duplicateIds | Select-Object -Unique)
        danglingSupersedes = @($danglingSupersedes)
        danglingPlanTopics = @($danglingPlanTopics)
    }
    byId = $byId
    byTag = $byTag
    byTopic = $byTopic
    byStatus = $byStatus
    ranges = @($ranges)
}

$index | ConvertTo-Json -Depth 12 | Set-Content -Path $paths.Index -Encoding UTF8
Write-Host "Rebuilt memory index: $($paths.Index)"

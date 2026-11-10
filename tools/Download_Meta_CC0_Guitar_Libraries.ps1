[CmdletBinding()]
param(
    [string]$InstallRoot = "$env:APPDATA\PopPunkGuitarUltimate\Samples\SourceLibraries"
)

$ErrorActionPreference = 'Stop'
[Net.ServicePointManager]::SecurityProtocol = [Net.SecurityProtocolType]::Tls12

$repo = 'facebookincubator/Meta-open-sound'
$releases = @(
    @{ Tag = 'meta-steel-guitar-v1.0.0'; Folder = 'Meta-Steel-Guitar'; ExpectedWav = 375 },
    @{ Tag = 'meta-nylon-guitar-v1.0.0'; Folder = 'Meta-Nylon-Guitar'; ExpectedWav = 348 }
)
$headers = @{ 'User-Agent' = 'POP-PUNK-GUITAR-ULTIMATE-library-installer' }

function Resolve-ChecksumFile([string]$Base, [string]$Relative) {
    $normal = $Relative.Replace('/', [IO.Path]::DirectorySeparatorChar).TrimStart('\')
    $candidate = Join-Path $Base $normal
    if (Test-Path -LiteralPath $candidate -PathType Leaf) { return (Get-Item -LiteralPath $candidate).FullName }

    $leaf = Split-Path $normal -Leaf
    $matches = @(Get-ChildItem -LiteralPath $Base -Filter $leaf -File -Recurse)
    if ($matches.Count -eq 1) { return $matches[0].FullName }

    if ($matches.Count -gt 1) {
        $suffixMatches = @($matches | Where-Object {
            $_.FullName.EndsWith($normal, [StringComparison]::OrdinalIgnoreCase)
        })
        if ($suffixMatches.Count -eq 1) { return $suffixMatches[0].FullName }
    }
    throw "Checksum target is missing or ambiguous: $Relative"
}

function Test-Library([string]$LibraryRoot, [int]$ExpectedWav) {
    $sfzFiles = @(Get-ChildItem -LiteralPath $LibraryRoot -Filter '*.sfz' -File -Recurse)
    $wavFiles = @(Get-ChildItem -LiteralPath $LibraryRoot -Filter '*.wav' -File -Recurse)
    $licenseFiles = @(Get-ChildItem -LiteralPath $LibraryRoot -Filter 'LICENSE' -File -Recurse)
    $sumFiles = @(Get-ChildItem -LiteralPath $LibraryRoot -Filter 'SHA256SUMS' -File -Recurse)

    if ($sfzFiles.Count -eq 0) { throw "No SFZ map found in $LibraryRoot" }
    if ($wavFiles.Count -ne $ExpectedWav) {
        throw "WAV inventory mismatch: expected $ExpectedWav, found $($wavFiles.Count)"
    }
    if ($licenseFiles.Count -eq 0) { throw 'Upstream LICENSE file is missing; refusing installation.' }
    if ($sumFiles.Count -eq 0) { throw 'Upstream SHA256SUMS file is missing; refusing installation.' }

    $checked = 0
    foreach ($sumFile in $sumFiles) {
        foreach ($line in Get-Content -LiteralPath $sumFile.FullName) {
            if ($line -match '^\s*([A-Fa-f0-9]{64})\s+\*?(.+?)\s*$') {
                $expected = $matches[1].ToLowerInvariant()
                $relative = $matches[2].Trim()
                $target = Resolve-ChecksumFile $LibraryRoot $relative
                $actual = (Get-FileHash -LiteralPath $target -Algorithm SHA256).Hash.ToLowerInvariant()
                if ($actual -ne $expected) { throw "SHA256 mismatch: $target" }
                $checked++
            }
        }
    }
    if ($checked -eq 0) { throw 'No valid SHA256 entries were checked.' }

    # The official SFZ maps refer to OGG preview names. Confirm that every
    # referenced OGG has exactly one same-name WAV source in the WAV release.
    foreach ($sfz in $sfzFiles) {
        foreach ($line in Get-Content -LiteralPath $sfz.FullName) {
            $withoutComment = ($line -split '//', 2)[0]
            if ($withoutComment -match '(?i)\bsample\s*=\s*"?([^"\s]+)') {
                $sampleName = [IO.Path]::GetFileName($matches[1])
                if ($sampleName -match '(?i)\.ogg$') {
                    $wavName = [IO.Path]::ChangeExtension($sampleName, '.wav')
                    $wavMatches = @(Get-ChildItem -LiteralPath $LibraryRoot -Filter $wavName -File -Recurse)
                    if ($wavMatches.Count -ne 1) {
                        throw "SFZ sample $sampleName must resolve to exactly one WAV file; found $($wavMatches.Count)"
                    }
                }
            }
        }
    }

    Write-Host "Verified: $($wavFiles.Count) WAV, $($sfzFiles.Count) SFZ, $checked SHA256 entries, upstream LICENSE and WAV references."
}

New-Item -ItemType Directory -Force -Path $InstallRoot | Out-Null
$tempRoot = Join-Path $env:TEMP ("PPGU_Meta_" + [guid]::NewGuid().ToString('N'))
New-Item -ItemType Directory -Force -Path $tempRoot | Out-Null

try {
    foreach ($item in $releases) {
        Write-Host "Resolving official release $($item.Tag) ..."
        $release = Invoke-RestMethod -Uri "https://api.github.com/repos/$repo/releases/tags/$($item.Tag)" -Headers $headers
        $asset = @($release.assets | Where-Object { $_.name -match '(?i)wav.*\.zip$' } |
            Sort-Object -Property size -Descending | Select-Object -First 1)
        if ($asset.Count -eq 0) { throw "No official WAV ZIP asset found for $($item.Tag)" }

        $zipPath = Join-Path $tempRoot $asset[0].name
        $extractPath = Join-Path $tempRoot $item.Folder
        New-Item -ItemType Directory -Force -Path $extractPath | Out-Null

        Write-Host "Downloading $($asset[0].name) ..."
        Invoke-WebRequest -Uri $asset[0].browser_download_url -OutFile $zipPath -Headers $headers
        Expand-Archive -LiteralPath $zipPath -DestinationPath $extractPath -Force

        $sfz = Get-ChildItem -LiteralPath $extractPath -Filter '*.sfz' -File -Recurse | Select-Object -First 1
        if ($null -eq $sfz) { throw "Downloaded archive contains no SFZ map: $($item.Tag)" }
        $libraryRoot = $sfz.DirectoryName
        Test-Library $libraryRoot $item.ExpectedWav

        $destination = Join-Path $InstallRoot $item.Folder
        $backup = "$destination.backup"
        if (Test-Path -LiteralPath $backup) { Remove-Item -LiteralPath $backup -Recurse -Force }
        if (Test-Path -LiteralPath $destination) { Move-Item -LiteralPath $destination -Destination $backup }
        try {
            Move-Item -LiteralPath $libraryRoot -Destination $destination
            if (Test-Path -LiteralPath $backup) { Remove-Item -LiteralPath $backup -Recurse -Force }
        }
        catch {
            if (-not (Test-Path -LiteralPath $destination) -and (Test-Path -LiteralPath $backup)) {
                Move-Item -LiteralPath $backup -Destination $destination
            }
            throw
        }

        Write-Host "Installed verified library: $destination"
    }
}
finally {
    if (Test-Path -LiteralPath $tempRoot) { Remove-Item -LiteralPath $tempRoot -Recurse -Force }
}

Write-Host ''
Write-Host 'Meta Steel Guitar and Meta Nylon Guitar are CC0 1.0 collections.'
Write-Host 'Original SFZ maps, LICENSE files and SHA256SUMS are preserved.'
Write-Host 'In PPGU, use LOAD SFZ and select the desired .sfz map inside the installed library.'

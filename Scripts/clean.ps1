#Requires -Version 5.1
<#
.SYNOPSIS
    Removes all generated/intermediate folders so the next build starts from
    a clean state. Does NOT touch Config/, Source/, Content/ or save data.

.PARAMETER IncludeSaved
    Also delete the Saved/ folder (autosaves, logs, screenshots, per-user
    editor settings). Off by default so you don't accidentally wipe local
    save games.
#>

param(
    [switch]$IncludeSaved
)

$RepoRoot = Split-Path -Parent $PSScriptRoot

$FoldersToRemove = @(
    "Binaries",
    "Intermediate",
    "DerivedDataCache",
    "Packaged",
    ".vs"
)

if ($IncludeSaved) {
    $FoldersToRemove += "Saved"
}

foreach ($Folder in $FoldersToRemove) {
    $FullPath = Join-Path $RepoRoot $Folder
    if (Test-Path $FullPath) {
        Write-Host "Removing $FullPath ..." -ForegroundColor Yellow
        Remove-Item -Recurse -Force $FullPath
    }
}

$SlnFile = Join-Path $RepoRoot "Dustland.sln"
if (Test-Path $SlnFile) {
    Write-Host "Removing $SlnFile ..." -ForegroundColor Yellow
    Remove-Item -Force $SlnFile
}

Write-Host "Clean complete. Run Scripts/setup-windows.ps1 to regenerate project files." -ForegroundColor Green

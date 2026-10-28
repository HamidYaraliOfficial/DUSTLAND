#Requires -Version 5.1
<#
.SYNOPSIS
    Cooks and packages a distributable Windows build of Dustland using
    RunUAT.bat BuildCookRun (the same pipeline the Editor's "Package Project"
    menu item uses).

.PARAMETER EnginePath
    UE5 install root. Defaults to the UE_5_4_PATH user environment variable.

.PARAMETER Configuration
    Development or Shipping. Defaults to Shipping for a distributable build.

.PARAMETER OutputDir
    Where the packaged build is written. Defaults to ./Packaged relative to
    the repo root.
#>

param(
    [string]$EnginePath = $env:UE_5_4_PATH,
    [ValidateSet("Development", "Shipping")]
    [string]$Configuration = "Shipping",
    [string]$OutputDir
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$UProjectFile = Join-Path $RepoRoot "Dustland.uproject"

if (-not $OutputDir) {
    $OutputDir = Join-Path $RepoRoot "Packaged"
}

if (-not $EnginePath -or -not (Test-Path $EnginePath)) {
    Write-Host "Engine path not found. Run Scripts/setup-windows.ps1 first, or pass -EnginePath explicitly." -ForegroundColor Red
    exit 1
}

$RunUAT = Join-Path $EnginePath "Engine\Build\BatchFiles\RunUAT.bat"
if (-not (Test-Path $RunUAT)) {
    Write-Host "MISSING: RunUAT.bat not found under $EnginePath" -ForegroundColor Red
    exit 1
}

Write-Host "Packaging Dustland ($Configuration, Win64) to $OutputDir ..." -ForegroundColor Cyan

& $RunUAT BuildCookRun `
    -project="$UProjectFile" `
    -noP4 `
    -platform=Win64 `
    -clientconfig=$Configuration `
    -serverconfig=$Configuration `
    -cook -allmaps -build -stage -pak -archive `
    -archivedirectory="$OutputDir" `
    -CulturesToCook=en+fa+zh-Hans

if ($LASTEXITCODE -ne 0) {
    Write-Host "Packaging FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Packaging succeeded. Output at: $OutputDir" -ForegroundColor Green

#Requires -Version 5.1
<#
.SYNOPSIS
    Compiles Dustland in the Shipping configuration (Dustland game target,
    Win64) - the client executable, without the editor.

.PARAMETER EnginePath
    UE5 install root. Defaults to the UE_5_4_PATH user environment variable.
#>

param(
    [string]$EnginePath = $env:UE_5_4_PATH,
    [ValidateSet("Win64")]
    [string]$Platform = "Win64"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$UProjectFile = Join-Path $RepoRoot "Dustland.uproject"

if (-not $EnginePath -or -not (Test-Path $EnginePath)) {
    Write-Host "Engine path not found. Run Scripts/setup-windows.ps1 first, or pass -EnginePath explicitly." -ForegroundColor Red
    exit 1
}

$BuildScript = Join-Path $EnginePath "Engine\Build\BatchFiles\Build.bat"
if (-not (Test-Path $BuildScript)) {
    Write-Host "MISSING: Build.bat not found under $EnginePath" -ForegroundColor Red
    exit 1
}

Write-Host "Building Dustland ($Platform, Shipping)..." -ForegroundColor Cyan

& $BuildScript Dustland $Platform Shipping -Project="$UProjectFile" -WaitMutex

if ($LASTEXITCODE -ne 0) {
    Write-Host "Shipping build FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Shipping build succeeded." -ForegroundColor Green
Write-Host "For a distributable build (pak files, staged content), run Scripts/package.ps1 instead." -ForegroundColor Yellow

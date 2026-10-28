#Requires -Version 5.1
<#
.SYNOPSIS
    Compiles Dustland in the Development configuration (DustlandEditor target,
    Win64) using the engine's own Build.bat / UnrealBuildTool.

.PARAMETER EnginePath
    UE5 install root. Defaults to the UE_5_4_PATH user environment variable
    saved by setup-windows.ps1.
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

Write-Host "Building DustlandEditor ($Platform, Development)..." -ForegroundColor Cyan

& $BuildScript DustlandEditor $Platform Development -Project="$UProjectFile" -WaitMutex -FromMsBuild

if ($LASTEXITCODE -ne 0) {
    Write-Host "Build FAILED with exit code $LASTEXITCODE" -ForegroundColor Red
    exit $LASTEXITCODE
}

Write-Host "Build succeeded. Launch the editor with:" -ForegroundColor Green
Write-Host "  `"$EnginePath\Engine\Binaries\Win64\UnrealEditor.exe`" `"$UProjectFile`"" -ForegroundColor Green

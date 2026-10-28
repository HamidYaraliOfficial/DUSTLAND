#Requires -Version 5.1
<#
.SYNOPSIS
    Runs the Dustland automation test suite headlessly via UnrealEditor-Cmd
    and writes a JUnit-style report under Saved/Automation/Reports.

.PARAMETER TestFilter
    Automation test group to run. Defaults to the project's own tests
    (everything registered under the "Dustland." prefix - see Tests/).

.PARAMETER EnginePath
    UE5 install root. Defaults to the UE_5_4_PATH user environment variable.
#>

param(
    [string]$EnginePath = $env:UE_5_4_PATH,
    [string]$TestFilter = "Dustland"
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$UProjectFile = Join-Path $RepoRoot "Dustland.uproject"
$ReportDir = Join-Path $RepoRoot "Saved\Automation\Reports"

if (-not $EnginePath -or -not (Test-Path $EnginePath)) {
    Write-Host "Engine path not found. Run Scripts/setup-windows.ps1 first, or pass -EnginePath explicitly." -ForegroundColor Red
    exit 1
}

$EditorCmd = Join-Path $EnginePath "Engine\Binaries\Win64\UnrealEditor-Cmd.exe"
if (-not (Test-Path $EditorCmd)) {
    Write-Host "MISSING: UnrealEditor-Cmd.exe not found under $EnginePath. Build the editor target first." -ForegroundColor Red
    exit 1
}

New-Item -ItemType Directory -Force -Path $ReportDir | Out-Null

Write-Host "Running automation tests matching '$TestFilter'..." -ForegroundColor Cyan

$ExecCmds = "Automation RunTests $TestFilter;Quit"

& $EditorCmd "$UProjectFile" `
    -ExecCmds="$ExecCmds" `
    -TestExit="Automation Test Queue Empty" `
    -ReportOutputPath="$ReportDir" `
    -unattended -nopause -nosplash -log

$exitCode = $LASTEXITCODE
$ReportFile = Join-Path $ReportDir "index.json"

if (Test-Path $ReportFile) {
    Write-Host "Report written to $ReportFile" -ForegroundColor Green
} else {
    Write-Host "WARNING: no report file was produced; check the console log above for errors." -ForegroundColor Yellow
}

if ($exitCode -ne 0) {
    Write-Host "Automation run exited with code $exitCode (non-zero can mean failed tests OR a crash - check the log)." -ForegroundColor Red
} else {
    Write-Host "Automation run completed." -ForegroundColor Green
}

exit $exitCode

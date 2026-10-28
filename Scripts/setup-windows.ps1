#Requires -Version 5.1
<#
.SYNOPSIS
    Validates that a Windows 10/11 machine has everything required to build
    Dustland, and generates the Visual Studio project files.

.PARAMETER EnginePath
    Path to the Unreal Engine 5 install root, e.g.
    "C:\Program Files\Epic Games\UE_5.4". If omitted, the script tries to
    read it from the UE_5.4 environment variable, then from the standard
    Epic Games Launcher install location.

.EXAMPLE
    ./Scripts/setup-windows.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.4"
#>

param(
    [string]$EnginePath = $env:UE_5_4_PATH
)

$ErrorActionPreference = "Stop"
$RepoRoot = Split-Path -Parent $PSScriptRoot
$UProjectFile = Join-Path $RepoRoot "Dustland.uproject"

function Write-Section($Title) {
    Write-Host ""
    Write-Host "==== $Title ====" -ForegroundColor Cyan
}

function Test-Command($Name) {
    return [bool](Get-Command $Name -ErrorAction SilentlyContinue)
}

$AllOk = $true

Write-Section "1. Checking Git"
if (Test-Command "git") {
    $gitVersion = git --version
    Write-Host "OK: $gitVersion" -ForegroundColor Green
} else {
    Write-Host "MISSING: Git is not on PATH. Install from https://git-scm.com/download/win" -ForegroundColor Red
    $AllOk = $false
}

Write-Section "2. Checking Visual Studio 2022 + C++ workloads"
$vswhere = "${env:ProgramFiles(x86)}\Microsoft Visual Studio\Installer\vswhere.exe"
if (Test-Path $vswhere) {
    $vsInstalls = & $vswhere -latest -products * `
        -requires Microsoft.VisualStudio.Workload.NativeGame `
        -property installationPath
    if ($vsInstalls) {
        Write-Host "OK: Visual Studio with C++ Game Development workload found at:" -ForegroundColor Green
        Write-Host "     $vsInstalls"
    } else {
        Write-Host "MISSING: Visual Studio 2022 was found but the 'Game development with C++' workload is not installed." -ForegroundColor Red
        Write-Host "          Open Visual Studio Installer -> Modify -> check 'Game development with C++'" -ForegroundColor Yellow
        Write-Host "          and also enable the 'Desktop development with C++' workload." -ForegroundColor Yellow
        $AllOk = $false
    }
} else {
    Write-Host "MISSING: Visual Studio 2022 was not found. Install it from https://visualstudio.microsoft.com/" -ForegroundColor Red
    Write-Host "          During install, select workloads: 'Game development with C++' AND 'Desktop development with C++'." -ForegroundColor Yellow
    $AllOk = $false
}

Write-Section "3. Checking Windows SDK"
$sdkRoot = "HKLM:\SOFTWARE\WOW6432Node\Microsoft\Windows Kits\Installed Roots"
if (Test-Path $sdkRoot) {
    $sdkVersions = (Get-Item $sdkRoot).Property | Where-Object { $_ -like "KitsRoot*" }
    if ($sdkVersions) {
        Write-Host "OK: Windows SDK registry entries found." -ForegroundColor Green
    } else {
        Write-Host "MISSING: No Windows SDK detected. Install the latest Windows 10/11 SDK via Visual Studio Installer." -ForegroundColor Red
        $AllOk = $false
    }
} else {
    Write-Host "MISSING: Could not read Windows SDK registry key. Install the Windows SDK via Visual Studio Installer." -ForegroundColor Red
    $AllOk = $false
}

Write-Section "4. Checking Unreal Engine 5 install"
if (-not $EnginePath) {
    $defaultPath = "C:\Program Files\Epic Games\UE_5.4"
    if (Test-Path $defaultPath) {
        $EnginePath = $defaultPath
    }
}

if ($EnginePath -and (Test-Path (Join-Path $EnginePath "Engine\Build\BatchFiles\Build.bat"))) {
    Write-Host "OK: Unreal Engine 5 found at $EnginePath" -ForegroundColor Green
    [System.Environment]::SetEnvironmentVariable("UE_5_4_PATH", $EnginePath, "User")
    Write-Host "     Saved to user environment variable UE_5_4_PATH for future script runs." -ForegroundColor Green
} else {
    Write-Host "MISSING: Unreal Engine 5.4 was not found." -ForegroundColor Red
    Write-Host "          Install it via the Epic Games Launcher (Unreal Engine tab -> Library -> + -> 5.4)." -ForegroundColor Yellow
    Write-Host "          Then re-run this script with -EnginePath pointing at the install folder." -ForegroundColor Yellow
    $AllOk = $false
}

Write-Section "5. Checking project file"
if (Test-Path $UProjectFile) {
    Write-Host "OK: Found $UProjectFile" -ForegroundColor Green
} else {
    Write-Host "MISSING: Dustland.uproject not found at repo root ($RepoRoot)." -ForegroundColor Red
    $AllOk = $false
}

if (-not $AllOk) {
    Write-Host ""
    Write-Host "Setup incomplete. Resolve the issues above, then re-run this script." -ForegroundColor Red
    exit 1
}

Write-Section "6. Generating Visual Studio project files"
$UBT = Join-Path $EnginePath "Engine\Binaries\DotNET\UnrealBuildTool\UnrealBuildTool.exe"
$GenerateProjectFilesScript = Join-Path $EnginePath "Engine\Build\BatchFiles\GenerateProjectFiles.bat"

if (Test-Path $GenerateProjectFilesScript) {
    & $GenerateProjectFilesScript $UProjectFile
    if ($LASTEXITCODE -ne 0) {
        Write-Host "GenerateProjectFiles failed with exit code $LASTEXITCODE" -ForegroundColor Red
        exit $LASTEXITCODE
    }
    Write-Host "OK: Dustland.sln generated at repo root." -ForegroundColor Green
} else {
    Write-Host "MISSING: GenerateProjectFiles.bat not found under $EnginePath" -ForegroundColor Red
    exit 1
}

Write-Host ""
Write-Host "Setup complete. Open Dustland.sln in Visual Studio 2022, or run Scripts/build-development.ps1." -ForegroundColor Cyan

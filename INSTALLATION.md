# Dustland — Installation Guide (Windows 10/11)

This document walks through everything needed to go from a clean Windows
machine to a compiled, running Dustland editor session.

## 1. Prerequisites

| Tool | Version | Purpose |
|---|---|---|
| Epic Games Launcher | latest | Installs/updates Unreal Engine 5 |
| Unreal Engine | 5.4.x | Engine + Editor |
| Visual Studio | 2022 (17.8+) | C++ compiler toolchain |
| Windows SDK | 10.0.19041.0 or newer | Native Win32 build dependencies |
| Git | latest | Source control |

## 2. Install Visual Studio 2022

1. Download the installer: https://visualstudio.microsoft.com/downloads/
2. In the Workloads tab, check:
   - **Desktop development with C++**
   - **Game development with C++**
3. In the Individual Components tab, confirm the **Windows 10/11 SDK** is checked.
4. Click Install (or Modify if VS is already installed) and wait for completion.

## 3. Install Epic Games Launcher + Unreal Engine 5

1. Download and install the launcher: https://www.epicgames.com/store/download
2. Sign in / create a free Epic account.
3. Go to the **Unreal Engine** tab → **Library** → click **+** under Engine Versions.
4. Select **5.4** and click Install. Note the install path (default:
   `C:\Program Files\Epic Games\UE_5.4`).

## 4. Install Git

1. Download: https://git-scm.com/download/win
2. Install with default options.
3. Verify:
   ```powershell
   git --version
   ```

## 5. Get the source

Extract the provided archive (or place the `Dustland/` folder) anywhere on
disk, for example:

```
C:\Projects\Dustland
```

## 6. Validate the environment and generate project files

Open **PowerShell** in the repository root and run:

```powershell
Set-ExecutionPolicy -Scope Process -ExecutionPolicy Bypass
./Scripts/setup-windows.ps1 -EnginePath "C:\Program Files\Epic Games\UE_5.4"
```

This script checks Git, Visual Studio + workloads, the Windows SDK and the
Unreal Engine install, then generates `Dustland.sln`. Fix anything it
reports as `MISSING` and re-run.

## 7. Compile (Development / Editor)

Either:

- **Visual Studio**: open `Dustland.sln`, set the solution configuration to
  `Development Editor` / `Win64`, set `Dustland` as the startup project,
  press **Ctrl+Shift+B** to build, then **F5** to launch the editor with the
  debugger attached; **or**
- **Script**:
  ```powershell
  ./Scripts/build-development.ps1
  ```

## 8. Run the Editor

```powershell
& "C:\Program Files\Epic Games\UE_5.4\Engine\Binaries\Win64\UnrealEditor.exe" "C:\Projects\Dustland\Dustland.uproject"
```

## 9. Run automated tests

```powershell
./Scripts/run-tests.ps1
```

Reads results from `Saved/Automation/Reports/index.json`.

## 10. Shipping build

```powershell
./Scripts/build-shipping.ps1
```

## 11. Package a distributable build

```powershell
./Scripts/package.ps1 -Configuration Shipping
```

Output is written to `Packaged/` (cooked content for English, Persian and
Chinese cultures, staged and archived as pak files).

## 12. Clean

```powershell
./Scripts/clean.ps1            # removes Binaries/Intermediate/DerivedDataCache/Packaged/.vs
./Scripts/clean.ps1 -IncludeSaved   # also wipes Saved/ (local saves, logs)
```

## Troubleshooting

- **"UnrealBuildTool failed" during GenerateProjectFiles**: confirm the
  Windows SDK version selected in Visual Studio matches one installed on
  the machine (Visual Studio Installer → Individual Components).
- **Missing `.sln` file**: re-run `Scripts/setup-windows.ps1`; it must
  complete with all checks `OK` before project files are generated.
- **Editor won't open the project ("modules are missing")**: the C++
  modules haven't been compiled yet — run `Scripts/build-development.ps1`
  first, then open the `.uproject` again (accept the "rebuild" prompt if UE
  shows one).
- **Packaging fails on Localization step**: confirm `Config/DefaultGame.ini`
  still lists `en`, `fa`, `zh-Hans` under `[Internationalization]`.

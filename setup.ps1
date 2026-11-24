$ErrorActionPreference = "Stop"
$Root = Split-Path -Parent $MyInvocation.MyCommand.Path
$Viewer = Join-Path $Root "build\GraphVisualizerViewer.exe"
$Compiler = Join-Path $Root "toolchain\mingw\bin\g++.exe"
$Desktop = [Environment]::GetFolderPath("Desktop")
$ShortcutPath = Join-Path $Desktop "GraphVisualizer.lnk"

if (!(Test-Path $Viewer)) { throw "Viewer is missing: $Viewer" }
if (!(Test-Path $Compiler)) { throw "Bundled MinGW compiler is missing: $Compiler" }
if (!(Test-Path (Join-Path $Root "client\GraphVisualizer.h"))) { throw "Client headers are missing." }

$Wsh = New-Object -ComObject WScript.Shell
$Shortcut = $Wsh.CreateShortcut($ShortcutPath)
$Shortcut.TargetPath = Join-Path $Root "GraphVisualizerLauncher.bat"
$Shortcut.WorkingDirectory = $Root
$Shortcut.Description = "Launch GraphVisualizer, select a C++ file, build and run it"
$Shortcut.Save()

Write-Host ""
Write-Host "[OK] GraphVisualizer portable setup completed."
Write-Host "Desktop shortcut created: $ShortcutPath"
Write-Host "Project location: $Root"

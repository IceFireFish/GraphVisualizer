@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0GraphVisualizerLauncher.ps1"
if errorlevel 1 (
  echo.
  echo [ERROR] Launcher failed.
  pause
)

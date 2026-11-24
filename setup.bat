@echo off
powershell.exe -NoProfile -ExecutionPolicy Bypass -File "%~dp0setup.ps1"
if errorlevel 1 (
  echo.
  echo [ERROR] Setup failed.
  pause
  exit /b 1
)
echo.
pause

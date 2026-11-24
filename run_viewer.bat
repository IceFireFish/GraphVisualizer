@echo off
setlocal
set "ROOT=%~dp0"
set "VIEWER=%ROOT%build\GraphVisualizerViewer.exe"
if not exist "%VIEWER%" (
  echo [ERROR] GraphVisualizerViewer.exe is missing.
  pause
  exit /b 1
)
start "" "%VIEWER%"

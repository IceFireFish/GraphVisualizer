@echo off
setlocal
set "ROOT=%~dp0"
set "EXAMPLES=%ROOT%examples"
set "PROGRAM=%ROOT%build\MyGraph.exe"
if not exist "%PROGRAM%" (
  echo [ERROR] MyGraph.exe not found. Run build_mygraph.bat first.
  pause
  exit /b 1
)
if not exist "%EXAMPLES%\file.in" (
  echo [ERROR] examples\file.in not found.
  pause
  exit /b 1
)
cd /d "%EXAMPLES%" || exit /b 1
"%PROGRAM%"
echo.
echo [INFO] Output file: "%EXAMPLES%\file.out"
pause

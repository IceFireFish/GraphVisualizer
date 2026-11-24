@echo off
setlocal
set "ROOT=%~dp0"
set "MINGW_BIN=%ROOT%toolchain\mingw\bin"
set "OUT_DIR=%ROOT%build"
set "OUT_EXE=%OUT_DIR%\MyGraph.exe"

if not exist "%MINGW_BIN%\g++.exe" (
  echo [ERROR] Bundled g++.exe is missing.
  pause
  exit /b 1
)
if not exist "%ROOT%examples\main.cpp" (
  echo [ERROR] examples\main.cpp is missing.
  pause
  exit /b 1
)
if not exist "%ROOT%client\GraphVisualizer.cpp" (
  echo [ERROR] client\GraphVisualizer.cpp is missing.
  pause
  exit /b 1
)

set "PATH=%MINGW_BIN%;%PATH%"
echo [BUILD] Compiling examples\main.cpp ...
"%MINGW_BIN%\g++.exe" -std=c++17 -O2 -finput-charset=UTF-8 -fexec-charset=UTF-8 -I"%ROOT%client" "%ROOT%examples\main.cpp" "%ROOT%client\GraphVisualizer.cpp" -o "%OUT_EXE%" -lws2_32
if errorlevel 1 (
  echo.
  echo [FAILED] Compilation failed.
  pause
  exit /b 1
)
echo.
echo [OK] Build successful: "%OUT_EXE%"
pause

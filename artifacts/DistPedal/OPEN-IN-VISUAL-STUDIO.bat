@echo off
title DistPedal — Visual Studio 2026 (Windows 11)
cd /d "%~dp0"

where cmake >nul 2>&1
if errorlevel 1 (
  echo CMake not on PATH. Visual Studio 2026 ships one at:
  echo   "C:\Program Files\Microsoft Visual Studio\18\Professional\Common7\IDE\CommonExtensions\Microsoft\CMake\CMake\bin\cmake.exe"
  echo Add that folder to PATH, or open DistPedal in VS 2026 with File - Open - CMake...
  pause
  exit /b 1
)

echo Generating DistPedal.sln for Visual Studio 2026 x64 ...
cmake -B Builds\CMakeVS2026 -G "Visual Studio 18 2026" -A x64
if errorlevel 1 (
  echo.
  echo Generator "Visual Studio 18 2026" failed.
  echo Need CMake 4.2+ ^(VS 2026's bundled CMake is fine^).
  echo Falling back to Visual Studio 17 2022 generator — VS 2026 can still open that .sln.
  cmake -B Builds\CMakeVS2022 -G "Visual Studio 17 2022" -A x64
  if errorlevel 1 (
    echo CMake failed. In VS 2026: File - Open - CMake...  and pick this DistPedal folder.
    pause
    exit /b 1
  )
  start "" "Builds\CMakeVS2022\DistPedal.sln"
  goto :help
)

echo.
echo Opening DistPedal.sln in Visual Studio 2026
start "" "Builds\CMakeVS2026\DistPedal.sln"

:help
echo.
echo In Visual Studio 2026:
echo   1. Workload required: Desktop development with C++
echo   2. Toolbar: Release ^| x64
echo   3. Solution Explorer - DistPedal_VST3 - Set as Startup Project
echo   4. Build - Build DistPedal_VST3
echo   5. Output:
echo      Builds\CMakeVS2026\DistPedal_artefacts\Release\VST3\DistPedal.vst3
echo   6. FL Studio: Options - Manage plugins - Find plugins - type Effect
echo.
pause

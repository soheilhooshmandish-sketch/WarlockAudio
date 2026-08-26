@echo off
setlocal
cd /d "%~dp0"
cmake --preset vs2022-x64
if errorlevel 1 exit /b 1
cmake --build --preset warlock-release
if errorlevel 1 exit /b 1
echo.
echo VST3: build\Warlock_artefacts\Release\VST3\Warlock.vst3
echo Next: compile installer.iss  OR  point your DAW at that folder.

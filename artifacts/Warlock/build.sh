#!/usr/bin/env bash
set -e
echo "=== WARLOCK SATANIC BLACK METAL PIPELINE RUNNER ==="
# 1. Verification of Local Directories
mkdir -p Resources BuildInstaller
# 2. CMake Build Template Generation
echo "[1/3] Generating build system tree templates via CMake..."
cmake -B build -G "Visual Studio 17 2022" -A x64
# 3. Compiling Binary Executable Blocks
echo "[2/3] Processing high-optimization Satanic C++ binary artifacts..."
cmake --build build --config Release --target Warlock
# 4. Inno Setup Structural Manifest Generation Pass
if command -v ISCC &> /dev/null
then
    echo "[3/3] Compiling target installation binary templates via ISCC..."
    ISCC installer_production.iss
else
    echo "[3/3] System Warning: Inno Setup (ISCC) engine not found on local PATH. Skipping installer packaging."
fi
echo "=== SATANIC PIPELINE COMPLETION ==="

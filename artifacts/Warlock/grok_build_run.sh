#!/usr/bin/env bash
set -e
echo "=== GLOBAL WARLOCK SATANIC BLACK METAL PIPELINE RUNNER ==="
# 1. Native Python Resource Initialization Loop
if command -v python3 &> /dev/null
then
    python3 generate_resources.py
else
    echo "Python3 engine not detected. Ensure assets directory exists manually."
fi
# 2. CMake Build System Blueprint Generation Pass
echo "[1/3] Generating build system tree templates via CMake..."
cmake -B build -G "Visual Studio 17 2022" -A x64
# 3. Compile Native Optimized Binary Executable Blocks
echo "[2/3] Processing high-optimization Satanic C++ binary artifacts..."
cmake --build build --config Release --target Warlock
# 4. Final Distribution Package Compilation
if command -v ISCC &> /dev/null
then
    echo "[3/3] Compiling target installation binary templates via ISCC..."
    ISCC installer_production.iss
else
    echo "[3/3] System Warning: Inno Setup (ISCC) engine not found on local PATH. Skipping installer packaging."
fi
echo "=== ALL WARLOCK SUITE COMPONENT JOBS COMPLETED ==="

# Build script for Windows WASM build
# Sets paths and environment
$ROOT_DIR = Split-Path -Parent $MyInvocation.MyCommand.Path
$BUILD_DIR = Join-Path $ROOT_DIR "build_wasm"
$EMSDK_DIR = Join-Path $ROOT_DIR "Dependencies\emsdk"

# Activate Emscripten environment
$env:PATH = "$EMSDK_DIR;$env:PATH"
& "$EMSDK_DIR\emsdk.ps1" activate latest

# Create and enter build directory
New-Item -ItemType Directory -Force -Path $BUILD_DIR | Out-Null
Push-Location $BUILD_DIR

# Configure with CMake using Emscripten
cmake -G "MinGW Makefiles" `
    -DCMAKE_TOOLCHAIN_FILE="$EMSDK_DIR\upstream\emscripten\cmake\Modules\Platform\Emscripten.cmake" `
    -DCMAKE_BUILD_TYPE=Release `
    -DBUILD_WASM=ON `
    ..

# Build
cmake --build . --config Release

# Copy output files to js directory
$JS_DIR = Join-Path $ROOT_DIR "js"
New-Item -ItemType Directory -Force -Path $JS_DIR | Out-Null
Copy-Item "$BUILD_DIR\aura_tokenizer*" -Destination $JS_DIR

Write-Host "Build complete! Output files are in the js directory."

# Return to original directory
Pop-Location

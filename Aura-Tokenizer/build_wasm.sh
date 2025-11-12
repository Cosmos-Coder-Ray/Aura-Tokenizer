#!/bin/bash

# Set paths
ROOT_DIR=$(dirname "$(readlink -f "$0")")
BUILD_DIR="$ROOT_DIR/build_wasm"
EMSDK_DIR="$ROOT_DIR/Dependencies/emsdk"

# Activate Emscripten
source "$EMSDK_DIR/emsdk_env.sh"

# Create and enter build directory
mkdir -p "$BUILD_DIR"
cd "$BUILD_DIR"

# Configure with CMake using Emscripten
cmake -G "Unix Makefiles" \
    -DCMAKE_TOOLCHAIN_FILE="$EMSDK_DIR/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake" \
    -DCMAKE_BUILD_TYPE=Release \
    -DBUILD_WASM=ON \
    ..

# Build
cmake --build . --config Release

# Copy output files to js directory
mkdir -p "$ROOT_DIR/js"
cp "$BUILD_DIR/aura_tokenizer"* "$ROOT_DIR/js/"

echo "Build complete! Output files are in the js directory."

@echo off
setlocal

:: Set paths
set ROOT_DIR=%~dp0
set BUILD_DIR=%ROOT_DIR%build_wasm
set EMSDK_DIR=%ROOT_DIR%Dependencies\emsdk

:: Activate Emscripten
call %EMSDK_DIR%\emsdk_env.bat

:: Create and enter build directory
if not exist %BUILD_DIR% mkdir %BUILD_DIR%
cd %BUILD_DIR%

:: Configure with CMake using Emscripten
cmake -G "MinGW Makefiles" ^
    -DCMAKE_TOOLCHAIN_FILE=%EMSDK_DIR%/upstream/emscripten/cmake/Modules/Platform/Emscripten.cmake ^
    -DCMAKE_BUILD_TYPE=Release ^
    -DBUILD_WASM=ON ^
    ..

:: Build
cmake --build . --config Release

:: Copy output files to js directory
if not exist %ROOT_DIR%js mkdir %ROOT_DIR%js
copy /Y %BUILD_DIR%\aura_tokenizer.* %ROOT_DIR%js\

echo Build complete! Output files are in the js directory.

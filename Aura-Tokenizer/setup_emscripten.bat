@echo off
setlocal enabledelayedexpansion

:: Check for required tools
where cmake >nul 2>&1
if %ERRORLEVEL% NEQ 0 (
    echo CMake is not installed or not in PATH
    echo Please install CMake from https://cmake.org/download/
    echo After installing CMake, run this script again
    pause
    exit /b 1
)

:: Check if EMSDK environment variable is set
if "%EMSDK%"=="" (
    echo EMSDK environment variable not set. Setting up Emscripten...
    
    :: Navigate to the Dependencies directory
    cd Dependencies\emsdk
    
    :: Install and activate latest version
    call emsdk.bat install latest
    call emsdk.bat activate latest
    
    :: Add to PATH
    call emsdk_env.bat
    
    cd ..\..
) else (
    echo Emscripten already configured. EMSDK=%EMSDK%
)

:: Ensure build directory exists
if not exist "build" mkdir build

:: Configure CMake
echo Configuring CMake...
cd build
cmake -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -DBUILD_WASM=ON .. || (
    echo CMake configuration failed
    cd ..
    exit /b 1
)
cd ..

:: Create .vscode directory if it doesn't exist
if not exist ".vscode" mkdir .vscode

:: Create c_cpp_properties.json
echo Creating c_cpp_properties.json...
(
    echo {
    echo     "configurations": [
    echo         {
    echo             "name": "Win32",
    echo             "includePath": [
    echo                 "${workspaceFolder}/**",
    echo                 "${workspaceFolder}/Dependencies/emsdk/upstream/emscripten/system/include",
    echo                 "${workspaceFolder}/Dependencies/emsdk/upstream/emscripten/cache/sysroot/include",
    echo                 "${workspaceFolder}/Dependencies/emsdk/upstream/lib/include"
    echo             ],
    echo             "defines": [
    echo                 "__EMSCRIPTEN__"
    echo             ],
    echo             "compilerPath": "cl.exe",
    echo             "cStandard": "c17",
    echo             "cppStandard": "c++20",
    echo             "intelliSenseMode": "windows-msvc-x64",
    echo             "configurationProvider": "ms-vscode.cmake-tools"
    echo         }
    echo     ],
    echo     "version": 4
    echo }
) > .vscode\c_cpp_properties.json

:: Verify required files exist
echo Verifying Emscripten setup...
if not exist "%EMSDK%\upstream\emscripten\system\include\emscripten\val.h" (
    echo Warning: Emscripten header files not found at expected location
    echo Expected: %EMSDK%\upstream\emscripten\system\include\emscripten\val.h
)

:: Copy compile_commands.json if it exists
if exist "build\compile_commands.json" (
    copy "build\compile_commands.json" "compile_commands.json"
    echo Copied compile_commands.json for better IntelliSense
)

echo.
echo Setup complete!
echo.
echo Please install these VS Code extensions for better C++ and CMake support:
echo - C/C++ Extension Pack
echo - CMake Tools
echo - CMake Language Support
echo.
pause

cmake_minimum_required(VERSION 3.16)
project(AuraTokenizer VERSION 1.0.0 LANGUAGES CXX)

# Set C++ standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Coverage options
option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

# Platform-specific settings
if(WIN32)
    add_definitions(-DWIN32_LEAN_AND_MEAN -D_CRT_SECURE_NO_WARNINGS)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /W4")
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /O0 /profile-instr-generate /profile-instr-use")
    endif()
else()
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Wpedantic")
    if(ENABLE_COVERAGE)
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O0 -g --coverage -fprofile-arcs -ftest-coverage")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} --coverage")
    endif()
endif()

# Find Python (optional)
find_package(Python3 COMPONENTS Interpreter Development QUIET)
if(Python3_FOUND)
    message(STATUS "Python3 found. Python bindings will be enabled.")
    set(BUILD_PYTHON_BINDINGS ON)
else()
    message(WARNING "Python3 not found. Python bindings will be disabled.")
    set(BUILD_PYTHON_BINDINGS OFF)
endif()

# Find ICU
find_package(PkgConfig QUIET)
if(PkgConfig_FOUND)
    pkg_check_modules(ICU REQUIRED icu-uc icu-i18n)
    if(ICU_FOUND)
        message(STATUS "ICU found via pkg-config")
        set(ICU_INCLUDE_DIRS ${ICU_INCLUDE_DIRS})
        set(ICU_LIBRARIES ${ICU_LIBRARIES})
    endif()
endif()

# Fallback to bundled ICU if not found
if(NOT ICU_FOUND)
    message(STATUS "Using bundled ICU")
    set(ICU_INCLUDE_DIRS "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/icu77.1/include")
    set(ICU_LIBRARIES 
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/icu77.1/lib64/icuuc.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/icu77.1/lib64/icuin.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/icu77.1/lib64/icuio.lib"
        "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/icu77.1/lib64/icutu.lib"
    )
endif()

# Check for Emscripten toolchain
if(CMAKE_TOOLCHAIN_FILE MATCHES "Emscripten")
    set(BUILD_WASM ON)
    message(STATUS "Emscripten toolchain detected. Building for WebAssembly.")

    # Set Emscripten-specific compiler flags
    set(EMSCRIPTEN_FLAGS
        "-s WASM=1"
        "-s USE_ICU=1"
        "-s MODULARIZE=1"
        "-s EXPORT_NAME='AuraTokenizerModule'"
        "-s EXPORTED_RUNTIME_METHODS=['UTF8ToString','stringToUTF8','ccall','cwrap']"
        "-s EXPORTED_FUNCTIONS=['_malloc','_free']"
        "-s ALLOW_MEMORY_GROWTH=1"
        "-s NO_EXIT_RUNTIME=1"
        "-s EXPORT_ES6=1"
        "-s USE_ES6_IMPORT_META=0"
        "-s ENVIRONMENT='web,worker'"
        "-s DYNAMIC_EXECUTION=0"
        "-s DISABLE_EXCEPTION_CATCHING=0"  # Enable for JSON exception handling
        "-s DISABLE_EXCEPTION_THROWING=0"  # Enable for JSON exception handling
        "-O3"
        "--no-entry"
        "--bind"
    )

    # Convert list to string with spaces
    string(JOIN " " EMSCRIPTEN_FLAGS_STR ${EMSCRIPTEN_FLAGS})
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${EMSCRIPTEN_FLAGS_STR}")

    # Add Emscripten bindings source
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer_wasm.cpp")

    # Add Emscripten include directories
    if(EXISTS "$ENV{EMSDK}")
        set(EMSCRIPTEN_INCLUDE_DIRS 
            "$ENV{EMSDK}/upstream/emscripten/system/include"
            "$ENV{EMSDK}/upstream/emscripten/cache/sysroot/include"
        )
    else()
        set(EMSCRIPTEN_INCLUDE_DIRS 
            "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/emsdk/upstream/emscripten/system/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/emsdk/upstream/emscripten/cache/sysroot/include"
            "${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/emsdk/upstream/lib/include"
        )
    endif()
    
    if(NOT EXISTS "${EMSCRIPTEN_INCLUDE_DIRS}")
        message(FATAL_ERROR "Emscripten include directories not found. Please ensure EMSDK is properly installed and configured.")
    endif()
    
    include_directories(${EMSCRIPTEN_INCLUDE_DIRS})

    # Set output file extensions for WASM build
    set(CMAKE_EXECUTABLE_SUFFIX ".js")
    set(CMAKE_SHARED_LIBRARY_SUFFIX ".wasm")
else()
    set(BUILD_WASM OFF)
endif()

# Source files
file(GLOB_RECURSE SOURCES
    "src/*.cpp"
)

if(NOT BUILD_WASM)
    list(FILTER SOURCES EXCLUDE REGEX ".*/tokenizer_wasm.cpp")
    list(FILTER SOURCES EXCLUDE REGEX ".*/vector_conversions.cpp")
endif()

file(GLOB_RECURSE HEADERS
    "include/*.h"
    "include/byte_level_pre_tokenizer.h"
    "include/tokenizer_json_parser.h"
    "include/wordpiece_model.h"
    "include/template_parser.h"
    "include/unigram_tokenizer.h"
    "include/char_level_tokenizer.h"
    "include/wordpiece_tokenizer.h"
)

# Test files
file(GLOB_RECURSE TEST_SOURCES
    "tests/*.cpp"
)

# Optional testing setup
option(BUILD_TESTS "Build test suite" OFF)

if(BUILD_TESTS)
    enable_testing()
    
    # Find GTest
    find_package(GTest)
    
    if(GTest_FOUND)
        message(STATUS "GTest found. Building tests.")
        # Add test executable
        add_executable(auratokenizer_tests ${TEST_SOURCES})
        target_link_libraries(auratokenizer_tests 
            PRIVATE
            auratokenizer
            GTest::GTest 
            GTest::Main
        )
    else()
        message(STATUS "GTest not found. Tests will be disabled.")
    endif()
endif()

# Header files
# Conditionally add WASM specific files
if(BUILD_WASM)
    list(APPEND SOURCES "${CMAKE_CURRENT_SOURCE_DIR}/src/tokenizer_wasm.cpp")
    list(APPEND HEADERS "${CMAKE_CURRENT_SOURCE_DIR}/include/tokenizer_wasm.h")
endif()

if(BUILD_WASM)
    add_library(auratokenizer_wasm STATIC 
        src/tokenizer_wasm.cpp
        src/vector_conversions.cpp
    )
    target_include_directories(auratokenizer_wasm
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${EMSCRIPTEN_INCLUDE_DIRS}
            ${ICU_INCLUDE_DIRS}
    )
    set_target_properties(auratokenizer_wasm PROPERTIES
        LINK_FLAGS "${CMAKE_CXX_FLAGS}"
        OUTPUT_NAME "aura_tokenizer_wasm"
    )
    target_link_libraries(auratokenizer_wasm PRIVATE nlohmann_json)
endif()

# Create library target
if(BUILD_WASM)
    add_library(auratokenizer STATIC ${SOURCES})
    target_include_directories(auratokenizer
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${EMSCRIPTEN_INCLUDE_DIRS}
            ${ICU_INCLUDE_DIRS}
    )
    set_target_properties(auratokenizer PROPERTIES
        LINK_FLAGS "${CMAKE_CXX_FLAGS}"
        OUTPUT_NAME "aura_tokenizer"
    )
    target_link_libraries(auratokenizer PRIVATE auratokenizer_wasm)
else()
    add_library(auratokenizer SHARED ${SOURCES} ${HEADERS})
    target_include_directories(auratokenizer
        PUBLIC
            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:include>
        PRIVATE
            ${ICU_INCLUDE_DIRS}
    )
    if(WIN32)
        set_target_properties(auratokenizer PROPERTIES
            WINDOWS_EXPORT_ALL_SYMBOLS ON
        )
    endif()
    
    # Add any necessary Emscripten specific include directories or libraries here
    # For example, if you need to link to specific Emscripten system libraries
    # target_link_libraries(auratokenizer PRIVATE ${EMSCRIPTEN_LIBRARIES})
endif()

# Configure JSON library
set(JSON_BuildTests OFF CACHE INTERNAL "")
add_library(nlohmann_json INTERFACE)
target_include_directories(nlohmann_json INTERFACE 
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/json/single_include>
)

# Set additional include directories
target_include_directories(auratokenizer 
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
        ${CMAKE_CURRENT_SOURCE_DIR}/Dependencies/emsdk/upstream/emscripten/system/include
)

# Link libraries
target_link_libraries(auratokenizer 
    PRIVATE 
        ${ICU_LIBRARIES}
        nlohmann_json
)

# Compiler-specific flags
target_compile_definitions(auratokenizer 
    PRIVATE 
        NOMINMAX
        NPY_NO_DEPRECATED_API=NPY_1_7_API_VERSION
)

# Python bindings (if enabled)
if(BUILD_PYTHON_BINDINGS)
    include("C:/Users/rstiw/Desktop/AURA/python/Lib/site-packages/pybind11/share/cmake/pybind11/pybind11Config.cmake")
    pybind11_add_module(auratokenizer_python python/tokenizer_python.cpp)
    target_link_libraries(auratokenizer_python PRIVATE auratokenizer)
    target_include_directories(auratokenizer_python PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
    
    # Ensure the Python module can find the DLL at runtime
    if(WIN32)
        set_target_properties(auratokenizer_python PROPERTIES
            RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/Release
        )
    endif()
endif()

# Installation
install(TARGETS auratokenizer
    EXPORT AuraTokenizerTargets
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    RUNTIME DESTINATION bin
    INCLUDES DESTINATION include
)

install(DIRECTORY include/
    DESTINATION include
    FILES_MATCHING PATTERN "*.h"
)

install(EXPORT AuraTokenizerTargets
    FILE AuraTokenizerTargets.cmake
    NAMESPACE AuraTokenizer::
    DESTINATION lib/cmake/AuraTokenizer
)


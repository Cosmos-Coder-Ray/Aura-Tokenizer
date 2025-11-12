cmake_minimum_required(VERSION 3.10)

project(AuraTokenizerTests)

# --- Find Dependencies ---
# Find GTest
find_package(GTest REQUIRED)

# --- Configure Project ---
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Add the include directory of the core library to the search path
include_directories(../include)

# Add the library directory to the search path
# Assuming the pre-built library is in Aura-Tokenizer/Aura-Tokenizer/lib/
link_directories(../lib)


# --- Define Test Executable ---
add_executable(run_aura_tests
    test_bpe_model.cpp
    test_unigram_model.cpp
    test_wordpiece_model.cpp
    test_trainer.cpp
    test_unicode.cpp
)

# --- Link Libraries ---
target_link_libraries(run_aura_tests
    GTest::GTest
    GTest::Main
    auratokenizer # The name of the pre-built core library
)

# --- Enable and Discover Tests ---
include(GoogleTest)
gtest_discover_tests(run_aura_tests)

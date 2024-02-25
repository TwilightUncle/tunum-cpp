cmake_minimum_required(VERSION 3.22)

project(tunum CXX)
add_library(tunum INTERFACE)
target_include_directories(tunum INTERFACE include)
target_compile_features(tunum INTERFACE cxx_std_20)

# -----------------------------------------------
# テストとかサンプルとか
# -----------------------------------------------
if (CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED True)
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/utf-8>")
    add_compile_options("$<$<CXX_COMPILER_ID:MSVC>:/Zc:__cplusplus>")
    enable_testing()
    add_subdirectory(test)
endif ()

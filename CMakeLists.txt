cmake_minimum_required(VERSION 3.21)
project(structures_and_algorithms VERSION 0.1.0 LANGUAGES CXX)

option(SAA_BUILD_EXAMPLES "Build example programs" ON)
option(SAA_BUILD_BENCHMARKS "Build Google Benchmark programs" OFF)

add_library(saa INTERFACE)
add_library(saa::saa ALIAS saa)
target_include_directories(saa INTERFACE
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
    $<INSTALL_INTERFACE:include>
)
target_compile_features(saa INTERFACE cxx_std_20)

include(CTest)
if(BUILD_TESTING)
    add_subdirectory(tests)
endif()

if(SAA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

if(SAA_BUILD_BENCHMARKS)
    include(FetchContent)
    set(BENCHMARK_ENABLE_TESTING OFF CACHE BOOL "" FORCE)
    FetchContent_Declare(
        googlebenchmark
        URL https://github.com/google/benchmark/archive/refs/tags/v1.9.4.tar.gz
    )
    FetchContent_MakeAvailable(googlebenchmark)
    add_subdirectory(benchmarks)
endif()

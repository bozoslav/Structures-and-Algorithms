find_package(Threads REQUIRED)

add_executable(slot_map_benchmarks slot_map_benchmarks.cpp)
target_link_libraries(slot_map_benchmarks PRIVATE saa::saa benchmark::benchmark)

add_executable(spsc_ring_buffer_benchmarks spsc_ring_buffer_benchmarks.cpp)
target_link_libraries(spsc_ring_buffer_benchmarks PRIVATE saa::saa benchmark::benchmark Threads::Threads)

if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
    target_compile_options(slot_map_benchmarks PRIVATE -O3)
    target_compile_options(spsc_ring_buffer_benchmarks PRIVATE -O3)
endif()

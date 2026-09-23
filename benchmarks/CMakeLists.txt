add_executable(slot_map_benchmarks slot_map_benchmarks.cpp)
target_link_libraries(slot_map_benchmarks PRIVATE saa::saa benchmark::benchmark)

find_package(Threads REQUIRED)

add_executable(slot_map_example slot_map_example.cpp)
target_link_libraries(slot_map_example PRIVATE saa::saa)

add_executable(spsc_ring_buffer_example spsc_ring_buffer_example.cpp)
target_link_libraries(spsc_ring_buffer_example PRIVATE saa::saa Threads::Threads)

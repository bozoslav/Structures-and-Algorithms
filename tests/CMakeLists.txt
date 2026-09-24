find_package(Threads REQUIRED)

add_executable(slot_map_tests
    data_structures/slot_map_tests.cpp
)
target_link_libraries(slot_map_tests PRIVATE saa::saa)
add_test(NAME slot_map_tests COMMAND slot_map_tests)

add_executable(spsc_ring_buffer_tests
    data_structures/spsc_ring_buffer_tests.cpp
)
target_link_libraries(spsc_ring_buffer_tests PRIVATE saa::saa Threads::Threads)
add_test(NAME spsc_ring_buffer_tests COMMAND spsc_ring_buffer_tests)

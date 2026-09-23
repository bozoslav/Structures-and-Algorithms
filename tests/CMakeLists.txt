add_executable(slot_map_tests
    data_structures/slot_map_tests.cpp
)
target_link_libraries(slot_map_tests PRIVATE saa::saa)
add_test(NAME slot_map_tests COMMAND slot_map_tests)

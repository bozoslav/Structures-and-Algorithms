#include "saa/data_structures/spsc_ring_buffer.hpp"

#include <cstddef>
#include <iostream>
#include <thread>

int main() {
    constexpr std::size_t item_count = 10;
    saa::SpscRingBuffer<int> buffer(4);

    std::thread producer([&buffer, item_count] {
        for (int value = 1; value <= static_cast<int>(item_count); ++value) {
            while (!buffer.push(value)) {
                std::this_thread::yield();
            }
        }
    });

    std::cout << "Consumer received:";
    for (std::size_t i = 0; i < item_count; ++i) {
        auto value = buffer.pop();
        while (!value) {
            std::this_thread::yield();
            value = buffer.pop();
        }
        std::cout << ' ' << *value;
    }
    std::cout << '\n';

    producer.join();
    return 0;
}

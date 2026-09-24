#include "saa/data_structures/spsc_ring_buffer.hpp"

#include <atomic>
#include <chrono>
#include <cstddef>
#include <cstdlib>
#include <iostream>
#include <thread>

namespace {

int failures = 0;

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "Line " << line << ": check failed: " << expression << '\n';
        ++failures;
    }
}

#define CHECK(expression) check(static_cast<bool>(expression), #expression, __LINE__)

void test_new_buffer_is_empty() {
    saa::SpscRingBuffer<int> buffer(3);

    CHECK(buffer.getCapacity() == 4);
    CHECK(!buffer.pop().has_value());
}

void test_full_buffer_rejects_push() {
    saa::SpscRingBuffer<int> buffer(2);

    CHECK(buffer.push(10));
    CHECK(buffer.push(20));
    CHECK(!buffer.push(30));
}

void test_values_are_popped_in_fifo_order() {
    saa::SpscRingBuffer<int> buffer(3);

    CHECK(buffer.push(10));
    CHECK(buffer.push(20));
    CHECK(buffer.push(30));

    const auto first = buffer.pop();
    const auto second = buffer.pop();
    const auto third = buffer.pop();

    CHECK(first == 10);
    CHECK(second == 20);
    CHECK(third == 30);
    CHECK(!buffer.pop().has_value());
}

void test_buffer_wraps_around_and_reuses_freed_slots() {
    saa::SpscRingBuffer<int> buffer(4);

    CHECK(buffer.push(1));
    CHECK(buffer.push(2));
    CHECK(buffer.push(3));
    CHECK(buffer.push(4));
    CHECK(!buffer.push(5));

    const auto first = buffer.pop();
    CHECK(first == 1);

    CHECK(buffer.push(5));
    CHECK(!buffer.push(6));

    const auto second = buffer.pop();
    const auto third = buffer.pop();
    const auto fourth = buffer.pop();
    const auto fifth = buffer.pop();

    CHECK(second == 2);
    CHECK(third == 3);
    CHECK(fourth == 4);
    CHECK(fifth == 5);
    CHECK(!buffer.pop().has_value());
}

void test_one_producer_and_one_consumer_preserve_fifo_order() {
    constexpr std::size_t item_count = 100'000;
    constexpr auto timeout = std::chrono::seconds(10);

    saa::SpscRingBuffer<int> buffer(64);
    std::atomic<bool> stop{false};
    std::atomic<bool> timed_out{false};
    const auto deadline = std::chrono::steady_clock::now() + timeout;
    std::size_t received = 0;
    std::size_t order_errors = 0;

    std::thread producer([&] {
        for (std::size_t value = 0; value < item_count;) {
            if (stop.load(std::memory_order_relaxed)) return;

            if (buffer.push(static_cast<int>(value))) {
                ++value;
            } else if (std::chrono::steady_clock::now() >= deadline) {
                timed_out.store(true, std::memory_order_relaxed);
                stop.store(true, std::memory_order_relaxed);
                return;
            } else {
                std::this_thread::yield();
            }
        }
    });

    std::thread consumer([&] {
        std::size_t expected = 0;
        while (expected < item_count) {
            if (stop.load(std::memory_order_relaxed)) break;

            const auto value = buffer.pop();
            if (!value) {
                if (std::chrono::steady_clock::now() >= deadline) {
                    timed_out.store(true, std::memory_order_relaxed);
                    stop.store(true, std::memory_order_relaxed);
                    break;
                }
                std::this_thread::yield();
                continue;
            }

            if (*value != static_cast<int>(expected)) ++order_errors;
            ++expected;
        }
        received = expected;
    });

    producer.join();
    consumer.join();

    CHECK(!timed_out.load(std::memory_order_relaxed));
    CHECK(received == item_count);
    CHECK(order_errors == 0);
}

} // namespace

int main() {
    test_new_buffer_is_empty();
    test_full_buffer_rejects_push();
    test_values_are_popped_in_fifo_order();
    test_buffer_wraps_around_and_reuses_freed_slots();
    test_one_producer_and_one_consumer_preserve_fifo_order();

    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "All SPSC ring buffer checks passed\n";
    return EXIT_SUCCESS;
}

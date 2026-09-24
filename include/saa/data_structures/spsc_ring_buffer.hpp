#pragma once

#include <bit>
#include <limits>
#include <atomic>
#include <memory>
#include <utility>
#include <cstddef>
#include <optional>
#include <stdexcept>

namespace saa {

template <typename T>
class SpscRingBuffer {
public:
    SpscRingBuffer(size_t desired) {
        if (desired == 0) {
            throw std::invalid_argument("Capacity must be positive");
        }

        constexpr auto largest = std::size_t(1) << (std::numeric_limits<std::size_t>::digits - 1);
        if (desired > largest) {
            throw std::length_error("Capacity is too large");
        }

        capacity = std::bit_ceil(desired);
        mask = capacity - 1;

        buffer = std::make_unique<T[]>(capacity);
    }

    bool push(T value) {
        const std::size_t write = head.load(std::memory_order_relaxed);
        const std::size_t read = tail.load(std::memory_order_acquire);

        if (write - read == capacity) return false;

        buffer[write & mask] = std::move(value);

        head.store(write + 1, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        const std::size_t read = tail.load(std::memory_order_relaxed);
        const std::size_t write = head.load(std::memory_order_acquire);

        if (read == write) return std::nullopt;

        T value = std::move(buffer[read & mask]);

        tail.store(read + 1, std::memory_order_release);
        return value;
    }

    const std::size_t getCapacity() {
        return capacity;
    }

private:
    std::atomic<std::size_t> head{0}, tail{0};
    std::unique_ptr<T[]> buffer;
    std::size_t capacity;
    std::size_t mask;
};

} // namespace saa

#pragma once

#include <atomic>
#include <memory>
#include <cstddef>
#include <optional>
#include <stdexcept>

namespace saa {

template <typename T>
class SpscRingBuffer {
public:
    SpscRingBuffer(size_t sz) {
        if (sz <= 0) {
            throw std::invalid_argument("Capacity must be positive");
        }

        buffer = std::make_unique<T[]>(sz);
        capacity = sz;
    }

    bool push(T value) {
        const std::size_t write = head.load(std::memory_order_relaxed);
        const std::size_t read = tail.load(std::memory_order_acquire);

        if (write - read == capacity) return false;

        buffer[head % capacity] = value;

        head.store(write + 1, std::memory_order_release);
        return true;
    }

    std::optional<T> pop() {
        const std::size_t read = tail.load(std::memory_order_relaxed);
        const std::size_t write = head.load(std::memory_order_acquire);

        if (read == write) return std::nullopt;

        T value = buffer[tail % capacity];

        tail.store(read + 1, std::memory_order_release);
        return value;
    }

private:
    std::atomic<std::size_t> head{0}, tail{0};
    std::unique_ptr<T[]> buffer;
    uint64_t capacity;
};

} // namespace saa

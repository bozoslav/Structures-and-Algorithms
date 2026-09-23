#pragma once

#include <vector>
#include <memory>
#include <utility>
#include <numeric>
#include <cstdint>
#include <cstddef>
#include <optional>

namespace saa {

template <typename T>
class SlotMap {
private:
    std::unique_ptr<std::optional<T>[]> slots;
    std::vector<std::size_t> availableIdx;
    std::unique_ptr<uint64_t[]> idAtIndex;
    size_t capacity;
    uint64_t tick;

public:
    struct Key {
        std::size_t index{};
        std::uint64_t id{};
    };

    explicit SlotMap(size_t n) {
        availableIdx.resize(n);
        std::iota(availableIdx.begin(), availableIdx.end(), 0);

        slots = std::make_unique<std::optional<T>[]>(n);
        idAtIndex = std::make_unique<uint64_t[]>(n);
        capacity = n;
        tick = 0;
    }

    std::optional<Key> insert(T x) {
        if (availableIdx.empty()) return std::nullopt;

        std::size_t idx = availableIdx.back();
        slots[idx].emplace(std::move(x));
        availableIdx.pop_back();

        uint64_t id = ++tick;
        idAtIndex[idx] = id;

        return Key{idx, id};
    }

    T* fetch(Key key) {
        const std::size_t i = key.index;

        if (i >= capacity || idAtIndex[i] != key.id || !slots[i].has_value()) {
            return nullptr;
        }

        return &*slots[i];
    }

    bool erase(Key key) {
        const std::size_t i = key.index;

        if (i >= capacity ||
            idAtIndex[i] != key.id ||
            !slots[i].has_value()) {
            return false;
        }

        availableIdx.push_back(i);
        slots[i].reset();
        idAtIndex[i] = 0;

        return true;
    }
};

} // namespace saa

//
// Created by Leon Mamic on 05.10.2026..
//

#pragma once
#include <cstddef>
#include <cstdint>
#include <memory>
#include <stdexcept>
#include <vector>

namespace saa {

template <typename T>
class SegTree {
private:
    std::unique_ptr<T[]> tree;
    std::size_t capacity;

public:
    explicit SegTree(std::int64_t n) {
        if (n < 0) {
            throw std::invalid_argument("Segment tree size must be nonnegative");
        }
        capacity = static_cast<std::size_t>(n);
        tree = std::make_unique<T[]>(2 * capacity);
    }

    SegTree(const std::vector<T>& v, std::int64_t n) : SegTree(n) {
        if (v.size() != capacity) {
            throw std::invalid_argument("Vector size must match segment tree size");
        }

        for (std::size_t i = 0; i < capacity; ++i) {
            tree[capacity + i] = v[i];
        }
        for (std::size_t i = capacity; i > 1;) {
            --i;
            tree[i] = op(tree[i << 1], tree[i << 1 | 1]);
        }
    }

    T op(T a, T b) const {
        return a + b;
    }

    void update(std::size_t idx, T val) {
        if (idx >= capacity) {
            throw std::out_of_range("Segment tree index out of range");
        }

        idx += capacity;

        tree[idx] = val;

        while (idx >>= 1) {
            tree[idx] = op(tree[idx << 1], tree[idx << 1 | 1]);
        }
    }

    T query(std::size_t l, std::size_t r) const { // inclusive
        if (l > r || r >= capacity) {
            throw std::out_of_range("Invalid segment tree query range");
        }

        l += capacity;
        r += capacity;

        T ret{};
        while (l <= r) {
            if (l & 1) {
                ret += tree[l++];
            }
            if (~r & 1) {
                ret += tree[r--];
            }
            l >>= 1;
            r >>= 1;
        }

        return ret;
    }
};

}

#include "saa/data_structures/segtree.hpp"

#include <cstdlib>
#include <iostream>
#include <numeric>
#include <stdexcept>
#include <vector>

namespace {
int failures = 0;

void check(bool condition, int line) {
    if (!condition) {
        std::cerr << "Line " << line << ": check failed\n";
        ++failures;
    }
}
#define CHECK(expression) check(static_cast<bool>(expression), __LINE__)

template <typename Exception, typename Function>
bool throws(Function function) {
    try { function(); }
    catch (const Exception&) { return true; }
    return false;
}

void check_ranges(saa::SegTree<int>& tree, const std::vector<int>& values) {
    for (std::size_t l = 0; l < values.size(); ++l) {
        int sum = 0;
        for (std::size_t r = l; r < values.size(); ++r) {
            sum += values[r];
            CHECK(tree.query(l, r) == sum);
        }
    }
}
}

int main() {
    saa::SegTree<int> example({1, 2, 3, 4}, 4);
    CHECK(example.query(1, 2) == 5);

    for (int n = 1; n <= 17; ++n) {
        std::vector<int> values(n);
        std::iota(values.begin(), values.end(), -3);
        saa::SegTree<int> tree(values, n);
        check_ranges(tree, values);
        for (int i = 0; i < n; ++i) {
            values[i] = 20 - i;
            tree.update(i, values[i]);
            check_ranges(tree, values);
        }

        saa::SegTree<int> zeroed(n);
        CHECK(zeroed.query(0, n - 1) == 0);
        zeroed.update(n - 1, 7);
        CHECK(zeroed.query(0, n - 1) == 7);
    }

    CHECK(throws<std::invalid_argument>([] { saa::SegTree<int> tree(-1); }));
    CHECK(throws<std::invalid_argument>([] { saa::SegTree<int> tree({1}, 2); }));
    CHECK(throws<std::out_of_range>([&] { example.update(4, 1); }));
    CHECK(throws<std::out_of_range>([&] { example.query(0, 4); }));
    CHECK(throws<std::out_of_range>([&] { example.query(2, 1); }));
    saa::SegTree<int> empty(0);
    CHECK(throws<std::out_of_range>([&] { empty.query(0, 0); }));
    CHECK(throws<std::out_of_range>([&] { empty.update(0, 1); }));

    if (failures) return EXIT_FAILURE;
    std::cout << "All segment tree checks passed\n";
    return EXIT_SUCCESS;
}

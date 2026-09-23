#include "saa/data_structures/slot_map.hpp"

#include <cstdlib>
#include <iostream>
#include <memory>
#include <utility>

namespace {

int failures = 0;

void check(bool condition, const char* expression, int line) {
    if (!condition) {
        std::cerr << "Line " << line << ": check failed: " << expression << '\n';
        ++failures;
    }
}

#define CHECK(expression) check(static_cast<bool>(expression), #expression, __LINE__)

struct NonDefaultMoveOnly {
    explicit NonDefaultMoveOnly(int value) : value(value) {}

    NonDefaultMoveOnly(const NonDefaultMoveOnly&) = delete;
    NonDefaultMoveOnly& operator=(const NonDefaultMoveOnly&) = delete;
    NonDefaultMoveOnly(NonDefaultMoveOnly&&) = default;
    NonDefaultMoveOnly& operator=(NonDefaultMoveOnly&&) = default;

    int value;
};

void test_insert_fetch_and_capacity() {
    saa::SlotMap<int> map(1);

    const auto key = map.insert(42);
    CHECK(key.has_value());
    CHECK(!map.insert(99).has_value());

    if (key) {
        int* value = map.fetch(*key);
        CHECK(value != nullptr);
        if (value != nullptr) {
            CHECK(*value == 42);
            *value = 43;
            int* updated_value = map.fetch(*key);
            CHECK(updated_value != nullptr);
            if (updated_value != nullptr) CHECK(*updated_value == 43);
        }
    }
}

void test_erase_reuse_and_stale_key_rejection() {
    saa::SlotMap<int> map(1);
    const auto old_key = map.insert(10);
    CHECK(old_key.has_value());
    if (!old_key) return;

    CHECK(map.erase(*old_key));
    CHECK(map.fetch(*old_key) == nullptr);
    CHECK(!map.erase(*old_key));

    const auto new_key = map.insert(20);
    CHECK(new_key.has_value());
    if (!new_key) return;

    CHECK(new_key->index == old_key->index);
    CHECK(new_key->id != old_key->id);
    CHECK(map.fetch(*old_key) == nullptr);

    int* value = map.fetch(*new_key);
    CHECK(value != nullptr);
    if (value != nullptr) CHECK(*value == 20);
}

void test_invalid_key_is_rejected() {
    saa::SlotMap<int> map(1);
    const saa::SlotMap<int>::Key invalid_key{1, 1};

    CHECK(map.fetch(invalid_key) == nullptr);
    CHECK(!map.erase(invalid_key));
}

void test_non_default_constructible_move_only_value() {
    saa::SlotMap<NonDefaultMoveOnly> map(1);
    const auto key = map.insert(NonDefaultMoveOnly{73});
    CHECK(key.has_value());
    if (!key) return;

    NonDefaultMoveOnly* value = map.fetch(*key);
    CHECK(value != nullptr);
    if (value != nullptr) CHECK(value->value == 73);
}

} // namespace

int main() {
    test_insert_fetch_and_capacity();
    test_erase_reuse_and_stale_key_rejection();
    test_invalid_key_is_rejected();
    test_non_default_constructible_move_only_value();

    if (failures != 0) {
        std::cerr << failures << " check(s) failed\n";
        return EXIT_FAILURE;
    }

    std::cout << "All slot map checks passed\n";
    return EXIT_SUCCESS;
}

#include "saa/data_structures/slot_map.hpp"

#include <iostream>

struct Order {
    int order_id;
    int quantity;
    double limit_price;
};

int main() {
    saa::SlotMap<Order> orders(1);

    const auto first_key = orders.insert(Order{1001, 25, 101.50});
    if (!first_key) {
        std::cerr << "Could not insert the first order\n";
        return 1;
    }

    if (Order* order = orders.fetch(*first_key)) {
        std::cout << "Order " << order->order_id << ": " << order->quantity
                  << " shares at " << order->limit_price << '\n';
    }

    if (!orders.erase(*first_key)) {
        std::cerr << "Could not erase the first order\n";
        return 1;
    }

    std::cout << "Old key is "
              << (orders.fetch(*first_key) == nullptr ? "stale" : "valid") << '\n';

    const auto replacement_key = orders.insert(Order{1002, 10, 99.75});
    if (!replacement_key) {
        std::cerr << "Could not reuse the freed slot\n";
        return 1;
    }

    if (Order* order = orders.fetch(*replacement_key)) {
        std::cout << "Reused slot for order " << order->order_id << '\n';
    }

    std::cout << "Old key is still "
              << (orders.fetch(*first_key) == nullptr ? "stale" : "valid") << '\n';
    return 0;
}

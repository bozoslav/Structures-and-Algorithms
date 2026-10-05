#include <vector>
#include <iostream>

#include "saa/data_structures/segtree.hpp"

int main() {
    std::vector<int> v = {1, 2, 3, 4};

    saa::SegTree<int> tree(v, 4);

    std::cout << tree.query(1, 2) << '\n';
}

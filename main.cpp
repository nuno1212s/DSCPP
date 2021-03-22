#include "datastructures.h"
#include "trees/avltree.h"
#include <iostream>
#include <memory>

int main() {

    std::cout << "Hello, World!" << std::endl;

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> key(new int(4)),
            key2(new int(3)), key3(new int(2)), key4(new int(1)),
            key5(new int(0)),
            value(new int(1));

    map->add(key, value);

    map->add(key2, value);

    map->add(key3, value);

    map->add(key4, value);

    map->add(key5, value);

//    map->remove(5);
//    map->remove(0);
//    map->remove(1);

    std::cout << "Smallest value: " << *std::get<0>(*map->peekSmallest()) << std::endl;
    std::cout << "Largest value: " << *std::get<0>(*map->peekLargest()) << std::endl;

    std::cout << "Contains: " << map->hasKey(5) << std::endl;
    std::cout << "Contains: " << map->hasKey(2) << std::endl;

    auto entries = map->entries();

    auto results = map->rangeSearch(1, 3);

    std::cout << "Results:" << std::endl;

    for (node_info<int, int> entry : *results) {

        std::cout << *std::get<0>(entry) << " " << *std::get<1>(entry) << std::endl;

    }

    std::cout << "All Entries: " << std::endl;

    for (node_info<int, int> entry : *entries) {

        std::cout << *std::get<0>(entry) << " " << *std::get<1>(entry) << std::endl;

    }

    return 0;
}

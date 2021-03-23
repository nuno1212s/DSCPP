#include "datastructures.h"
#include "trees/avltree.h"
#include <iostream>
#include <memory>

int main() {

    std::cout << "Hello, World!" << std::endl;

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value =  std::make_shared<int>(42);

    for (int i = 0; i < 7; i ++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);
    }

//     map->add(std::make_shared<int>(5), value);
//     map->add(std::make_shared<int>(2), value);
//     map->add(std::make_shared<int>(4), value);

    for (int i = 6; i >= 4; i--) {
        std::cout << "Removing " << i << std::endl;
        map->remove(i);
    }

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

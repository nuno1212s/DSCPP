#include "datastructures.h"
#include "trees/avltree.h"
#include <iostream>
#include <memory>

void hello() {

    std::unique_ptr<OrderedMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    map->add(std::make_shared<int>(1), std::make_shared<int>(2));
}

/*int main(int argc, char **argv) {

    hello();

    std::unique_ptr<OrderedMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    map->add(std::make_shared<int>(1), std::make_shared<int>(2));
}*/
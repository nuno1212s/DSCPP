#include "datastructures.h"
#include "trees/avltree.h"
#include <iostream>
#include <memory>

class Test {

    std::unique_ptr<Test> child;

public:
    Test() : child(nullptr) {

    }

    explicit Test(std::unique_ptr<Test> child) : child(std::move(child)) {}

    ~Test() {
        std::cout << "Destroyed" << std::endl;

        child.reset();
    }

    std::unique_ptr<Test> getTestOwner() {
        return std::move(child);
    }

    Test *getTest() {
        return child.get();
    }

};

int main() {

    std::cout << "Hello, World!" << std::endl;

    std::unique_ptr<Map<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> key(new int(2)), value(new int(1)),
            key2(new int(1)), key3(new int(3)), key4(new int(0)),
            key5(new int(5));

    map->add(key, value);

    map->add(key2, value);

    map->add(key3, value);

    map->add(key4, value);

    map->add(key5, value);

    map->remove(2);
    map->remove(0);
    map->remove(1);

    std::cout << "Contains: " << map->hasKey(5) << std::endl;
    std::cout << "Contains: " << map->hasKey(2) << std::endl;

    auto entries = map->entries();

    for (node_info<int, int> entry : *entries) {

        std::cout << *std::get<0>(entry) << " " << *std::get<1>(entry) << std::endl;

    }

    return 0;
}

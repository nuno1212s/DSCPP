#include "performancetests.h"
#include <iostream>
#include <bits/stdc++.h>
#include <chrono>
#include "../trees/avltree.h"
#include "../trees/redblacktree.h"
#include "../trees/splaytree.h"
#include "../trees/treaps.h"
#include "../probabilisticlist/skiplist.h"

T_Type readTestTypeFrom(std::string &string) {

    if (string == ("I")) {
        return INSERT;
    } else if (string == ("R")) {
        return REMOVE;
    } else if (string == "C") {
        return CONTAINS;
    } else if (string == "PL") {
        return POP_LARGEST;
    } else if (string == "PS") {
        return POP_SMALLEST;
    } else if (string == "S") {
        return SIZE;
    }

}

void executeTests(std::string file) {

    std::fstream stream;

    stream.open(file, std::fstream::in);

    auto testCases = parseTestCases(stream);

    std::unique_ptr<OrderedMap<int, int>> mapPointer = std::make_unique<AvlTree<int, int>>();

    auto start = std::chrono::high_resolution_clock::now();

    runTests(mapPointer.get(), *testCases);

    auto end = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Took " << duration.count() << " ms to run the tests on the AVL Tree" << std::endl;

    mapPointer = std::make_unique<RedBlackTree<int, int>>();

    start = std::chrono::high_resolution_clock::now();

    runTests(mapPointer.get(), *testCases);

    end = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Took " << duration.count() << " ms to run the tests on the Red Black Tree" << std::endl;

    mapPointer = std::make_unique<SplayTree<int, int>>();

    start = std::chrono::high_resolution_clock::now();

    runTests(mapPointer.get(), *testCases);

    end = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Took " << duration.count() << " ms to run the tests on the Splay Tree" << std::endl;

    mapPointer = std::make_unique<Treap<int, int>>();

    start = std::chrono::high_resolution_clock::now();

    runTests(mapPointer.get(), *testCases);

    end = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Took " << duration.count() << " ms to run the tests on the Treap" << std::endl;

    mapPointer = std::make_unique<SkipList<int, int>>();

    start = std::chrono::high_resolution_clock::now();

    runTests(mapPointer.get(), *testCases);

    end = std::chrono::high_resolution_clock::now();

    duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

    std::cout << "Took " << duration.count() << " ms to run the tests on the Splay Tree" << std::endl;
}

std::unique_ptr<std::vector<TCase>> parseTestCases(std::fstream &fp) {

    auto vector = std::make_unique<std::vector<TCase>>();

    std::string line;

    while (getline(fp, line)) {

        std::stringstream ss(line);

        std::string testType, value;

        ss >> testType;
        ss >> value;

        T_Type type = readTestTypeFrom(testType);

        vector->emplace_back(type, std::stoi(value));
    }

    return std::move(vector);
}

void runTests(OrderedMap<int, int> *map, std::vector<TCase> &tests) {

    auto sharedValue = std::make_shared<int>(1);

    for (const auto &item : tests) {

        switch (item.type) {

            case INSERT: {

                map->add(std::make_shared<int>(item.value), sharedValue);

                break;
            }

            case REMOVE: {
                map->remove(item.value);

                break;
            }

            case CONTAINS: {
                if (!map->hasKey(item.value)) {

                    //Error
                    std::cout << "Unexpected result. Required CONTAINS " << item.value << " but it does not" << std::endl;
                    return;
                }

                break;
            }

            case N_CONTAINS: {
                if (map->hasKey(item.value)) {

                    std::cout << "Unexpected result. Required N_CONTAINS " << item.value << " but it does" << std::endl;
                    return;
                }

                break;
            }

            case POP_LARGEST: {
                auto largest = map->popLargest();

                if (!largest) {
                    std::cout << "Failed to pop largest node." << std::endl;

                    return;
                } else {
                    if (*std::get<0>(*largest) != item.value) {
                        std::cout << "Node popped is not of the expected value" << std::endl;

                        return;
                    }
                }

                break;
            }

            case POP_SMALLEST: {
                auto largest = map->popSmallest();

                if (!largest) {
                    std::cout << "Failed to pop largest node." << std::endl;

                    return;
                } else {
                    if (*std::get<0>(*largest) != item.value) {
                        std::cout << "Node popped is not of the expected value" << std::endl;

                        return;
                    }
                }

                break;
            }
            case SIZE: {

                if (map->size() != item.value) {
                    std::cout << "The size of the map is not as expected. " << map->size() << " != " << item.value << std::endl;

                    return;
                }

                break;
            }

        }

    }

}
#include "../datastructures.h"
#include "../trees/avltree.h"
#include "../trees/redblacktree.h"
#include "../trees/splaytree.h"
#include "../trees/treaps.h"
#include "../probabilisticlist/skiplist.h"
#include <chrono>

#include "gtest/gtest.h"

#define BASE_TEST_SIZE 100
#define TEST_MULTIPLY 10
#define TEST_AMOUNTS 5
#define RANDOM_SEED 0xFA4812

void insertTest(OrderedMap<int, int> *map, int start, int end) {

    std::shared_ptr<int> value = std::make_shared<int>(1);

    for (int i = start; i != end; start > end ? i-- : i++) {
        map->add(std::move(std::make_shared<int>(i)), value);
    }
}

void deleteTest(OrderedMap<int, int> *map, int start, int end) {
    for (int i = start; i != end; start > end ? i-- : i++) {
        map->remove(i);
    }
}

void lookupTest(OrderedMap<int, int> *map, int start, int end) {

    for (int i = start; i != end; start > end ? i-- : i++) {
        map->hasKey(i);
    }

}

void insertHeavyAscTest(int testSize, OrderedMap<int, int> *map) {

    auto start = std::chrono::high_resolution_clock::now();

    insertTest(map, 0, testSize * 3);

    lookupTest(map, testSize, testSize * 2);

    deleteTest(map, testSize, testSize * 2);

    insertTest(map, testSize * 3, testSize * 6);

    lookupTest(map, testSize * 3, testSize * 4);

    deleteTest(map, testSize * 3, testSize * 4);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

void insertHeavyDescTest(int testSize, OrderedMap<int, int> *map) {

    auto start = std::chrono::high_resolution_clock::now();

    insertTest(map, testSize * 3, 0);

    lookupTest(map, testSize, testSize * 2);

    deleteTest(map, testSize * 2, testSize);

    insertTest(map, testSize * 6, testSize * 3 + 1);

    lookupTest(map, testSize * 4, testSize * 3);

    deleteTest(map, testSize * 4, testSize * 3);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

void lookupHeavyDescTest(int testSize, OrderedMap<int, int> *map) {

    auto start = std::chrono::high_resolution_clock::now();

    insertTest(map, testSize, 0);

    for (int i = 0; i < 5; i++)
        lookupTest(map, 0, testSize);

    insertTest(map, testSize * 2, testSize);

    for (int i = 0; i < 5; i++)
        lookupTest(map, testSize * 2, 0);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

void lookupHeavyAscTest(int testSize, OrderedMap<int, int> *map) {

    auto start = std::chrono::high_resolution_clock::now();

    insertTest(map, 0, testSize);

    for (int i = 0; i < 5; i++)
        lookupTest(map, 0, testSize);

    insertTest(map, testSize, testSize * 2);

    for (int i = 0; i < 5; i++)
        lookupTest(map, 0, testSize * 2);

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

void randomizedInsertHeavyTest(int testSize, OrderedMap<int, int> *map) {

    srand(RANDOM_SEED);

    auto value = std::make_shared<int>(1);

    auto values = std::make_unique<std::vector<int>>(testSize * 2);

    for (int i = 0; i < testSize * 2; i++) {

        int randNum = rand() % RAND_MAX;

        values->push_back(randNum);
    }

    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < testSize; i++) {
        map->add(std::make_shared<int>((*values)[i]), value);
    }

    for (const auto &num : *values) {

        if (num % 3 == 0) {
            map->hasKey(num);
        }

        if (num % 2 == 0) {
            map->add(std::make_shared<int>(num), value);
        } else {
            map->remove(num);
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

void randomizedLookupHeavyTest(int testSize, OrderedMap<int, int> *map) {

    srand(RANDOM_SEED);

    auto value = std::make_shared<int>(1);

    auto values = std::make_unique<std::vector<int>>(testSize * 2);

    for (int i = 0; i < testSize * 2; i++) {

        int randNum = rand() % RAND_MAX;

        values->push_back(randNum);
    }
    auto start = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < testSize; i++) {
        map->add(std::make_shared<int>((*values)[i]), value);
    }

    for (const auto &num : *values) {

        if (num % 2 == 0) {
            map->hasKey(num);
        } else {
            if (num % 3 == 0) {
                map->remove(num);
            } else {
                map->add(std::make_shared<int>(num), value);
            }
        }
    }

    auto end = std::chrono::high_resolution_clock::now();

    std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
              << " ms to complete." << std::endl;
}

TEST(PerfTest, SEQUENTIAL_ASC_INSERT_HEAVY) {

    int currentTestSize = BASE_TEST_SIZE;
    std::unique_ptr<OrderedMap<int, int>> ptrs;
    for (int i = 0; i <= TEST_AMOUNTS; i++) {

        std::cout << "Test size: " << currentTestSize << std::endl;

        ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        insertHeavyAscTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        insertHeavyAscTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        insertHeavyAscTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        insertHeavyAscTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        insertHeavyAscTest(currentTestSize, ptrs.get());

        ptrs.reset();

        currentTestSize *= TEST_MULTIPLY;
    }

}

TEST(PerfTest, SEQUENTIAL_ASC_LOOKUP_HEAVY) {

    int currentTestSize = BASE_TEST_SIZE;

    for (int i = 0; i <= TEST_AMOUNTS; i++) {
        std::cout << "Test size: " << currentTestSize << std::endl;

        std::unique_ptr<OrderedMap<int, int>> ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        lookupHeavyAscTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        lookupHeavyAscTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        lookupHeavyAscTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        lookupHeavyAscTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        lookupHeavyAscTest(currentTestSize, ptrs.get());

        currentTestSize *= TEST_MULTIPLY;
    }

}

TEST(PerfTest, SEQUENTIAL_DESC_INSERT_HEAVY) {

    int currentTestSize = BASE_TEST_SIZE;
    std::unique_ptr<OrderedMap<int, int>> ptrs;
    for (int i = 0; i <= TEST_AMOUNTS; i++) {

        std::cout << "Test size: " << currentTestSize << std::endl;

        ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        insertHeavyDescTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        insertHeavyDescTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        insertHeavyDescTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        insertHeavyDescTest(currentTestSize, ptrs.get());

        ptrs.reset();

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        insertHeavyDescTest(currentTestSize, ptrs.get());

        ptrs.reset();

        currentTestSize *= TEST_MULTIPLY;
    }
}

TEST(PerfTest, SEQUENTIAL_DESC_LOOKUP_HEAVY) {
    int currentTestSize = BASE_TEST_SIZE;

    for (int i = 0; i <= TEST_AMOUNTS; i++) {
        std::cout << "Test size: " << currentTestSize << std::endl;

        std::unique_ptr<OrderedMap<int, int>> ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        lookupHeavyDescTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        lookupHeavyDescTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        lookupHeavyDescTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        lookupHeavyDescTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        lookupHeavyDescTest(currentTestSize, ptrs.get());

        currentTestSize *= TEST_MULTIPLY;
    }
}

TEST(PerfTest, RANDOM_INSERT_HEAVY) {

    int currentTestSize = BASE_TEST_SIZE;

    for (int i = 0; i <= TEST_AMOUNTS; i++) {
        std::cout << "Test size: " << currentTestSize << std::endl;

        std::unique_ptr<OrderedMap<int, int>> ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        randomizedInsertHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        randomizedInsertHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        randomizedInsertHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        randomizedInsertHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        randomizedInsertHeavyTest(currentTestSize, ptrs.get());

        currentTestSize *= TEST_MULTIPLY;
    }
}

TEST(PerfTest, RANDOM_LOOKUP_HEAVY) {

    int currentTestSize = BASE_TEST_SIZE;

    for (int i = 0; i <= TEST_AMOUNTS; i++) {
        std::cout << "Test size: " << currentTestSize << std::endl;

        std::unique_ptr<OrderedMap<int, int>> ptrs = std::make_unique<AvlTree<int, int>>();

        std::cout << "Testing the DS: AVL Tree" << std::endl;

        randomizedLookupHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<RedBlackTree<int, int>>();

        std::cout << "Testing the DS: Red Black" << std::endl;

        randomizedLookupHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SplayTree<int, int>>();

        std::cout << "Testing the DS: Splay Tree" << std::endl;

        randomizedLookupHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<Treap<int, int>>();

        std::cout << "Testing the DS: Treap" << std::endl;

        randomizedLookupHeavyTest(currentTestSize, ptrs.get());

        ptrs = std::make_unique<SkipList<int, int>>();

        std::cout << "Testing the DS: Skip List" << std::endl;

        randomizedLookupHeavyTest(currentTestSize, ptrs.get());

        currentTestSize *= TEST_MULTIPLY;
    }
}
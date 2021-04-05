#include "../datastructures.h"

#include "../trees/avltree.h"
#include "../trees/redblacktree.h"
#include "../trees/splaytree.h"
#include "../trees/treaps.h"
#include "gtest/gtest.h"
#include "mutex"
#include <chrono>

//#define TEST_SIZE 1048576
#define TEST_SIZE 8

void insert(Map<int, int> *map) {

    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }

}

void insertBackwards(Map<int, int> *map) {

    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = TEST_SIZE - 1; i >= 0; i--) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }
}

void insertAndContains(Map<int, int> *map) {
    std::shared_ptr<int> value = std::make_shared<int>(42);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key = std::make_shared<int>(i);

        std::cout << "inserting " << i << std::endl;
        map->add(std::move(key), value);
    }
    auto endAdd = std::chrono::high_resolution_clock::now();

    auto startContains = endAdd;

    ASSERT_TRUE(map->hasKey(TEST_SIZE / 2));

    auto endContains = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endContains - startContains);

    auto durationAdd = std::chrono::duration_cast<std::chrono::microseconds>(endAdd - start);
    std::cout << "Time taken for adds: " << durationAdd.count() << "/" << TEST_SIZE << " micros per operation. Has: "
              << duration.count() << "/" << TEST_SIZE << " micros per operation." << std::endl;
}

void insertAndRemove(Map<int, int> *map) {

    insert(map);

    for (int i = 0; i < TEST_SIZE; i++) {
        ASSERT_TRUE(map->hasKey(i));
    }

    for (int i = 0; i < TEST_SIZE; i++) {

        ASSERT_TRUE(map->hasKey(i));

        map->remove(i);

        ASSERT_FALSE(map->hasKey(i));
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        ASSERT_FALSE(map->hasKey(i));
    }

    ASSERT_EQ(map->size(), 0);
}

void insertAndRemoveBackwards(Map<int, int> *map) {

    insert(map);

    std::cout << "Done inserting." << std::endl;

    for (int i = 0; i < TEST_SIZE; i++) {
        ASSERT_TRUE(map->hasKey(i));
    }

    for (int i = TEST_SIZE - 1; i >= 0; i--) {
        ASSERT_TRUE(map->hasKey(i));

        map->remove(i);

        ASSERT_FALSE(map->hasKey(i));
    }

    for (int i = 0; i < TEST_SIZE; i++) {
        ASSERT_FALSE(map->hasKey(i));
    }

    ASSERT_EQ(map->size(), 0);
}

void insertAndPop(TreeMap<int, int> *map) {

    insert(map);

    std::cout << "Done inserting." << std::endl;

    for (int i = 0; i < TEST_SIZE; i++) {
        auto result = map->popSmallest();

        ASSERT_TRUE(result);

        ASSERT_EQ(i, *std::get<0>(*result));
    }

    ASSERT_EQ(map->size(), 0);
}


void insertAndPopBackwards(TreeMap<int, int> *map) {

    insert(map);

    std::cout << "Done inserting." << std::endl;

    for (int i = TEST_SIZE - 1; i >= 0; i--) {
        auto result = map->popLargest();

        ASSERT_TRUE(result);

        int res = *std::get<0>(*result);
        ASSERT_EQ(i, res);
    }

    ASSERT_EQ(map->size(), 0);
}

TEST(TreeTest, InsertAndContains) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndContains(map.get());

}

TEST(TreeTest, InsertAndRemove) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndRemove(map.get());

    map.reset();

    map = std::make_unique<RedBlackTree<int, int>>();

    insertAndRemove(map.get());

    map.reset();

    map = std::make_unique<SplayTree<int, int>>();

    insertAndRemove(map.get());
}

TEST(TreeTest, InsertAndRemoveBackwards) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndRemoveBackwards(map.get());

    map.reset();

    map = std::make_unique<RedBlackTree<int, int>>();

    insertAndRemoveBackwards(map.get());

    map.reset();

    map = std::make_unique<SplayTree<int, int>>();

    insertAndRemoveBackwards(map.get());

    map.reset();

    map = std::make_unique<Treap<int, int>>();

    insertAndRemoveBackwards(map.get());
}

TEST(TreeTest, InsertAndPopInOrder) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndPop(map.get());

    map.reset();

    std::cout << "testing red black tree." << std::endl;

    map = std::make_unique<RedBlackTree<int, int>>();

    insertAndPop(map.get());

    std::cout << "testing splay tree." << std::endl;

    map = std::make_unique<SplayTree<int, int>>();

    insertAndPop(map.get());
}

TEST(TreeTest, InsertAndPopBackwards) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndPopBackwards(map.get());

    map.reset();

    std::cout << "testing red black tree." << std::endl;

    map = std::make_unique<RedBlackTree<int, int>>();

    insertAndPopBackwards(map.get());

    map.reset();

    std::cout << "testing splay tree." << std::endl;

    map = std::make_unique<SplayTree<int, int>>();

    insertAndPopBackwards(map.get());
}

class DestructionTest {

private:
    int i;

public:

    explicit DestructionTest(int i) : i(i) {}

    ~DestructionTest() {
        std::cout << "LOL " << i << std::endl;
    }

};

TEST(test, random) {

    std::vector<DestructionTest> vector;

    vector.reserve(10);

    for (int i = 0; i < 10; i++) {
        vector.emplace_back(i);
    }

}


int main(int argc, char **argv) {

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

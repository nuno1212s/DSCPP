#include "../datastructures.h"

#include "../trees/avltree.h"
#include "../trees/redblacktree.h"
#include "../trees/splaytree.h"
#include "gtest/gtest.h"
#include <chrono>

//#define TEST_SIZE 1048576
#define TEST_SIZE 8

void insertAndContains(TreeMap<int, int> *map) {
    std::shared_ptr<int> value = std::make_shared<int>(42);

    auto start = std::chrono::high_resolution_clock::now();
    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key = std::make_shared<int>(i);

        std::cout << "inserting " << i << std::endl;
        map->add(std::move(key), value);
    }
    auto endAdd = std::chrono::high_resolution_clock::now();

//    std::cout << "Height: " << map->getTreeHeight() << std::endl;

    auto startContains = std::chrono::high_resolution_clock::now();

    ASSERT_TRUE(map->hasKey(TEST_SIZE / 2));

    auto endContains = std::chrono::high_resolution_clock::now();

    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endContains - startContains);

    auto durationAdd = std::chrono::duration_cast<std::chrono::microseconds>(endAdd - start);
    std::cout << "Time taken for adds: " << durationAdd.count() << "/" << TEST_SIZE << " micros per operation. Has: " << duration.count()
              << "/" << TEST_SIZE << " micros per operation." << std::endl;
}

void insertAndRemove(TreeMap<int, int> *map) {

    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }

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

void insertAndRemoveBackwards(TreeMap<int, int> *map) {
    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }

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

    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }

    std::cout << "Done inserting." << std::endl;

    for (int i = 0; i < TEST_SIZE; i++) {
        auto result = map->popSmallest();

        ASSERT_TRUE(result);

        ASSERT_EQ(i, *std::get<0>(*result));
    }

    ASSERT_EQ(map->size(), 0);
}


void insertAndPopBackwards(TreeMap<int, int> *map) {

    std::shared_ptr<int> value = std::make_shared<int>(42);

    for (int i = 0; i < TEST_SIZE; i++) {
        std::shared_ptr<int> key(new int(i));

        map->add(std::move(key), value);

        ASSERT_TRUE(map->hasKey(i));
    }

    std::cout << "Done inserting." << std::endl;

    for (int i = TEST_SIZE - 1; i >= 0; i--) {
        auto result = map->popLargest();

        ASSERT_TRUE(result);

        int res = *std::get<0>(*result);
        ASSERT_EQ(i, res);
    }

    ASSERT_EQ(map->size(), 0);
}

TEST(AVLTest, InsertAndContains) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndContains(map.get());

}

TEST(AVLTest, InsertAndRemove) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

    insertAndRemove(map.get());
}

TEST(AVLTest, InsertAndRemoveBackwards) {

    std::unique_ptr<TreeMap<int, int>> map = std::make_unique<AvlTree<int, int>>();

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

TEST(RBTree, TestRemoveSimple) {

    auto tree = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    tree->add(std::make_shared<int>(30), value);
    tree->add(std::make_shared<int>(20), value);
    tree->add(std::make_shared<int>(40), value);
    tree->add(std::make_shared<int>(10), value);

    tree->remove(20);

    ASSERT_FALSE(tree->hasKey(20));

}

TEST(RBTree, TestRemoveDoubleBlack) {

    auto tree = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    tree->add(std::make_shared<int>(30), value);
    tree->add(std::make_shared<int>(20), value);
    tree->add(std::make_shared<int>(40), value);
    tree->add(std::make_shared<int>(50), value);

    tree->remove(20);

    ASSERT_FALSE(tree->hasKey(20));

}

TEST(RBTree, TestRemoveDoubleBlack2) {

    auto tree = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    tree->add(std::make_shared<int>(30), value);
    tree->add(std::make_shared<int>(20), value);
    tree->add(std::make_shared<int>(40), value);
    tree->add(std::make_shared<int>(35), value);
    tree->add(std::make_shared<int>(50), value);

    tree->remove(20);

    ASSERT_FALSE(tree->hasKey(20));
}

int main(int argc, char **argv) {

    std::cout << "Hello, World!" << std::endl;

    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

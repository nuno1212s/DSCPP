#include "gtest/gtest.h"
#include "../trees/avltree.h"

/**
 * Tests all the possible rotations,
 * If these tests all pass, then because each child makes up the root of a new AVL tree,
 * We can assume that these properties will be maintained for a tree of any height
 */

TEST(AVLTest, TestLRotation) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(6), value);
    map->add(std::make_shared<int>(7), value);
    map->add(std::make_shared<int>(8), value);

    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 5);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 7);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 4);
    ASSERT_EQ(*(root->getRightChild()->getRightChild()->getKeyVal()), 8);
    ASSERT_EQ(*(root->getRightChild()->getLeftChild()->getKeyVal()), 6);
}

TEST(AVLTest, TestRRotation) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(6), value);
    map->add(std::make_shared<int>(3), value);
    map->add(std::make_shared<int>(2), value);

    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 5);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 6);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 3);
    ASSERT_EQ(*(root->getLeftChild()->getLeftChild()->getKeyVal()), 2);
    ASSERT_EQ(*(root->getLeftChild()->getRightChild()->getKeyVal()), 4);
}

TEST(AVLTest, TestLRRotation) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(7), value);
    map->add(std::make_shared<int>(6), value);

    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 6);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 7);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 5);
}

TEST(AVLTest, TestRLRotation) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(3), value);
    map->add(std::make_shared<int>(4), value);

    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 4);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 5);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 3);
}

TEST(AVLTest, TestDoubleRotate) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(20), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(3), value);
    map->add(std::make_shared<int>(9), value);
    map->add(std::make_shared<int>(26), value);
    map->add(std::make_shared<int>(15), value);

    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 9);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 20);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 4);
    ASSERT_EQ(*(root->getRightChild()->getRightChild()->getKeyVal()), 26);
    ASSERT_EQ(*(root->getRightChild()->getLeftChild()->getKeyVal()), 15);

    ASSERT_EQ(*(root->getLeftChild()->getLeftChild()->getKeyVal()), 3);
}

TEST(AVLTest, TestRemove) {

    std::unique_ptr<AvlTree<int, int>> map = std::make_unique<AvlTree<int, int>>();

    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(2), value);
    map->add(std::make_shared<int>(1), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(3), value);
    map->add(std::make_shared<int>(5), value);


    map->remove(1);


    auto *root = map->getRoot();

    ASSERT_EQ(*root->getKeyVal(), 4);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 5);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 2);
    ASSERT_EQ(*(root->getLeftChild()->getRightChild()->getKeyVal()), 3);
}
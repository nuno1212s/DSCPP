
#include "../trees/redblacktree.h"
#include "gtest/gtest.h"

TEST(RBTests, TestRecolour) {

    std::unique_ptr<RedBlackTree<int, int>> map = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(6), value);
    map->add(std::make_shared<int>(7), value);

    auto *root = (RBNode<int, int> *) map->getRoot();

    ASSERT_EQ(root->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild()->getRightChild())->getColor(), RED);
}

TEST(RBTests, TestRLRotation) {

    std::unique_ptr<RedBlackTree<int, int>> map = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(6), value);
    map->add(std::make_shared<int>(10), value);
    map->add(std::make_shared<int>(8), value);

    auto *root = (RBNode<int, int> *) map->getRoot();

    ASSERT_EQ(root->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild()->getRightChild())->getColor(), RED);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild()->getLeftChild())->getColor(), RED);


    ASSERT_EQ(*root->getKeyVal(), 5);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 8);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 4);
    ASSERT_EQ(*(root->getRightChild()->getRightChild()->getKeyVal()), 10);
    ASSERT_EQ(*(root->getRightChild()->getLeftChild()->getKeyVal()), 6);
}

TEST(RBTests, TestLRRotation) {

    std::unique_ptr<RedBlackTree<int, int>> map = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(5), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(6), value);
    map->add(std::make_shared<int>(1), value);
    map->add(std::make_shared<int>(3), value);

    auto *root = (RBNode<int, int> *) map->getRoot();

    ASSERT_EQ(root->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild()->getRightChild())->getColor(), RED);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild()->getLeftChild())->getColor(), RED);


    ASSERT_EQ(*root->getKeyVal(), 5);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 6);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 3);
    ASSERT_EQ(*(root->getLeftChild()->getRightChild()->getKeyVal()), 4);
    ASSERT_EQ(*(root->getLeftChild()->getLeftChild()->getKeyVal()), 1);
}

TEST(RBTests, TestRemoveSimple) {

    auto tree = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    tree->add(std::make_shared<int>(30), value);
    tree->add(std::make_shared<int>(20), value);
    tree->add(std::make_shared<int>(40), value);
    tree->add(std::make_shared<int>(10), value);

    tree->remove(20);

    ASSERT_FALSE(tree->hasKey(20));

    auto *root = (RBNode<int, int> *) tree->getRoot();

    ASSERT_EQ(root->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild())->getColor(), BLACK);

    ASSERT_EQ(*root->getKeyVal(), 30);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 40);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 10);
}

TEST(RBTests, TestRemoveDoubleBlack) {

    auto tree = std::make_unique<RedBlackTree<int, int>>();

    auto value = std::make_shared<int>(42);

    tree->add(std::make_shared<int>(30), value);
    tree->add(std::make_shared<int>(20), value);
    tree->add(std::make_shared<int>(40), value);
    tree->add(std::make_shared<int>(50), value);

    tree->remove(20);

    ASSERT_FALSE(tree->hasKey(20));

    auto *root = (RBNode<int, int> *) tree->getRoot();

    ASSERT_EQ(root->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getLeftChild())->getColor(), BLACK);
    ASSERT_EQ(((RBNode<int, int> *) root->getRightChild())->getColor(), BLACK);

    ASSERT_EQ(*root->getKeyVal(), 40);
    ASSERT_EQ(*(root->getRightChild()->getKeyVal()), 50);
    ASSERT_EQ(*(root->getLeftChild()->getKeyVal()), 30);
}
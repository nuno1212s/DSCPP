#include "gtest/gtest.h"
#include "../trees/treaps.h"

bool verifyHeapProperty(TreapNode<int, int> *root) {

    if (root == nullptr) return true;

    auto *leftchild = (TreapNode<int, int> *) root->getLeftChild();
    auto *rightChild = (TreapNode<int, int> *) root->getRightChild();

    bool left = false, right = false;


    if (leftchild != nullptr) {
        if (leftchild->getHeapWeight() > root->getHeapWeight()) {
            return false;
        }

        if (*leftchild->getKeyVal() >= *root->getKeyVal()) {
            std::cout << "Not a BST, left" << std::endl;
            return false;
        }

        if (!verifyHeapProperty(leftchild)) {
            return false;
        }
    }

    if (rightChild != nullptr) {
        if (rightChild->getHeapWeight() > root->getHeapWeight()) {
            return false;
        }

        if (*rightChild->getKeyVal() <= *root->getKeyVal()) {
            std::cout << "Not a BST, right" << std::endl;
            return false;
        }

        if (!verifyHeapProperty(rightChild)) {
            return false;
        }
    }

    return true;

}

TEST(TreapTests, VerifyHeapProperty) {

    std::unique_ptr<Treap<int, int>> map = std::make_unique<Treap<int, int>>();


    std::shared_ptr<int> value = std::make_shared<int>(42);

    map->add(std::make_shared<int>(20), value);
    map->add(std::make_shared<int>(4), value);
    map->add(std::make_shared<int>(3), value);
    map->add(std::make_shared<int>(9), value);
    map->add(std::make_shared<int>(26), value);
    map->add(std::make_shared<int>(15), value);

    map->remove(15);

    map->remove(4);

    map->remove(26);

    auto *root = (TreapNode<int, int> *) map->getRoot();

    ASSERT_FALSE(map->hasKey(15));
    ASSERT_FALSE(map->hasKey(4));
    ASSERT_TRUE(map->hasKey(20));

    ASSERT_TRUE(verifyHeapProperty(root));


}

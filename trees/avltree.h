#ifndef TRABALHO1_AVLTREE_H
#define TRABALHO1_AVLTREE_H

#include <vector>
#include "trees.h"

template<typename T, typename V>
class AVLNode : public TreeNode<T, V> {
private:
    int balance;

public:
    AVLNode(std::shared_ptr<T> key, std::shared_ptr<V> value, AVLNode<T, V> *parent) : TreeNode<T, V>(std::move(key), std::move(value), parent), balance(0) {}

    int getBalance() {
        return balance;
    }

    void setBalance(int balance) {
        this->balance = balance;
    }
};

template<typename T, typename V>
class AvlTree : public BinarySearchTree<T, V> {

private:
    void updateBalance(TreeNode<T, V> *leaf) {

    }

public:
    AvlTree() : BinarySearchTree<T, V>() {}

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        if (this->getRoot() == nullptr) {

            std::unique_ptr<AVLNode<T, V>> rootNode = std::make_unique<AVLNode<T, V>>(std::move(key), std::move(value), nullptr);

            std::cout << "new root node " << std::endl;

            this->setRootNode(std::move(rootNode));

            this->treeSize++;

            return;
        }

        AVLNode<T, V> *rootNode = (AVLNode<T, V> *) this->getRoot(),
                *parent = nullptr;

        while (rootNode != nullptr) {

            parent = rootNode;

            auto keyValue = *rootNode->getKey().get();

            if (keyValue == *key) {

                rootNode->setValue(std::move(value));

            } else if (*key < keyValue) {
                rootNode = (AVLNode<T, V> *) rootNode->getLeftChild();
            } else {
                rootNode = (AVLNode<T, V> *) rootNode->getRightChild();
            }
        }

        if (parent == nullptr) {
            //what?
            std::cout << "parent is null where it shouldn't be" << std::endl;

            return;
        }

        auto keyValue = *parent->getKey().get();

        T *keyRef = key.get();

        std::unique_ptr<AVLNode<T, V>> newNode = std::make_unique<AVLNode<T, V>>(std::move(key), std::move(value), parent);

        if (*keyRef < keyValue) {
            parent->setLeftChild(std::move(newNode));

            std::cout << "Left " << std::endl;
        } else {
            parent->setRightChild(std::move(newNode));

            std::cout << "Right " << std::endl;
        }

        this->treeSize++;

    }

    std::optional<node_info<T, V>> popSmallest() override {
        return std::nullopt;
    }

    std::optional<node_info<T, V>> popLargest() override {
        return std::nullopt;
    }

};


#endif //TRABALHO1_AVLTREE_H

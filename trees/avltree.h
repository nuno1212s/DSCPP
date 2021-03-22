#ifndef TRABALHO1_AVLTREE_H
#define TRABALHO1_AVLTREE_H

#include <vector>
#include "binarytrees.h"

template<typename T, typename V>
class AVLNode : public TreeNode<T, V> {
private:
    int balance;

public:
    AVLNode(std::shared_ptr<T> key, std::shared_ptr<V> value, AVLNode<T, V> *parent) : TreeNode<T, V>(std::move(key),
                                                                                                      std::move(value),
                                                                                                      parent),
                                                                                       balance(0) {}

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

    std::unique_ptr<TreeNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value,
                                                   TreeNode<T, V> *parent) override {
        return std::make_unique<TreeNode<T, V>>(key, value, parent);
    }

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {
        TreeNode<T, V> *nodes = this->addNode(key, value);

        if (nodes->getParent() != nullptr) {
            TreeNode<T, V> *parent = nodes->getParent();


        }
    }

    std::optional<node_info<T, V>> popSmallest() override {
        return std::nullopt;
    }

    std::optional<node_info<T, V>> popLargest() override {
        return std::nullopt;
    }

};


#endif //TRABALHO1_AVLTREE_H

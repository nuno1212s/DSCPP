#ifndef TRABALHO1_AVLTREE_H
#define TRABALHO1_AVLTREE_H

#include <vector>
#include "binarytrees.h"

template<typename T, typename V>
class AVLNode : public TreeNode<T, V> {
private:
    int balance;

public:
    AVLNode(std::shared_ptr<T> key, std::shared_ptr<V> value, AVLNode<T, V> *parent)
            : TreeNode<T, V>(std::move(key),
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
    std::unique_ptr<AVLNode<T, V>> rotateLeft(std::unique_ptr<AVLNode<T, V>> root) {

        std::unique_ptr<AVLNode<T, V>> right(static_cast<AVLNode<T, V>*>(root->getRightNodeOwnership().release()));

        AVLNode<T, V> *rootRef = root.get(), *rightRef = right.get();

        std::cout << "Rotating left around root: " << *(root->getKey()) << std::endl;

        if (rightRef->getLeftChild() != nullptr) {
            //Move the left child of the root's right child into the right child of the root
            root->setRightChild(std::move(rightRef->getLeftNodeOwnership()));
        }

        //The set child methods automatically set the parent to the node that they have been moved to
        rightRef->setLeftChild(std::move(root));

        rootRef->setBalance(rootRef->getBalance() - 1 - std::max(0, rightRef->getBalance()));
        rightRef->setBalance(rightRef->getBalance() - 1 + std::min(0, rootRef->getBalance()));

        //The new root is now the right node
        return right;
    }

    std::unique_ptr<AVLNode<T, V>> rotateRight(std::unique_ptr<AVLNode<T, V>> root) {

        std::unique_ptr<AVLNode<T, V>> left(static_cast<AVLNode<T, V>*>(root->getLeftNodeOwnership().release()));

        AVLNode<T, V> *rootRef = root.get(), *leftRef = left.get();

        std::cout << "Rotating right around root: " << *(root->getKey()) << std::endl;

        if (leftRef->getRightChild() != nullptr) {
            rootRef->setLeftChild(std::move(leftRef->getRightNodeOwnership()));
        }

        leftRef->setRightChild(std::move(root));

        rootRef->setBalance(rootRef->getBalance() + 1 - std::min(0, leftRef->getBalance()));
        leftRef->setBalance(leftRef->getBalance() + 1 - std::max(0, rootRef->getBalance()));

        return left;
    }

    void rotateRightP(AVLNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<AVLNode<T, V>> rootOwner(
                    static_cast<AVLNode<T, V>*>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateRight(std::move(rootOwner)));

        } else {
            if (parent->getLeftChild() == root) {
                std::unique_ptr<AVLNode<T, V>> leftOwner(
                        static_cast<AVLNode<T, V>*>(parent->getLeftNodeOwnership().release()));

                parent->setLeftChild(rotateRight(std::move(leftOwner)));
            } else {
                std::unique_ptr<AVLNode<T, V>> rightOwner(
                        static_cast<AVLNode<T, V>*>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateRight(std::move(rightOwner)));
            }
        }
    }

    void rotateLeftP(AVLNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<AVLNode<T, V>> rootOwner(
                    static_cast<AVLNode<T, V>*>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateLeft(std::move(rootOwner)));
        } else {

            if (parent->getLeftChild() == root) {
                std::unique_ptr<AVLNode<T, V>> leftOwner(
                        static_cast<AVLNode<T, V>*>(parent->getLeftNodeOwnership().release()));
                parent->setLeftChild(rotateLeft(std::move(leftOwner)));
            } else {
                std::unique_ptr<AVLNode<T, V>> rightOwner(
                        static_cast<AVLNode<T, V>*>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateLeft(std::move(rightOwner)));
            }
        }
    }

    void rebalance(AVLNode<T, V> *root) {
        if (root->getBalance() > 0) {

            auto rightChild = (AVLNode<T, V> *) root->getRightChild();

            if (rightChild->getBalance() < 0) {

                std::unique_ptr<AVLNode<T, V>> rightChildOwner(
                        static_cast<AVLNode<T, V>*>(root->getRightNodeOwnership().release()));

                auto result = rotateRight(std::move(rightChildOwner));

                root->setRightChild(std::move(result));

                rotateLeftP(root);
            } else {
                rotateLeftP(root);
            }
        } else if (root->getBalance() < 0) {

            auto leftChild = (AVLNode<T,V> *) root->getLeftChild();

            if (leftChild->getBalance() > 0) {
                std::unique_ptr<AVLNode<T, V>> leftChildOwner(
                        static_cast<AVLNode<T, V>*>(root->getLeftNodeOwnership().release()));

                auto result = rotateLeft(std::move(leftChildOwner));

                root->setLeftChild(std::move(result));

                rotateRightP(root);
            } else {
                rotateRightP(root);
            }
        }
    }

    void updateBalance(AVLNode<T, V> *leaf, int increase) {
        if (leaf->getBalance() > 1 || leaf->getBalance() < -1) {
            rebalance(leaf);
            return;
        }

        if (leaf->getParent() != nullptr) {

            auto parent = (AVLNode<T, V>*) leaf->getParent();

            if (parent->getLeftChild() == leaf) {
                parent->setBalance(parent->getBalance() - increase);
            } else {
                parent->setBalance(parent->getBalance() + increase);
            }

            if (parent->getBalance() != 0) {
                updateBalance(parent, increase);
            }
        }
    }

public:
    AvlTree() : BinarySearchTree<T, V>() {}

    std::unique_ptr<TreeNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value,
                                                   TreeNode<T, V> *parent) override {
        return std::unique_ptr<TreeNode<T, V>>(
                (TreeNode<T, V> *) new AVLNode<T, V>(key, value, (AVLNode<T, V> *) parent));
    }

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {
        TreeNode<T, V> *nodes = this->addNode(key, value);

        updateBalance((AVLNode<T, V>*) nodes, +1);
    }

    std::optional<std::shared_ptr<V>> remove(const T&key) override {
        auto removedNodeInfo = this->removeNode(key);

        if (removedNodeInfo) {
            std::unique_ptr<AVLNode<T, V>> removedNode(
                    static_cast<AVLNode<T, V> *> (std::get<1>(*removedNodeInfo).release()));

            updateBalance(removedNode.get(), -1);

            return std::get<1>(std::get<0>(*removedNodeInfo));
        }

        return std::nullopt;
    }

    std::optional<node_info<T, V>> popSmallest() override {
        return std::nullopt;
    }

    std::optional<node_info<T, V>> popLargest() override {
        return std::nullopt;
    }

};


#endif //TRABALHO1_AVLTREE_H

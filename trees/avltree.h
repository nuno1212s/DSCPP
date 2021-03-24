#ifndef TRABALHO1_AVLTREE_H
#define TRABALHO1_AVLTREE_H

#include <vector>
#include "binarytrees.h"

template<typename T, typename V>
class AVLNode : public TreeNode<T, V> {
private:
    int height;

public:
    AVLNode(std::shared_ptr<T> key, std::shared_ptr<V> value, AVLNode<T, V> *parent)
            : TreeNode<T, V>(std::move(key),
                             std::move(value),
                             parent),
              height(0) {}

    int getHeight() {
        return height;
    }

    void setHeight(int balance) {
        this->height = balance;
    }
};

template<typename T, typename V>
static int getHeight(TreeNode<T, V> *node) {
    if (node == nullptr) return 0;

    return ((AVLNode<T, V> *) node)->getHeight();
}

template<typename T, typename V>
static int getBalance(TreeNode<T, V> *node) {
    return getHeight(node->getLeftChild()) - getHeight(node->getRightChild());
}

template<typename T, typename V>
class AvlTree : public BinarySearchTree<T, V> {

private:

    std::unique_ptr<TreeNode<T, V>> rotateLeft(std::unique_ptr<TreeNode<T, V>> root) override {

        auto *rootRef = (AVLNode<T, V>*) root.get();

        auto newRoot = BinarySearchTree<T, V>::rotateLeft(std::move(root));

        auto* rightRef = (AVLNode<T, V>*) newRoot.get();

        rootRef->setHeight(1 + std::max(getHeight(rootRef->getLeftChild()), getHeight(rootRef->getRightChild())));
        rightRef->setHeight(1 + std::max(getHeight(rightRef->getLeftChild()), getHeight(rightRef->getRightChild())));

        //The new root is now the right node
        return std::move(newRoot);
    }

    std::unique_ptr<TreeNode<T, V>> rotateRight(std::unique_ptr<TreeNode<T, V>> root) override {

        auto *rootRef = (AVLNode<T, V> *) root.get();

        auto newRoot = BinarySearchTree<T, V>::rotateRight(std::move(root));

        auto *leftRef = (AVLNode<T, V> *) newRoot.get();

        rootRef->setHeight(1 + std::max(getHeight(rootRef->getLeftChild()), getHeight(rootRef->getRightChild())));
        leftRef->setHeight(1 + std::max(getHeight(leftRef->getLeftChild()), getHeight(leftRef->getRightChild())));

        //The new root is now the left node
        return std::move(newRoot);
    }

    void rotateRightP(AVLNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<AVLNode<T, V>> rootOwner(
                    static_cast<AVLNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateRight(std::move(rootOwner)));

        } else {
            if (parent->getLeftChild() == root) {
                std::unique_ptr<AVLNode<T, V>> leftOwner(
                        static_cast<AVLNode<T, V> *>(parent->getLeftNodeOwnership().release()));

                parent->setLeftChild(rotateRight(std::move(leftOwner)));
            } else {
                std::unique_ptr<AVLNode<T, V>> rightOwner(
                        static_cast<AVLNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateRight(std::move(rightOwner)));
            }
        }
    }

    void rotateLeftP(AVLNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<AVLNode<T, V>> rootOwner(
                    static_cast<AVLNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateLeft(std::move(rootOwner)));
        } else {

            if (parent->getLeftChild() == root) {
                std::unique_ptr<AVLNode<T, V>> leftOwner(
                        static_cast<AVLNode<T, V> *>(parent->getLeftNodeOwnership().release()));
                parent->setLeftChild(rotateLeft(std::move(leftOwner)));
            } else {
                std::unique_ptr<AVLNode<T, V>> rightOwner(
                        static_cast<AVLNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateLeft(std::move(rightOwner)));
            }
        }
    }

    void rebalance(AVLNode<T, V> *root, int balance) {
        if (balance > 1) {

            auto leftChild = (AVLNode<T, V> *) root->getLeftChild();

            if (getBalance(leftChild) >= 0) {
                rotateRightP(root);
            } else {
                std::unique_ptr<AVLNode<T, V>> leftChildOwner(
                        static_cast<AVLNode<T, V> *>(root->getLeftNodeOwnership().release()));

                auto result = rotateLeft(std::move(leftChildOwner));

                root->setLeftChild(std::move(result));

                rotateRightP(root);
            }

        } else if (balance < -1) {

            auto rightChild = (AVLNode<T, V> *) root->getRightChild();

            if (getBalance(rightChild) <= 0) {
                rotateLeftP(root);
            } else {
                std::unique_ptr<AVLNode<T, V>> rightChildOwner(
                        static_cast<AVLNode<T, V> *>(root->getRightNodeOwnership().release()));

                auto result = rotateRight(std::move(rightChildOwner));

                root->setRightChild(std::move(result));

                rotateLeftP(root);
            }
        }
    }

    void updateBalance(AVLNode<T, V> *leaf) {

        if (leaf == nullptr) return;

        auto leftHeight = getHeight((AVLNode<T, V> *) leaf->getLeftChild());
        auto rightHeight = getHeight((AVLNode<T, V> *) leaf->getRightChild());

        leaf->setHeight(1 + std::max(leftHeight, rightHeight));

        int bal = leftHeight - rightHeight;

        if (bal > 1 || bal < -1) {
            rebalance(leaf, bal);
        }

        updateBalance((AVLNode<T, V> *) leaf->getParent());

    }

public:
    AvlTree() : BinarySearchTree<T, V>() {}

    int getTreeHeight() override {
        return ((AVLNode<T, V>*) this->getRoot())->getHeight();
    }

    std::unique_ptr<TreeNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value,
                                                   TreeNode<T, V> *parent) override {
        return std::unique_ptr<TreeNode<T, V>>(
                (TreeNode<T, V> *) new AVLNode<T, V>(key, value, (AVLNode<T, V> *) parent));
    }

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {
        TreeNode<T, V> *newNode = this->addNode(key, value);

        updateBalance((AVLNode<T, V> *) newNode);
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {
        auto removedNodeInfo = this->removeNode(key);

        if (removedNodeInfo) {
            std::unique_ptr<AVLNode<T, V>> removedNode(
                    static_cast<AVLNode<T, V> *> (std::get<1>(*removedNodeInfo).release()));

            updateBalance((AVLNode<T, V> *) removedNode->getParent());

            return std::get<1>(std::get<0>(*removedNodeInfo));
        } else {
            std::cout << "not present" << std::endl;
            return std::nullopt;
        }
    }

    std::optional<node_info<T, V>> popSmallest() override {

        if (this->leftMostNode != nullptr) {

            auto result = this->popSmallestNode();

            updateBalance((AVLNode<T, V> *) std::get<1>(result)->getParent());

            return std::get<0>(result);
        }

        return std::nullopt;
    }

    std::optional<node_info<T, V>> popLargest() override {
        if (this->rightMostNode != nullptr) {

            auto result = this->popLargestNode();

            updateBalance((AVLNode<T, V> *) std::get<1>(result)->getParent());

            return std::get<0>(result);
        }
        return std::nullopt;
    }

};


#endif //TRABALHO1_AVLTREE_H

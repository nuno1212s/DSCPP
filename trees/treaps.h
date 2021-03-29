#ifndef TRABALHO1_TREAPS_H
#define TRABALHO1_TREAPS_H

#include "binarytrees.h"
#include <random>

template<typename T, typename V>
class TreapNode : public TreeNode<T, V> {

private:
    int heapWeight;

public:
    TreapNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreapNode<T, V> *parent,
              int heapWeight) :
            TreeNode<T, V>(std::move(key), std::move(value), parent),
            heapWeight(heapWeight) {}

    int getHeapWeight() const {
        return heapWeight;
    }

};

template<typename T, typename V>
class Treap : public BinarySearchTree<T, V> {
private:
    std::random_device randomEngine;
    std::mt19937 generator;
    std::uniform_int_distribution<int> distribution;

public:
    Treap() : randomEngine() {
        this->generator = std::mt19937(randomEngine());
    }

protected:
    std::unique_ptr<TreeNode<T, V>>
    initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent) override {
        return std::make_unique<TreapNode<T, V>>(key, value, parent, distribution(generator));
    }

    void rotateRightP(TreapNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<TreapNode<T, V>>
                    rootOwner(
                    static_cast<TreapNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateRight(std::move(rootOwner)));

        } else {
            if (parent->getLeftChild() == root) {
                std::unique_ptr<TreapNode<T, V>> leftOwner(
                        static_cast<TreapNode<T, V> *>(parent->getLeftNodeOwnership().release()));

                parent->setLeftChild(rotateRight(std::move(leftOwner)));
            } else {
                std::unique_ptr<TreapNode<T, V>> rightOwner(
                        static_cast<TreapNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateRight(std::move(rightOwner)));
            }
        }
    }

    void rotateLeftP(TreapNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<TreapNode<T, V>> rootOwner(
                    static_cast<TreapNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(rotateLeft(std::move(rootOwner)));
        } else {

            if (parent->getLeftChild() == root) {
                std::unique_ptr<TreapNode<T, V>> leftOwner(
                        static_cast<TreapNode<T, V> *>(parent->getLeftNodeOwnership().release()));
                parent->setLeftChild(rotateLeft(std::move(leftOwner)));
            } else {
                std::unique_ptr<TreapNode<T, V>> rightOwner(
                        static_cast<TreapNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(rotateLeft(std::move(rightOwner)));
            }
        }
    }

private:
    void heapify(TreapNode<T, V> *leaf) {

        if (leaf == nullptr) return;

        auto *parent = leaf->getParent();

        if (parent == nullptr) return;

        if (parent->getLeftChild() == leaf) {

            if (leaf->getHeapWeight() > ((TreapNode<T, V> *) parent)->getHeapWeight()) {
                rotateRightP(parent);

                //The leaf is now the parent, so we recursively call this rotation
                heapify(leaf);
            }

        } else {

            if (leaf->getHeapWeight() > ((TreapNode<T, V> *) parent)->getHeapWeight()) {
                rotateLeftP(parent);

                heapify(leaf);
            }

        }

    }

public:
    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        auto addedNode = this->addNode(key, value);

        heapify(addedNode);

    }

    std::shared_ptr<V> remove(const T &key) override {

        auto *rootNode = (TreapNode<T, V> *) this->getRoot();

        while (rootNode != nullptr) {

            auto keyValue = *(rootNode->getKeyVal());

            if (keyValue == key) {

                if (rootNode->getLeftChild() == nullptr && rootNode->getRightChild() == nullptr) {
                    //Leaf node
                    auto *parent = rootNode->getParent();

                    if (parent == nullptr) {
                        auto rootOwnership = this->getRootNodeOwnership();

                        return rootNode->getKey();
                    }

                    if (parent->getLeftChild() == rootNode) {

                        auto leftNode = parent->getLeftNodeOwnership();

                    } else {

                        auto rightNode = parent->getRightNodeOwnership();

                    }

                } else if (rootNode->getLeftChild() != nullptr || rootNode->getRightChild() != nullptr) {

                } else {

                }

            } else if (key < keyValue) {
                rootNode = (TreapNode<T, V> *) rootNode->getLeftChild();
            } else {
                rootNode = (TreapNode<T, V> *) rootNode->getRightChild();
            }
        }

    }

};

#endif //TRABALHO1_TREAPS_H

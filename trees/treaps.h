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

    ~TreapNode() override {}

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
        auto node = std::make_unique<TreapNode<T, V>>(key, value, (TreapNode<T, V> *) parent, distribution(generator));

        //std::cout << "New node with weight "  << node->getHeapWeight() << std::endl;

        return std::move(node);
    }

    void rotateRightP(TreeNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<TreeNode<T, V>> rootOwner = this->getRootNodeOwnership();

            this->setRootNode(this->rotateRight(std::move(rootOwner)));

        } else {
            if (parent->getLeftChild() == root) {
                std::unique_ptr<TreeNode<T, V>> leftOwner = parent->getLeftNodeOwnership();

                parent->setLeftChild(this->rotateRight(std::move(leftOwner)));
            } else {
                std::unique_ptr<TreeNode<T, V>> rightOwner = parent->getRightNodeOwnership();

                parent->setRightChild(this->rotateRight(std::move(rightOwner)));
            }
        }
    }

    void rotateLeftP(TreeNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<TreeNode<T, V>> rootOwner = this->getRootNodeOwnership();

            this->setRootNode(this->rotateLeft(std::move(rootOwner)));
        } else {

            if (parent->getLeftChild() == root) {
                std::unique_ptr<TreeNode<T, V>> leftOwner = parent->getLeftNodeOwnership();

                parent->setLeftChild(this->rotateLeft(std::move(leftOwner)));
            } else {
                std::unique_ptr<TreeNode<T, V>> rightOwner = parent->getRightNodeOwnership();

                parent->setRightChild(this->rotateLeft(std::move(rightOwner)));
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

                //The leaf is now the parent, so we recursively call this rotation
                heapify(leaf);
            }
        }
    }

    /**
     * Move a node to where he is either a leaf
     * Or only has one child
     * @param root
     */
    void bottomfy(TreapNode<T, V> *root) {

        if (root == nullptr) {
            return;
        }

        auto *leftChild = (TreapNode<T, V> *) root->getLeftChild();
        auto *rightChild = (TreapNode<T, V> *) root->getRightChild();

        if (leftChild != nullptr && rightChild != nullptr) {

            if (leftChild->getHeapWeight() > rightChild->getHeapWeight()) {
                //The left node is now the root of this subtree and we recurse down the right subtree
                //The root of the right subtree is where our root node is now
                rotateRightP(root);

                bottomfy(root);
            } else {
                rotateLeftP(root);

                bottomfy(root);
            }
        }

    }

    void deleteNode(TreapNode<T, V> *rootNode) {

        if (rootNode->getLeftChild() == nullptr && rootNode->getRightChild() == nullptr) {
            //Leaf node, so we just have to delete it
            auto *parent = rootNode->getParent();

            if (parent == nullptr) {
                auto rootOwnership = this->getRootNodeOwnership();
            } else if (parent->getLeftChild() == rootNode) {

                std::unique_ptr<TreeNode<T, V>> leftNode = parent->getLeftNodeOwnership();
            } else {

                std::unique_ptr<TreeNode<T, V>> rightNode = parent->getRightNodeOwnership();
            }

            this->treeSize--;

        } else if (rootNode->getLeftChild() != nullptr || rootNode->getRightChild() != nullptr) {
            //SkipNode only has one child, remove it and set the child in it's place

            std::unique_ptr<TreeNode<T, V>> child, node;

            if (rootNode->getLeftChild() != nullptr) {
                child = rootNode->getLeftNodeOwnership();
            } else {
                child = rootNode->getRightNodeOwnership();
            }

            auto parent = rootNode->getParent();

            if (parent == nullptr) {
                this->setRootNode(std::move(child));
            } else {
                if (parent->getLeftChild() == rootNode) {
                    //By acquiring the node, since it is a unique_ptr , it will be freed as soon as we exit the scope
                    parent->getLeftNodeOwnership();
                    parent->setLeftChild(std::move(child));
                } else {
                    parent->getRightNodeOwnership();
                    parent->setRightChild(std::move(child));
                }
            }

            this->treeSize--;

        } else {
            //SkipNode has 2 children
            bottomfy(rootNode);

            deleteNode(rootNode);
        }
    }

public:
    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        auto addedNode = this->addNode(key, value);

        heapify((TreapNode<T, V> *) addedNode);

    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        auto *rootNode = (TreapNode<T, V> *) this->getRoot();

        while (rootNode != nullptr) {

            auto keyValue = *(rootNode->getKeyVal());

            if (keyValue == key) {

                std::shared_ptr<V> value = rootNode->getValue();

                deleteNode(rootNode);

                return value;

            } else if (key < keyValue) {
                rootNode = (TreapNode<T, V> *) rootNode->getLeftChild();
            } else {
                rootNode = (TreapNode<T, V> *) rootNode->getRightChild();
            }
        }

        return std::nullopt;
    }

};

#endif //TRABALHO1_TREAPS_H

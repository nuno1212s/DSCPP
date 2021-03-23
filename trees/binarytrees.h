#ifndef TRABALHO1_BINARYTREES_H
#define TRABALHO1_BINARYTREES_H

#include "trees.h"
#include <tuple>
#include <memory>
#include <vector>
#include <stack>
#include <iostream>

template<typename T, typename V>
class TreeNode {

protected:

    std::shared_ptr<T> key;
    std::shared_ptr<V> value;

    TreeNode<T, V> *parent;

    std::unique_ptr<TreeNode<T, V>> leftNode, rightNode;


public:
    TreeNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent) : key(key), value(value),
                                                                                         parent(parent),
                                                                                         leftNode(nullptr),
                                                                                         rightNode(nullptr) {}

    ~TreeNode() {

        //Delete our references to these nodes
        this->key.reset();
        this->value.reset();

        this->leftNode.reset();
        this->rightNode.reset();

    }

    std::shared_ptr<T> getKey() const {
        return std::shared_ptr<T>(key);
    }

    std::shared_ptr<V> getValue() const {
        return std::shared_ptr<V>(value);
    }

    void setKey(std::shared_ptr<T> key) {
        this->key = key;
    }

    void setValue(std::shared_ptr<V> value) {
        this->value = value;
    }

    void setParent(TreeNode<T, V> *parent) {
        this->parent = parent;
    }

    TreeNode<T, V> *getParent() const {
        return this->parent;
    }

    TreeNode<T, V> *getLeftChild() const {
        return leftNode.get();
    }

    TreeNode<T, V> *getRightChild() const {
        return rightNode.get();
    }

    std::unique_ptr<TreeNode<T, V>> getLeftNodeOwnership() {
        return std::move(leftNode);
    }

    std::unique_ptr<TreeNode<T, V>> getRightNodeOwnership() {
        return std::move(rightNode);
    }

    void setLeftChild(std::unique_ptr<TreeNode<T, V>> node) {
        this->leftNode = std::move(node);

        this->leftNode->setParent(this);
    }

    void setRightChild(std::unique_ptr<TreeNode<T, V>> node) {
        this->rightNode = std::move(node);

        this->rightNode->setParent(this);
    }
};

template<typename T, typename V>
class BinarySearchTree : public TreeMap<T, V> {

protected:
    std::unique_ptr<TreeNode<T, V>> rootNode;

    TreeNode<T, V> *leftMostNode, *rightMostNode;

    int treeSize;

    BinarySearchTree() : treeSize(0), rootNode(nullptr),
                         leftMostNode(nullptr), rightMostNode(nullptr) {

    }

    ~BinarySearchTree() {
        this->rootNode.reset();
    }

    TreeNode<T, V> *getRoot() {
        return this->rootNode.get();
    }

    std::unique_ptr<TreeNode<T, V>> getRootNodeOwnership() {
        return std::move(rootNode);
    }

    void setRootNode(std::unique_ptr<TreeNode<T, V>> rootNode) {
        this->rootNode = std::move(rootNode);
        this->rootNode->setParent(nullptr);
    }

    void setLeftMostNode(TreeNode<T, V> *leftMost) {
        this->leftMostNode = leftMost;
    }

    void setRightMostNode(TreeNode<T, V> *rightMost) {
        this->rightMostNode = rightMost;
    }

protected:

    void inOrderHelper(TreeNode<T, V> *current, std::vector<node_info<T, V>> *destination) {

        if (current == nullptr) return;

        inOrderHelper(current->getLeftChild(), destination);

        destination->emplace_back(std::make_tuple(current->getKey(), current->getValue()));

        inOrderHelper(current->getRightChild(), destination);

    }

    void preOrderHelper(TreeNode<T, V> *current, std::vector<node_info<T, V>> *destination) {
        if (current == nullptr) return;

        destination->push_back(std::make_tuple(current->getKey(), current->getValue()));

        preOrderHelper(current->getLeftChild(), destination);

        preOrderHelper(current->getRightChild(), destination);
    }

    TreeNode<T, V> *getRightMostNodeInTree(TreeNode<T, V> *root) {

        auto current = root;

        while (current->getRightChild() != nullptr) {
            current = current->getRightChild();
        }

        return current;
    }

    TreeNode<T, V> *getLeftMostNodeInTree(TreeNode<T, V> *root) {

        auto current = root;

        while (current->getLeftChild() != nullptr) {
            current = current->getLeftChild();
        }

        return current;
    }

    virtual std::unique_ptr<TreeNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value,
                                                           TreeNode<T, V> *parent) = 0;

    TreeNode<T, V> *addNode(std::shared_ptr<T> key, std::shared_ptr<V> value) {

        if (this->getRoot() == nullptr) {

            std::unique_ptr<TreeNode<T, V>> newRoot = initializeNode(key, value, nullptr);

            std::cout << "new root node " << std::endl;

            this->setRootNode(std::move(newRoot));

            this->setLeftMostNode(this->getRoot());
            this->setRightMostNode(this->getRoot());

            this->treeSize++;

            return this->getRoot();
        }

        TreeNode<T, V> *currentRoot = this->getRoot(),
                *parent = nullptr;

        while (currentRoot != nullptr) {

            parent = currentRoot;

            auto keyValue = *currentRoot->getKey().get();

            if (keyValue == *key) {

                currentRoot->setValue(std::move(value));

            } else if (*key < keyValue) {
                currentRoot = currentRoot->getLeftChild();
            } else {
                currentRoot = currentRoot->getRightChild();
            }
        }

        if (parent == nullptr) {
            //what?
            std::cout << "parent is null where it shouldn't be" << std::endl;

            return nullptr;
        }

        auto keyValue = *parent->getKey().get();

        T *keyRef = key.get();

        std::unique_ptr<TreeNode<T, V>> newNode = initializeNode(key, value, parent);

        TreeNode<T, V> *newNodeP = newNode.get();

        if (*keyRef < keyValue) {

            //When adding a child to the left of a node, check if the key is smaller than the current smallest
            //If so, update the smallest node
            if (*keyRef < *std::get<0>(*this->peekSmallest())) {
                this->setLeftMostNode(newNodeP);
            }

            parent->setLeftChild(std::move(newNode));

            std::cout << "Left " << std::endl;
        } else {

            //When adding a child to the right of a node, check if the key is largest than the current largest
            //If so, update the largest node
            if (*keyRef > *std::get<0>(*this->peekLargest())) {
                this->setRightMostNode(newNodeP);
            }

            parent->setRightChild(std::move(newNode));

            std::cout << "Right " << std::endl;
        }

        this->treeSize++;

        return newNodeP;
    }

    /**
     * This returns the node info associated with the key, and the node that got removed from the tree
     * (Most of the time, this node is not the node that contained the information)
     * @param key
     * @return
     */
    std::optional<std::tuple<node_info<T, V>, std::unique_ptr<TreeNode<T, V>>>> removeNode(const T &key) {

        if (this->getRoot() == nullptr) {
            return std::nullopt;
        }

        TreeNode<T, V> *root = this->getRoot();

        node_info<T, V> nodeInfo;

        const T *currentKey = &key;

        while (root != nullptr) {

            if (*(root->getKey()) == *currentKey) {

                nodeInfo = std::make_tuple(root->getKey(), root->getValue());

                TreeNode<T, V> *toReplace = nullptr;

                std::unique_ptr<TreeNode<T, V>> child;

                if (root->getRightChild() != nullptr && root->getLeftChild() != nullptr) {
                    toReplace = getLeftMostNodeInTree(root->getRightChild());
                } else if (root->getLeftChild() != nullptr) {
                    child = root->getLeftNodeOwnership();
                } else if (root->getRightChild() != nullptr) {
                    child = root->getRightNodeOwnership();
                }

                if (root->getRightChild() == nullptr) {
                    //If we are the right most node in this subtree, check if we are global
                    auto largest = this->peekLargest();
                    if (largest) {
                        if (key == *std::get<0>(*largest)) {
                            std::cout<< "Removing largest, setting new one to: " << *root->getParent()->getKey() << std::endl;
                            this->setRightMostNode(root->getParent());
                        }
                    }
                }

                if (root->getLeftChild() == nullptr) {
                    //If we are the left most node in this subtree, check if we are global
                    auto smallest = this->peekSmallest();

                    if (key == *std::get<0>(*smallest)) {
                        this->setLeftMostNode(root->getParent());
                    }
                }

                if (child.get() != nullptr) {
                    if (root->getParent() == nullptr) {
                        this->setRootNode(std::move(child));
                    } else {
                        if (root->getParent()->getLeftChild() == root) {
                            root->getParent()->setLeftChild(std::move(child));
                        } else {
                            root->getParent()->setRightChild(std::move(child));
                        }
                    }
                }

                if (toReplace != nullptr && toReplace != root) {
                    //toReplace is the left most node of the right sub tree

                    //Set the root as the toReplace values
                    root->setKey(toReplace->getKey());
                    root->setValue(toReplace->getValue());

                    //Update the key we are searching for to remove the duplicate
                    currentKey = toReplace->getKey().get();
                    root = root->getRightChild();

                    std::cout << "Replaced " << *root->getKey() << " with " << *toReplace->getKey() << std::endl;

                } else {

                    this->treeSize--;

                    std::cout << "To replace is null or matches root" << std::endl;

                    //This node is a leaf
                    if (root->getParent() != nullptr) {

                        auto parentNode = root->getParent();

                        std::unique_ptr<TreeNode<T, V>> child;

                        if (parentNode->getLeftChild() == root) {
                            child = parentNode->getLeftNodeOwnership();

                            //If we are the left child, then it is the left most node
                            //So we can only have children on the right
                            if (child->getRightChild() != nullptr) {
                                parentNode->setLeftChild(std::move(child->getRightNodeOwnership()));
                                std::cout << "FOUND ONE" << std::endl;
                            }
                        } else {
                            child = parentNode->getRightNodeOwnership();

                            //If we are the right child, then it is the right most node
                            //So we can only have left on the right
                            if (child->getLeftChild() != nullptr) {
                                parentNode->setRightChild(std::move(child->getLeftNodeOwnership()));
                                std::cout << "FOUND ONE" << std::endl;
                            }
                        }

                        return std::make_tuple(nodeInfo, std::move(child));
                    } else {
                        return std::make_tuple(nodeInfo, std::move(this->getRootNodeOwnership()));
                    }

                }

            } else if (*currentKey < (*root->getKey())) {
                root = root->getLeftChild();
            } else {
                root = root->getRightChild();
            }

        }

        return std::nullopt;
    }

    void searchInTree(TreeNode<T, V> *root, const T &base, const T &max, std::vector<node_info<T, V>> *result) {

        if (root == nullptr) return;

        if (*root->getKey() >= base && *root->getKey() <= max) {

            //Search first in the left sub tree, so that the smaller values come out first in the vector
            searchInTree(root->getLeftChild(), base, max, result);

            result->push_back(std::make_tuple(root->getKey(), root->getValue()));

            //After inserting our value (larger than the left sub tree and smaller than the right subtree) search the right sub tree,so
            //that in the end, the elements get added in order
            searchInTree(root->getRightChild(), base, max, result);
        }

    }

public:

    int size() override {
        return this->treeSize;
    }

    std::optional<std::shared_ptr<V>> get(const T &key) override {

        if (this->getRoot() != nullptr) {

            TreeNode<T, V> *rootRef = this->getRoot();

            while (rootRef != nullptr) {

                auto rootKey = *rootRef->getKey();

                if (rootKey == key) {
                    return rootRef->getValue();
                } else if (key < rootKey) {
                    rootRef = rootRef->getLeftChild();
                } else {
                    rootRef = rootRef->getRightChild();
                }

            }

        }

        return std::nullopt;
    }

    bool hasKey(const T &key) override {

        if (this->getRoot() != nullptr) {

            TreeNode<T, V> *rootRef = this->getRoot();

            while (rootRef != nullptr) {

                auto rootKey = (*rootRef->getKey());

                if (rootKey == key) {
                    return true;
                } else if (key < rootKey) {
                    rootRef = rootRef->getLeftChild();
                } else {
                    rootRef = rootRef->getRightChild();
                }
            }
        }

        return false;
    }

    std::optional<node_info<T, V>> peekSmallest() override {

        if (this->leftMostNode != nullptr) {
            return std::make_tuple(this->leftMostNode->getKey(), this->leftMostNode->getValue());
        }

        return std::nullopt;
    }

    std::optional<node_info<T, V>> peekLargest() override {

        if (this->rightMostNode != nullptr) {
            return std::make_tuple(this->rightMostNode->getKey(), this->rightMostNode->getValue());
        }

        return std::nullopt;
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        auto result = removeNode(key);

        if (result) {

            node_info<T, V> keyValue;

            std::tie(keyValue, std::ignore) = *result;

            std::shared_ptr<V> value;

            std::tie(std::ignore, value) = keyValue;

            return value;
        } else {
            return std::nullopt;
        }

    }

    /**
     * Range search, returns elements in-order by using the in order traversal
     * @param base The base value
     * @param max The max value
     * @return
     */
    std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) override {

        auto vector = std::make_unique<std::vector<node_info<T, V>>>();

        searchInTree(this->rootNode.get(), base, max, vector.get());

        return vector;
    }

    std::unique_ptr<std::vector<std::shared_ptr<T>>> keys() override {

        auto vector = std::make_unique<std::vector<std::shared_ptr<T>>>(this->size());

        std::vector<node_info<T, V>> nodeCache(this->size());

        inOrderHelper(this->getRoot(), &nodeCache);

        for (node_info<T, V> &start : nodeCache) {
            vector->push_back(std::move(std::get<0>(start)));
        }

        return vector;
    }

    std::unique_ptr<std::vector<std::shared_ptr<V>>> values() override {

        auto vector = std::make_unique<std::vector<std::shared_ptr<V>>>(this->size());

        std::vector<node_info<T, V>> nodeCache(this->size());

        inOrderHelper(this->getRoot(), &nodeCache);

        for (node_info<T, V> &start : nodeCache) {
            vector->push_back(std::move(std::get<1>(start)));
        }

        return vector;
    }

    std::unique_ptr<std::vector<node_info<T, V>>> entries() override {

        auto vector = std::make_unique<std::vector<node_info<T, V>>>();

        inOrderHelper(this->getRoot(), vector.get());

        return vector;
    }

};

#endif //TRABALHO1_BINARYTREES_H

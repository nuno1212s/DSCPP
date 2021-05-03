#ifndef TRABALHO1_BINARYTREES_H
#define TRABALHO1_BINARYTREES_H

#include "../datastructures.h"
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
    TreeNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent) : key(std::move(key)),
                                                                                         value(std::move(value)),
                                                                                         parent(parent),
                                                                                         leftNode(nullptr),
                                                                                         rightNode(nullptr) {}

    ~TreeNode() {

        std::cout << "Deleted node with key " << *getKeyVal() << std::endl;

        //Delete our references to these nodes
        this->key.reset();
        this->value.reset();

        this->leftNode.reset();
        this->rightNode.reset();

    }

    const T *getKeyVal() const {
        return key.get();
    }

    const V *getValVal() const {
        return value.get();
    }

    std::shared_ptr<T> getKey() const {
        return key;
    }

    std::shared_ptr<V> getValue() const {
        return value;
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

        if (this->leftNode.get() != nullptr)
            this->leftNode->setParent(this);
    }

    void setRightChild(std::unique_ptr<TreeNode<T, V>> node) {
        this->rightNode = std::move(node);

        if (this->rightNode.get() != nullptr)
            this->rightNode->setParent(this);
    }
};

template<typename T, typename V>
class BinarySearchTree : public OrderedMap<T, V> {

protected:
    std::unique_ptr<TreeNode<T, V>> rootNode;

    TreeNode<T, V> *leftMostNode, *rightMostNode;

    unsigned int treeSize;

    BinarySearchTree() : treeSize(0), rootNode(nullptr),
                         leftMostNode(nullptr), rightMostNode(nullptr) {}

    ~BinarySearchTree() {
        this->rootNode.reset();
    }

    std::unique_ptr<TreeNode<T, V>> getRootNodeOwnership() {
        return std::move(rootNode);
    }

    void setRootNode(std::unique_ptr<TreeNode<T, V>> rootNode) {
        this->rootNode = std::move(rootNode);

        if (this->rootNode.get() != nullptr)
            this->rootNode->setParent(nullptr);
    }

    void setLeftMostNode(TreeNode<T, V> *leftMost) {
        this->leftMostNode = leftMost;
    }

    void setRightMostNode(TreeNode<T, V> *rightMost) {
        this->rightMostNode = rightMost;
    }

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

            auto keyValue = *currentRoot->getKeyVal();

            if (keyValue == *key) {

                currentRoot->setValue(std::move(value));

                return currentRoot;

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

        auto keyValue = *parent->getKeyVal();

        T *keyRef = key.get();

        std::unique_ptr<TreeNode<T, V>> newNode = initializeNode(std::move(key), std::move(value), parent);

        TreeNode<T, V> *newNodeP = newNode.get();

        if (*keyRef < keyValue) {

            //When adding a child to the left of a node, check if the key is smaller than the current smallest
            //If so, update the smallest node
            if (this->leftMostNode != nullptr) {
                if (*keyRef < *this->leftMostNode->getKeyVal()) {
                    this->setLeftMostNode(newNodeP);
                }
            } else {
                this->setLeftMostNode(newNodeP);
            }

            parent->setLeftChild(std::move(newNode));

//            std::cout << "Left " << std::endl;
        } else {

            //When adding a child to the right of a node, check if the key is largest than the current largest
            //If so, update the largest node
            if (this->rightMostNode != nullptr) {
                if (*keyRef > *this->rightMostNode->getKeyVal()) {
                    this->setRightMostNode(newNodeP);
                }
            } else {
                this->setRightMostNode(newNodeP);
            }

            parent->setRightChild(std::move(newNode));

//            std::cout << "Right " << std::endl;
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
    std::optional<std::tuple<node_info<T, V>, std::unique_ptr<TreeNode<T, V>>, TreeNode<T, V> *>>
    removeNode(const T &key) {

        if (this->getRoot() == nullptr) {
            return std::nullopt;
        }

        TreeNode<T, V> *root = this->getRoot();

        node_info<T, V> nodeInfo;

        const T *currentKey = &key;

        while (root != nullptr) {

            if (*(root->getKeyVal()) == *currentKey) {

                nodeInfo = std::make_tuple(root->getKey(), root->getValue());

                TreeNode<T, V> *toReplace = nullptr;

                std::unique_ptr<TreeNode<T, V>> child(nullptr);

                if (root->getRightChild() == nullptr) {
                    //If we are the right most node in this subtree, check if we are global
                    auto largest = this->peekLargest();
                    if (largest) {
                        if (key == *std::get<0>(*largest)) {
                            this->handleRemoveLargestNode();
                        }
                    }
                }

                if (root->getLeftChild() == nullptr) {
                    //If we are the left most node in this subtree, check if we are global
                    auto smallest = this->peekSmallest();

                    if (key == *std::get<0>(*smallest)) {
                        this->handleRemoveSmallestNode();
                    }
                }

                if (root->getRightChild() != nullptr && root->getLeftChild() != nullptr) {
                    toReplace = getLeftMostNodeInTree(root->getRightChild());
                } else if (root->getLeftChild() != nullptr) {
                    child = root->getLeftNodeOwnership();
                } else if (root->getRightChild() != nullptr) {
                    child = root->getRightNodeOwnership();
                }

                if (child.get() != nullptr) {
                    this->treeSize--;

                    auto childP = child.get();

                    if (root->getParent() == nullptr) {

                        auto oldRoot = this->getRootNodeOwnership();

                        this->setRootNode(std::move(child));

                        return std::make_tuple(nodeInfo, std::move(oldRoot), childP);
                    } else {
                        if (root->getParent()->getLeftChild() == root) {

                            auto ownership = root->getParent()->getLeftNodeOwnership();

                            root->getParent()->setLeftChild(std::move(child));
                            return std::make_tuple(nodeInfo, std::move(ownership), childP);
                        } else {
                            auto ownership = root->getParent()->getRightNodeOwnership();

                            root->getParent()->setRightChild(std::move(child));

                            return std::make_tuple(nodeInfo, std::move(ownership), childP);
                        }
                    }
                } else if (toReplace != nullptr && toReplace != root) {
                    //toReplace is the left most node of the right sub tree

                    //Set the root as the toReplace values
                    root->setKey(toReplace->getKey());
                    root->setValue(toReplace->getValue());

                    //Update the key we are searching for to remove the duplicate
                    currentKey = toReplace->getKeyVal();
                    root = root->getRightChild();

                } else {

                    this->treeSize--;

                    if (root->getParent() != nullptr) {
                        //This node is a leaf and has a parent
                        auto parentNode = root->getParent();

                        std::unique_ptr<TreeNode<T, V>> child(nullptr);

                        if (parentNode->getLeftChild() == root) {
                            child = parentNode->getLeftNodeOwnership();
                        } else {
                            child = parentNode->getRightNodeOwnership();
                        }

                        return std::make_tuple(nodeInfo, std::move(child), nullptr);
                    } else {
                        //SkipNode is the root and has no leaves so it must be the only node in the tree
                        return std::make_tuple(nodeInfo, std::move(this->getRootNodeOwnership()), nullptr);
                    }
                }
            } else if (*currentKey < *root->getKeyVal()) {
                root = root->getLeftChild();
            } else {
                root = root->getRightChild();
            }
        }

        return std::nullopt;
    }

    void searchInTree(TreeNode<T, V> *root, const T &base, const T &max, std::vector<node_info<T, V>> *result) {

        if (root == nullptr) return;

        if (*root->getKeyVal() >= base && *root->getKeyVal() <= max) {

            //Search first in the left sub tree, so that the smaller values come out first in the vector
            searchInTree(root->getLeftChild(), base, max, result);

            result->push_back(std::make_tuple(root->getKey(), root->getValue()));

            //After inserting our value (larger than the left sub tree and smaller than the right subtree) search the right sub tree,so
            //that in the end, the elements get added in order
            searchInTree(root->getRightChild(), base, max, result);
        }

    }

    void handleRemoveLargestNode() {
        if (this->peekLargest()) {

            auto largest = this->rightMostNode;

            //We don't have to check whether this node has a right child as it is always the right most node in the tree

            if (largest->getLeftChild() != nullptr) {

                auto rightMost = getRightMostNodeInTree(largest->getLeftChild());

                this->setRightMostNode(rightMost);

            } else {
                this->setRightMostNode(largest->getParent());
            }
        }
    }

    void handleRemoveSmallestNode() {
        if (this->peekSmallest()) {

            auto smallest = this->leftMostNode;

            if (smallest->getRightChild() != nullptr) {

                auto leftMost = getLeftMostNodeInTree(smallest->getRightChild());

                this->setLeftMostNode(leftMost);

            } else {
                this->setLeftMostNode(smallest->getParent());
            }
        }
    }

    std::tuple<node_info<T, V>, std::unique_ptr<TreeNode<T, V>>> popLargestNode() {

        auto rightMostNodePointer = this->rightMostNode;

        //Set the new largest node
        this->handleRemoveLargestNode();

        //remove the node from the tree
        std::unique_ptr<TreeNode<T, V>> rightMost;

        if (rightMostNodePointer->getParent() == nullptr) {
            rightMost = this->getRootNodeOwnership();

            if (rightMost->getLeftChild() != nullptr) {
                this->setRootNode(rightMost->getLeftNodeOwnership());
            }
        } else {
            rightMost = rightMostNodePointer->getParent()->getRightNodeOwnership();

            //Since this is the right most node, we don't even need to check for right children
            if (rightMost->getLeftChild() != nullptr) {
                rightMost->getParent()->setRightChild(std::move(rightMost->getLeftNodeOwnership()));
            }
        }

        if (this->leftMostNode == rightMostNodePointer) {
            //The only way this happens is if the root is the only member of the tree, so if we remove that
            //We have to remove our reference to it
            this->leftMostNode = nullptr;
        }

        this->treeSize--;

        return std::make_tuple(std::make_tuple(rightMost->getKey(), rightMost->getValue()), std::move(rightMost));
    }

    std::tuple<node_info<T, V>, std::unique_ptr<TreeNode<T, V>>> popSmallestNode() {

        auto leftMostNodePointer = this->leftMostNode;

        //Set the new smallest node
        this->handleRemoveSmallestNode();

        //remove the node from the tree
        std::unique_ptr<TreeNode<T, V>> leftMost;

        if (leftMostNodePointer->getParent() == nullptr) {
            leftMost = this->getRootNodeOwnership();

            if (leftMost->getRightChild() != nullptr) {
                this->setRootNode(leftMost->getRightNodeOwnership());
            }
        } else {
            leftMost = leftMostNodePointer->getParent()->getLeftNodeOwnership();

            //Since this is the left most node, we don't even need to check for left children
            if (leftMost->getRightChild() != nullptr) {
                leftMost->getParent()->setLeftChild(std::move(leftMost->getRightNodeOwnership()));
            }
        }

        if (this->rightMostNode == leftMostNodePointer) {
            //The only way this happens is if the root is the only member of the tree, so if we remove that
            //We have to remove our reference to it
            this->rightMostNode = nullptr;
        }

        this->treeSize--;

        return std::make_tuple(std::make_tuple(leftMost->getKey(), leftMost->getValue()), std::move(leftMost));
    }

    virtual std::unique_ptr<TreeNode<T, V>> rotateLeft(std::unique_ptr<TreeNode<T, V>> root) {

        std::unique_ptr<TreeNode<T, V>> right = root->getRightNodeOwnership();

        TreeNode<T, V> *rootRef = root.get(), *rightRef = right.get();

        std::cout << "Rotating left around root: " << *(root->getKeyVal()) << std::endl;

        if (rightRef->getLeftChild() != nullptr) {
            //Move the left child of the root's right child into the right child of the root
            rootRef->setRightChild(std::move(rightRef->getLeftNodeOwnership()));
        }

        //The set child methods automatically set the parent to the node that they have been moved to
        rightRef->setLeftChild(std::move(root));

        //The new root is now the right node
        return right;
    }

    virtual std::unique_ptr<TreeNode<T, V>> rotateRight(std::unique_ptr<TreeNode<T, V>> root) {

        std::unique_ptr<TreeNode<T, V>> left = root->getLeftNodeOwnership();

        TreeNode<T, V> *rootRef = root.get(), *leftRef = left.get();

        std::cout << "Rotating right around root: " << *(root->getKeyVal()) << std::endl;

        if (leftRef->getRightChild() != nullptr) {
            rootRef->setLeftChild(std::move(leftRef->getRightNodeOwnership()));
        }

        leftRef->setRightChild(std::move(root));

        return left;
    }

    TreeNode<T, V> *getNodeBy(const T &key) {

        if (this->getRoot() != nullptr) {

            TreeNode<T, V> *rootRef = this->getRoot();

            while (rootRef != nullptr) {

                auto rootKey = *rootRef->getKeyVal();

                if (rootKey == key) {
                    return rootRef;
                } else if (key < rootKey) {
                    rootRef = rootRef->getLeftChild();
                } else {
                    rootRef = rootRef->getRightChild();
                }

            }

        }

        return nullptr;
    }

public:

    unsigned int size() override {
        return this->treeSize;
    }

    std::optional<std::shared_ptr<V>> get(const T &key) override {

        auto result = this->getNodeBy(key);

        if (result != nullptr) {
            return (result)->getValue();
        }

        return std::nullopt;
    }

    bool hasKey(const T &key) override {
        auto result = this->getNodeBy(key);

        if (result != nullptr) {
            return true;
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

    std::optional<node_info<T, V>> popLargest() override {

        if (this->rightMostNode != nullptr) {
            return std::get<0>(popLargestNode());
        }

        return std::nullopt;
    }

    std::optional<node_info<T, V>> popSmallest() override {

        if (this->leftMostNode != nullptr) {
            return std::get<0>(popSmallestNode());
        }

        return std::nullopt;
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        auto result = removeNode(key);

        if (result) {

            node_info<T, V> keyValue;

            std::tie(keyValue, std::ignore, std::ignore) = *result;

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

    TreeNode<T, V> *getRoot() {
        return this->rootNode.get();
    }
};

#endif //TRABALHO1_BINARYTREES_H

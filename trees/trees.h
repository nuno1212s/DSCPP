#ifndef TRABALHO1_TREES_H
#define TRABALHO1_TREES_H

#include <tuple>
#include <memory>
#include <vector>
#include <stack>
#include "../datastructures.h"

template<class T>
class TreeSet : public Set<T> {

public:
    virtual std::optional<std::shared_ptr<T>> peekSmallest() = 0;

    virtual std::optional<std::shared_ptr<T>> peekLargest() = 0;

    virtual std::optional<std::shared_ptr<T>> popSmallest() = 0;

    virtual std::optional<std::shared_ptr<T>> popLargest() = 0;

    virtual std::vector<std::shared_ptr<T>> rangeSearch(const T &base, const T &max) = 0;

};

template<typename T, typename V>
class TreeMap : public Map<T, V> {

public:
    virtual std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) = 0;

    virtual std::optional<node_info<T, V>> peekSmallest() = 0;

    virtual std::optional<node_info<T, V>> peekLargest() = 0;

    virtual std::optional<node_info<T, V>> popSmallest() = 0;

    virtual std::optional<node_info<T, V>> popLargest() = 0;

};

template<typename T, typename V>
class TreeNode {

protected:

    std::shared_ptr<T> key;
    std::shared_ptr<V> value;

    TreeNode<T, V> *parent;

    std::unique_ptr<TreeNode<T, V>> leftNode, rightNode;

    void setParent(TreeNode<T, V> *parent) {
        this->parent = parent;
    }

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
        TreeNode<T, V> *previous = root;

        while (current->getRightChild() != nullptr) {
            previous = current;
            current = current->getRightChild();
        }

        return previous;
    }

    TreeNode<T, V> *getLeftMostNodeInTree(TreeNode<T, V> *root) {

        auto current = root;

        TreeNode<T, V> *previous = root;

        while (current->getLeftChild() != nullptr) {
            previous = current;
            current = current->getLeftChild();
        }

        return previous;
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

        std::cout << "Test" << std::endl;

        TreeNode<T, V> *root = this->getRoot();

        while (root != nullptr) {

            if (*(root->getKey()) == key) {

                this->treeSize--;

                node_info<T, V> nodeInfo = std::make_tuple(root->getKey(), root->getValue());

                std::cout << "Test 2" << std::endl;
                TreeNode<T, V> *toReplace = nullptr;

                if (root->getRightChild() != nullptr) {
                    toReplace = getLeftMostNodeInTree(root->getRightChild());

                    std::cout << "1" << std::endl;
                } else if (root->getLeftChild() != nullptr) {
                    toReplace = getRightMostNodeInTree(root->getLeftChild());
                    std::cout << "2" << std::endl;
                }

                if (toReplace != nullptr && toReplace != root) {
                    //toReplace is the left most node of the right sub tree or the right most node of the left subtree

                    //Set the root as the toReplace values
                    root->setKey(toReplace->getKey());
                    root->setValue(toReplace->getValue());

                    TreeNode<T, V> *toReplaceParent = toReplace->getParent();

                    std::unique_ptr<TreeNode<T, V>> childToSet(nullptr), removed(nullptr);

                    if (toReplaceParent->getLeftChild() == toReplace) {
                        removed = toReplaceParent->getLeftNodeOwnership();
                    } else {
                        removed = toReplaceParent->getRightNodeOwnership();
                    }

                    //There can only be either a right or a left child never both,
                    // since this is the left most or the right most node
                    if (toReplace->getLeftChild() != nullptr) {
                        childToSet = toReplace->getLeftNodeOwnership();
                    } else if (toReplace->getRightChild() != nullptr) {
                        childToSet = toReplace->getRightNodeOwnership();
                    }

                    if (childToSet.get() != nullptr) {
                        if (toReplaceParent->getLeftChild() == toReplace) {
                            toReplaceParent->setLeftChild(std::move(childToSet));
                        } else {
                            toReplaceParent->setRightChild(std::move(childToSet));
                        }
                    }

                    return std::make_tuple(nodeInfo, std::move(removed));

                } else {
                    //This node is a leaf
                    if (root->getParent() != nullptr) {

                        auto parentNode = root->getParent();

                        if (parentNode->getLeftChild() == root) {

                            auto leftChild = parentNode->getLeftNodeOwnership();

                            return std::make_tuple(nodeInfo, std::move(leftChild));

                        } else {
                            auto rightChild = parentNode->getRightNodeOwnership();

                            return std::make_tuple(nodeInfo, std::move(rightChild));
                        }

                    } else {
                        return std::make_tuple(nodeInfo, std::move(this->getRootNodeOwnership()));
                    }

                }

            } else if (key < (*root->getKey())) {
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


#endif //TRABALHO1_TREES_H

#ifndef TRABALHO1_TREES_H
#define TRABALHO1_TREES_H

#include <tuple>
#include <memory>
#include <vector>

template<typename T, typename V>
class TreeNode {

protected:

    std::shared_ptr<T> key;
    std::shared_ptr<V> value;

    TreeNode<T, V> *parent;

    std::unique_ptr<TreeNode<T, V>> leftNode, rightNode;

    TreeNode(T &key, V &value, TreeNode<T, V> *parent);

protected:

    void setParent(TreeNode<T, V> *parent);

public:
    std::shared_ptr<T> getKey() const {
        return key;
    }

    std::shared_ptr<V> getValue() const {
        return value;
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
        return leftNode;
    }

    std::unique_ptr<TreeNode<T, V>> getRightNodeOwnership() {
        return rightNode;
    }

    void setLeftChild(std::unique_ptr<TreeNode<T, V>> node);

    void setRightChild(std::unique_ptr<TreeNode<T, V>> node);
};

template<class T>
class TreeSet {

public:
    virtual void add(const T &key) = 0;

    virtual bool contains(const T &key) = 0;

    virtual std::shared_ptr<T> find(const T &key) = 0;

    virtual std::shared_ptr<T> remove(const T &key) = 0;

    virtual std::shared_ptr<T> peekSmallest() = 0;

    virtual std::shared_ptr<T> peekLargest() = 0;

    virtual std::shared_ptr<T> popSmallest() = 0;

    virtual std::shared_ptr<T> popLargest() = 0;

    virtual std::vector<std::shared_ptr<T>> rangeSearch(const T &base, const T &max) = 0;

    virtual int size() = 0;
};

template<typename T, typename V>
using node_info = std::tuple<std::shared_ptr<T>, std::shared_ptr<V>>;

template<typename T, typename V>
class TreeMap {

public:
    virtual void add(const T &key, const V &value) = 0;

    virtual bool hasKey(const T &key) = 0;

    virtual std::shared_ptr<V> get(const T &key) = 0;

    virtual std::shared_ptr<V> remove(const T &key) = 0;

    virtual std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) = 0;

    virtual node_info<T, V> peekSmallest() = 0;

    virtual node_info<T, V> peekLargest() = 0;

    virtual node_info<T, V> popSmallest() = 0;

    virtual node_info<T, V> popLargest() = 0;

    virtual int size() = 0;

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

    TreeNode<T, V> *getRoot() {
        return rootNode.get();
    }

    std::unique_ptr<TreeNode<T, V>> getRootNodeOwnership() {
        return rootNode;
    }

    void setRootNode(std::unique_ptr<TreeNode<T, V>> rootNode);

public:

    int size() override;

    std::shared_ptr<V> get(const T &key) override;

    void add(const T &key, const V &value) override;

    bool hasKey(const T &key) override;

    node_info<T, V> peekSmallest() override;

    node_info<T, V> peekLargest() override;

    std::shared_ptr<V> remove(const T &key) override;

    /**
     * Range search, returns elements in-order by using the in order traversal
     * @param base The base value
     * @param max The max value
     * @return
     */
    std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) override;
};


#endif //TRABALHO1_TREES_H

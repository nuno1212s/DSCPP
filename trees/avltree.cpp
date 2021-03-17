#include "avltree.h"

template<typename T, typename V>
std::unique_ptr<AVLNode<T, V>> rotateLeft(std::unique_ptr<AVLNode<T, V>> root) {

    auto right = root->getRightNodeOwnership();

    AVLNode<T, V> *rootRef = root.get(), *rightRef = right.get();

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

template<typename T, typename V>
std::unique_ptr<AVLNode<T, V>> rotateRight(std::unique_ptr<AVLNode<T, V>> root) {

    auto left = root->getLeftNodeOwnership();

    AVLNode<T, V> *rootRef = root.get(), *leftRef = left.get();

    if (leftRef->getRightChild() != nullptr) {
        rootRef->setRightChild(std::move(leftRef->getRightNodeOwnership()));
    }

    leftRef->setRightChild(std::move(root));

    return left;
}

template<typename T, typename V>
AVLNode<T, V>::AVLNode(T &key, V &value, AVLNode<T, V> *parent) : TreeNode<T, V>(key, value),
                                                                  parent(parent) {

    if (this->parent != nullptr) {
        this->balance = this->parent->getBalance() + 1;
    } else {
        this->balance = 0;
    }

}

template<typename T, typename V>
void AVLNode<T, V>::setBalance(int balance) {
    this->balance = balance;
}

template<typename T, typename V>
AvlTree<T, V>::AvlTree() : BinarySearchTree<T, V>() {}

template<typename T, typename V>
std::tuple<std::shared_ptr<T>, std::shared_ptr<V>> removeLocal(std::shared_ptr<AVLNode<T, V>> root, const T &key) {

    if (*(root->getKey()) == (*key)) {
        auto keyAndValue = std::make_tuple(root->getKey(), root->getValue());

        if (root->getLeftChild().get() != nullptr) {
            auto result = getRightMostNodeInTree(root->getLeftChild());


        } else if (root->getRightChild().get() != nullptr) {

        }
    }

}

template<typename T, typename V>
std::shared_ptr<T> AvlTree<T, V>::remove(const T &key) {


    return std::shared_ptr<T>();
}

template<typename T, typename V>
std::tuple<std::shared_ptr<T>, std::shared_ptr<V>> AvlTree<T, V>::popSmallest() {
    return std::tuple<std::shared_ptr<T>, std::shared_ptr<V>>();
}

template<typename T, typename V>
std::tuple<std::shared_ptr<T>, std::shared_ptr<V>> AvlTree<T, V>::popLargest() {
    return std::tuple<std::shared_ptr<T>, std::shared_ptr<V>>();
}
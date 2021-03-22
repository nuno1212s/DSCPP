#include <iostream>
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

    //TODO: Update balance

    return left;
}
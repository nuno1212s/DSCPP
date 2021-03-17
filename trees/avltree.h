#ifndef TRABALHO1_AVLTREE_H
#define TRABALHO1_AVLTREE_H

#include <vector>
#include "trees.h"

template<typename T, typename V>
class AVLNode : public TreeNode<T, V> {

    AVLNode<T, V> *parent;

    int balance;

private:
    AVLNode(T &key, V &value, AVLNode<T, V> *parent);

public:

    int getBalance() {
        return balance;
    }

    void setBalance(int balance);
};

template<typename T, typename V>
class AvlTree : public BinarySearchTree<T, V> {

public:
    AvlTree();

    std::shared_ptr<T> remove(const T &key) override;

    std::tuple<std::shared_ptr<T>, std::shared_ptr<V>> popSmallest() override;

    std::tuple<std::shared_ptr<T>, std::shared_ptr<V>> popLargest() override;

};


#endif //TRABALHO1_AVLTREE_H

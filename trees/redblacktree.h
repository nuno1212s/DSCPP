#ifndef TRABALHO1_REDBLACKTREE_H
#define TRABALHO1_REDBLACKTREE_H

#include "binarytrees.h"

enum NodeColor {
    RED,
    BLACK
};

template<typename T, typename V>
class RedBlackNode : public TreeNode<T, V> {

protected:
    NodeColor color;

};

template<typename T, typename V>
class RedBlackTree : public BinarySearchTree<T, V> {



};

#endif //TRABALHO1_REDBLACKTREE_H

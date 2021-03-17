#include "trees.h"

template<typename T, typename V>
TreeNode<T, V>::TreeNode(T &key, V &value, TreeNode<T, V> *parent): parent(parent) {

    this->key = std::make_shared<>(new T(key));
    this->value = std::make_shared<>(new V(value));

}

template<typename T, typename V>
void TreeNode<T, V>::setLeftChild(std::unique_ptr<TreeNode<T, V>> node) {
    this->leftNode = std::move(node);

    this->leftNode->setParent(this);
}

template<typename T, typename V>
void TreeNode<T, V>::setRightChild(std::unique_ptr<TreeNode<T, V>> node) {
    this->rightNode = std::move(node);

    this->rightNode->setParent(this);
}

template<typename T, typename V>
void TreeNode<T, V>::setParent(TreeNode<T, V> *parent) {
    this->parent = parent;
}

template<typename T, typename V>
void BinarySearchTree<T, V>::setRootNode(std::unique_ptr<TreeNode<T, V>> rootNode) {
    this->rootNode = std::move(rootNode);
}

template<typename T, typename V>
int BinarySearchTree<T, V>::size() {
    return this->treeSize;
}

template<typename T, typename V>
static TreeNode<T, V> *getRightMostNodeInTree(TreeNode<T, V> *root) {

    auto current = root;

    while (current->getRightChild().get() != nullptr) {
        current = current->getRightChild();
    }

    return current;
}

template<typename T, typename V>
static TreeNode<T, V> *getLeftMostNodeInTree(TreeNode<T, V> *root) {

    auto current = root;

    while (current->getLeftChild() != nullptr) {
        current = current->getLeftChild();
    }

    return current;
}

template<typename T, typename V>
void BinarySearchTree<T, V>::add(const T &key, const V &value) {

    if (this->getRoot() == nullptr) {



    }

}

template<typename T, typename V>
std::shared_ptr<V> BinarySearchTree<T, V>::remove(const T &key) {

    if (this->getRoot() == nullptr) {
        return nullptr;
    }

    TreeNode<T, V> *root = this->getRoot();

    while (root != nullptr) {

        if ((*root->getKey()) == key) {

            if (root->getRightChild() != nullptr) {

                auto smallestInRight = getLeftMostNodeInTree(root->getRightChild());


            } else if (root->getLeftChild() != nullptr) {

                auto largestInLeft = getRightMostNodeInTree(root->getLeftChild());

            }

            break;

        } else if (key < (*root->getKey())) {
            root = root->getLeftChild();
        } else {
            root = root->getRightChild();
        }

    }

}

template<typename T, typename V>
std::shared_ptr<V> BinarySearchTree<T, V>::get(const T &key) {

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

    return nullptr;
}

template<typename T, typename V>
bool BinarySearchTree<T, V>::hasKey(const T &key) {

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

template<typename T, typename V>
node_info<T, V> BinarySearchTree<T, V>::peekSmallest() {

    if (this->leftMostNode != nullptr) {
        return std::make_tuple(this->leftMostNode->getKey(), this->leftMostNode->getValue());
    }

}

template<typename T, typename V>
node_info<T, V> BinarySearchTree<T, V>::peekLargest() {

    if (this->rightMostNode != nullptr) {
        return std::make_tuple(this->rightMostNode->getKey(), this->rightMostNode->getValue());
    }

}

template<typename T, typename V>
void searchInTree(TreeNode<T, V> *root, const T &base, const T &max, std::vector<node_info<T, V>> *result) {

    if (root == nullptr) return;

    if (root->getKey() >= base && root->getKey() <= max) {

        //Search first in the left sub tree, so that the smaller values come out first in the vector
        searchInTree(root->getLeftChild(), base, max, result);

        result->push_back(std::make_tuple(root->getKey(), root->getValue()));

        //After inserting our value (larger than the left sub tree and smaller than the right subtree) search the right sub tree,so
        //that in the end, the elements get added in order
        searchInTree(root->getRightChild(), base, max, result);
    }

}

template<typename T, typename V>
std::unique_ptr<std::vector<node_info<T, V>>> BinarySearchTree<T, V>::rangeSearch(const T &base, const T &max) {

    auto vector = std::make_unique<>(new std::vector<node_info<T, V>>());

    searchInTree(this->rootNode.get(), base, max, vector.get());

    return vector;
}

#ifndef TRABALHO1_SPLAYTREE_H
#define TRABALHO1_SPLAYTREE_H

#include "binarytrees.h"

template<typename T, typename V>
class SplayNode : public TreeNode<T, V> {

public:
    SplayNode(std::shared_ptr<T> key, std::shared_ptr<V> value, SplayNode<T, V> *parent)
            : TreeNode<T, V>(std::move(key),
                             std::move(value),
                             parent) {}

    ~SplayNode() override {

    }
};

template<typename T, typename V>
class SplayTree : public BinarySearchTree<T, V> {

protected:

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

    void zigZig(TreeNode<T, V> *root) {
        rotateRightP(root->getParent()->getParent());
        rotateRightP(root->getParent());
    }

    void zagZag(TreeNode<T, V> *root) {
        rotateLeftP(root->getParent()->getParent());
        rotateLeftP(root->getParent());
    }

    void zigZag(TreeNode<T, V> *root) {
        rotateRightP(root->getParent());

        rotateLeftP(root->getParent());
    }

    void zagZig(TreeNode<T, V> *root) {
        rotateLeftP(root->getParent());

        rotateRightP(root->getParent());
    }

    void splay(TreeNode<T, V> *x) {

        if (x == nullptr) return;

        auto parent = x->getParent();

        while (parent != nullptr) {

            auto grandParent = parent->getParent();

            if (grandParent != nullptr) {
                if (grandParent->getLeftChild() == parent) {
                    if (parent->getLeftChild() == x) {
                        //Left left case, apply zig zig
                        zigZig(x);
                    } else {
                        //Left Right case
                        zagZig(x);
                    }
                } else {
                    if (parent->getRightChild() == x) {
                        //Right right case, apply zag zag
                        zagZag(x);
                    } else {
                        //Right Left case
                        zigZag(x);
                    }
                }
            } else {
                //We are only one step away from becoming the root
                if (parent->getLeftChild() == x) {
                    //Zig rotation
                    rotateRightP(x->getParent());
                } else {
                    //Zag rotation
                    rotateLeftP(x->getParent());
                }

                break;
            }

            parent = x->getParent();
        }
    }

    std::tuple<TreeNode<T, V> *, TreeNode<T, V> *> getNodeAndPreviousByKey(const T &key) {

        if (this->getRoot() != nullptr) {

            TreeNode<T, V> *rootRef = this->getRoot(), *previous = nullptr;

            while (rootRef != nullptr) {


                auto rootKey = *rootRef->getKeyVal();

                if (rootKey == key) {
                    return std::make_tuple(rootRef, previous);
                } else if (key < rootKey) {
                    previous = rootRef;
                    rootRef = rootRef->getLeftChild();
                } else {
                    previous = rootRef;
                    rootRef = rootRef->getRightChild();
                }
            }

            return std::make_tuple(rootRef, previous);
        } else {
            return std::make_tuple(nullptr, nullptr);
        }
    }

protected:
    std::unique_ptr<TreeNode<T, V>>
    initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent) override {
        auto node = std::make_unique<SplayNode<T, V>>(std::move(key), std::move(value),
                                                    (SplayNode<T, V> *) parent);

        return std::move(node);
    }

public:

    SplayTree() : BinarySearchTree<T, V>() {}

    ~SplayTree() override {

        auto nodes = std::make_unique<std::vector<std::unique_ptr<SplayNode<T, V>>>>(this->size());



    }

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        auto *addedNode = this->addNode(std::move(key), std::move(value));

        splay(addedNode);

    }

    bool hasKey(const T &key) override {

        if (this->getRoot() == nullptr) {
            return false;
        }

        auto node = this->getNodeAndPreviousByKey(key);

        TreeNode<T, V> *root, *previous;

        std::tie(root, previous) = node;

        if (root != nullptr) {

            splay(root);

            return true;
        } else {
            splay(previous);

            return false;
        }
    }

    std::optional<std::shared_ptr<V>> get(const T &key) override {
        if (this->getRoot() == nullptr) {
            return std::nullopt;
        }

        auto node = this->getNodeAndPreviousByKey(key);

        TreeNode<T, V> *root, *previous;

        std::tie(root, previous) = node;

        if (root != nullptr) {

            splay(root);

            return root->getValue();
        } else {
            splay(previous);

            return std::nullopt;
        }
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        if (this->getRoot() == nullptr) {
            return std::nullopt;
        }

        auto node = this->getNodeAndPreviousByKey(key);

        TreeNode<T, V> *foundNode, *previous;

        std::tie(foundNode, previous) = node;

        if (foundNode != nullptr) {
            splay(foundNode);
        } else {
            splay(previous);
        }

        TreeNode<T, V> *root = this->getRoot();

        if ((*root->getKeyVal()) == key) {

            std::unique_ptr<TreeNode<T, V>> rootOwner = this->getRootNodeOwnership(),
                    leftNodeOwner = rootOwner.get()->getLeftNodeOwnership(),
                    rightNodeOwner = rootOwner.get()->getRightNodeOwnership();

            if (leftNodeOwner.get() == nullptr) {

                this->setRootNode(std::move(rightNodeOwner));

            } else {

                auto *leftNodeP = leftNodeOwner.get();

                //Set the left sub tree as the new root-
                this->setRootNode(std::move(leftNodeOwner));

                auto largestNodeInRoot1 = this->getRightMostNodeInTree(leftNodeP);

                //Splay the largest node
                splay(largestNodeInRoot1);

                //The right subtree is now the right child of the
                largestNodeInRoot1->setRightChild(std::move(rightNodeOwner));
            }

            this->treeSize--;

            return rootOwner.get()->getKey();

        } else {
            return std::nullopt;
        }
    }

    std::optional<node_info<T, V>> popLargest() override {

        if (this->rightMostNode != nullptr) {

            auto rightNode = this->rightMostNode;

            this->handleRemoveLargestNode();

            splay(rightNode);

            std::unique_ptr<TreeNode<T, V>> rootOwner = this->getRootNodeOwnership(),
                    leftNodeOwner = rootOwner->getLeftNodeOwnership(),
                    rightNodeOwner = rootOwner->getRightNodeOwnership();

            if (leftNodeOwner.get() == nullptr) {

                this->setRootNode(std::move(rightNodeOwner));

            } else {

                auto *leftNodeP = leftNodeOwner.get();

                //Set the left sub tree as the new root-
                this->setRootNode(std::move(leftNodeOwner));

                auto largestNodeInRoot1 = this->getRightMostNodeInTree(leftNodeP);

                //Splay the largest node
                splay(largestNodeInRoot1);

                //The right subtree is now the right child of the
                largestNodeInRoot1->setRightChild(std::move(rightNodeOwner));
            }

            this->treeSize--;

            return std::make_tuple(rootOwner->getKey(), rootOwner->getValue());
        }

        return std::nullopt;
    }

    std::optional<node_info<T, V>> popSmallest() override {

        if (this->leftMostNode != nullptr) {
            auto leftNode = this->leftMostNode;

            this->handleRemoveSmallestNode();

            splay(leftNode);

            std::unique_ptr<TreeNode<T, V>> rootOwner = this->getRootNodeOwnership(),
                    leftNodeOwner = rootOwner->getLeftNodeOwnership(),
                    rightNodeOwner = rootOwner->getRightNodeOwnership();

            if (leftNodeOwner.get() == nullptr) {

                this->setRootNode(std::move(rightNodeOwner));

            } else {

                auto *leftNodeP = leftNodeOwner.get();
                //Set the left sub tree as the new root-
                this->setRootNode(std::move(leftNodeOwner));

                auto largestNodeInRoot1 = this->getRightMostNodeInTree(leftNodeP);

                //Splay the largest node
                splay(largestNodeInRoot1);

                //The right subtree is now the right child of the
                largestNodeInRoot1->setRightChild(std::move(rightNodeOwner));
            }

            this->treeSize--;

            return std::make_tuple(rootOwner->getKey(), rootOwner->getValue());
        }

        return std::nullopt;
    }


};

#endif //TRABALHO1_SPLAYTREE_H

#ifndef TRABALHO1_REDBLACKTREE_H
#define TRABALHO1_REDBLACKTREE_H

#include "binarytrees.h"

enum NodeColor {
    RED,
    BLACK
};

template<typename T, typename V>
class RBNode : public TreeNode<T, V> {

protected:
    NodeColor color;

public:
    RBNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent, NodeColor color) :
            TreeNode<T, V>(std::move(key), std::move(value), parent), color(color) {}

    NodeColor getColor() const {
        return color;
    }

    void setColor(NodeColor color) {
        std::cout << "Set color of " << *this->getKeyVal() << " to " << (color == RED ? "RED" : "BLACK") << std::endl;

        this->color = color;
    }

};


template<typename T, typename V>
static NodeColor getNodeColor(RBNode<T, V> *node) {
    if (node == nullptr) return BLACK;

    return node->getColor();
}

template<typename T, typename V>
static bool hasRedChild(RBNode<T, V> *node) {
    if (node == nullptr) return false;

    return getNodeColor((RBNode<T, V>*) node->getRightChild()) == RED || getNodeColor((RBNode<T, V>*)node->getLeftChild()) == RED;
}

template<typename T, typename V>
class RedBlackTree : public BinarySearchTree<T, V> {

protected:
    std::unique_ptr<TreeNode<T, V>>
    initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value, TreeNode<T, V> *parent) override {

        //A Node always starts as a RED node
        auto node = std::make_unique<RBNode<T, V>>(key, value, parent, NodeColor::RED);

        return std::move(node);
    }

    void rotateRightP(RBNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<RBNode<T, V>> rootOwner(
                    static_cast<RBNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(this->rotateRight(std::move(rootOwner)));

        } else {
            if (parent->getLeftChild() == root) {
                std::unique_ptr<RBNode<T, V>> leftOwner(
                        static_cast<RBNode<T, V> *>(parent->getLeftNodeOwnership().release()));

                parent->setLeftChild(this->rotateRight(std::move(leftOwner)));
            } else {
                std::unique_ptr<RBNode<T, V>> rightOwner(
                        static_cast<RBNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(this->rotateRight(std::move(rightOwner)));
            }
        }
    }

    void rotateLeftP(RBNode<T, V> *root) {

        auto parent = root->getParent();

        if (parent == nullptr) {
            std::unique_ptr<AVLNode<T, V>> rootOwner(
                    static_cast<AVLNode<T, V> *>(this->getRootNodeOwnership().release()));

            this->setRootNode(this->rotateLeft(std::move(rootOwner)));
        } else {

            if (parent->getLeftChild() == root) {
                std::unique_ptr<AVLNode<T, V>> leftOwner(
                        static_cast<AVLNode<T, V> *>(parent->getLeftNodeOwnership().release()));
                parent->setLeftChild(this->rotateLeft(std::move(leftOwner)));
            } else {
                std::unique_ptr<AVLNode<T, V>> rightOwner(
                        static_cast<AVLNode<T, V> *>(parent->getRightNodeOwnership().release()));

                parent->setRightChild(this->rotateLeft(std::move(rightOwner)));
            }
        }
    }

    void handleLeftCase(RBNode<T, V> *node) {

        auto parent = (RBNode<T, V> *) node->getParent();

        //We will always have a grand parent because if the tree height is less than 2,
        //Then we only have the root (black), one red level and the NULL leaves
        auto grandParent = (RBNode<T, V> *) parent->getParent();

        auto uncle = (RBNode<T, V> *) grandParent->getRightChild();

        NodeColor uncleColour = getNodeColor(uncle);

        if (uncleColour == RED) {
            parent->setColor(BLACK);

            uncle->setColor(BLACK);

            grandParent->setColor(RED);

            updateBalance(grandParent);
        } else {
            if (parent->getLeftChild() == node) {

                //Left Left case

                //Rotate right around grand parent and then switch the parent and grandparent color
                rotateRightP(grandParent);

                auto colorTemp = parent->getColor();

                parent->setColor(grandParent->getColor());
                grandParent->setColor(colorTemp);
            } else {
                //Left Right case

                rotateLeftP(parent);
                //We are now in the parent position, and we want to apply the left left case
                rotateRightP(grandParent);
                //Since we are now in the parent position, we will exchange colors with the grand parent node
                auto colorTemp = node->getColor();
                node->setColor(grandParent->getColor());
                grandParent->setColor(colorTemp);
            }
        }
    }

    void handleRightCase(RBNode<T, V> *node) {
        auto parent = (RBNode<T, V> *) node->getParent();

        //We will always have a grand parent because if the tree height is less than 2,
        //Then we only have the root (black), one red level and the NULL leaves
        auto grandParent = (RBNode<T, V> *) parent->getParent();

        auto uncle = (RBNode<T, V> *) grandParent->getLeftChild();

        NodeColor uncleColour = getNodeColor(uncle);

        if (uncleColour == RED) {
            parent->setColor(BLACK);

            uncle->setColor(BLACK);

            grandParent->setColor(RED);

            updateBalance(grandParent);
        } else {
            if (parent->getRightChild() == node) {

                //Right Right case

                //Rotate left around grand parent and then switch the parent and grandparent color
                rotateLeftP(grandParent);

                auto colorTemp = parent->getColor();

                parent->setColor(grandParent->getColor());
                grandParent->setColor(colorTemp);
            } else {
                //Right Left case

                rotateRightP(parent);
                //We are now in the parent position, and we want to apply the right right case
                rotateLeftP(grandParent);
                //Since we are now in the parent position, we will exchange colors with the grand parent node
                auto colorTemp = node->getColor();

                node->setColor(grandParent->getColor());
                grandParent->setColor(colorTemp);
            }
        }
    }

    void updateBalance(RBNode<T, V> *node) {

        auto parent = (RBNode<T, V> *) node->getParent();

        if (parent == nullptr || this->getRoot() == node) {
            //The root is always BLACK
            node->setColor(NodeColor::BLACK);
            return;
        }

        if (getNodeColor(parent) != BLACK && node != this->getRoot()) {

            auto grandParent = (RBNode<T, V>*) parent->getParent();

            RBNode<T, V> *uncle;

            if (grandParent->getLeftChild() == parent) {
                uncle = (RBNode<T, V>*) grandParent->getRightChild();
            } else {
                uncle = (RBNode<T, V>*) grandParent->getLeftChild();
            }

            if (getNodeColor(uncle) == RED) {

                parent->setColor(BLACK);
                uncle->setColor(BLACK);
                grandParent->setColor(RED);

                updateBalance(grandParent);
            } else {
                if (grandParent->getLeftChild() == parent) {
                    //Parent is on the left of grand parent
                    handleLeftCase(node);
                } else {
                    //Parent is on the right of grand parent
                    handleRightCase(node);
                }
            }
        }
    }

    void fixDoubleBlack(RBNode<T, V> *node) {

        if (node == this->getRoot()) return;

        RBNode<T, V> *parent = (RBNode<T, V> *) node->getParent();

        RBNode<T, V> *sibling;

        bool siblingOnLeft = false;

        if (parent->getLeftChild() == nullptr && parent->getRightChild() == nullptr) {
            fixDoubleBlack(parent);
            return;
        } else if (parent->getLeftChild() == nullptr || parent->getLeftChild() == node) {
            sibling = (RBNode<T, V> *) parent->getRightChild();
        } else {
            sibling = (RBNode<T, V> *) parent->getLeftChild();
            siblingOnLeft = true;
        }

        if (sibling == nullptr) {
            std::cout << "Sibling is null"<< std::endl;
            fixDoubleBlack(parent);
        } else {
            if (sibling->getColor() == RED) {

                std::cout << "Sibling is RED"<< std::endl;

                parent->setColor(RED);
                sibling->setColor(BLACK);

                if (siblingOnLeft) {
                    rotateRightP(parent);
                } else {
                    rotateLeftP(parent);
                }

                fixDoubleBlack(node);
            } else {
                std::cout << "Sibling is BLACK "<< std::endl;
                //Sibling is BLACK
                if (hasRedChild(sibling)) {

                    std::cout << "Sibling has RED child "<< std::endl;
                    auto *siblingLeftChild = (RBNode<T, V> *) sibling->getLeftChild();

                    auto *siblingRightChild = (RBNode<T, V>*) sibling->getRightChild();

                    if (siblingRightChild != nullptr && siblingRightChild->getColor() == RED) {

                        if (siblingOnLeft) {

                            siblingRightChild->setColor(parent->getColor());

                            rotateLeftP(sibling);
                            rotateRightP(parent);
                        } else {

                            siblingRightChild->setColor(sibling->getColor());
                            sibling->setColor(parent->getColor());
                            rotateLeftP(parent);
                        }
                    }else {
                        if (siblingOnLeft) {
                            //Left Left rotation
                            siblingLeftChild->setColor(sibling->getColor());
                            sibling->setColor(parent->getColor());

                            rotateRightP(parent);
                        } else {
                            //Right left
                            siblingLeftChild->setColor(parent->getColor());

                            rotateRightP(sibling);
                            rotateLeftP(parent);
                        }
                    }

                    parent->setColor(BLACK);
                } else {

                    std::cout << "Sibling does not have RED child"<< std::endl;
                    sibling->setColor(RED);

                    if (parent->getColor() == BLACK) {
                        fixDoubleBlack(parent);
                    } else {
                        parent->setColor(BLACK);
                    }
                }
            }
        }
    }

public:
    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        auto newNode = this->addNode(key, value);

        updateBalance((RBNode<T, V> *) newNode);
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        auto result = this->removeNode(key);

        std::cout << "Hello" << std::endl;
        if (result) {
            auto *replacement = (RBNode<T, V> *) std::get<2>(*result);

            std::unique_ptr<RBNode<T, V>> removed(static_cast<RBNode<T, V> *> (std::get<1>(*result).release()));

            if (getNodeColor(replacement) == RED || getNodeColor(removed.get()) == RED) {
                if (replacement != nullptr) {
                    replacement->setColor(BLACK);
                }
            } else {
                std::cout << "Double black" << std::endl;
                //u is double black
                if (replacement == nullptr) {
                    //If the node removed was a leaf, then the double black will be in a "null" leaf
                    //So, use the removed node as an entry point
                    fixDoubleBlack(removed.get());
                } else {
                    fixDoubleBlack(replacement);
                }
            }

            return std::get<1>(std::get<0>(*result));
        }

        std::cout << "Failed" << std::endl;

        return std::nullopt;
    }

    int getTreeHeight() {
        return 0;
    }

};

#endif //TRABALHO1_REDBLACKTREE_H

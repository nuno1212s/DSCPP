#ifndef TRABALHO1_SKIPLIST_H
#define TRABALHO1_SKIPLIST_H

#include <chrono>
#include <memory>
#include <random>
#include <vector>
#include "../datastructures.h"

//Since most cache lines are around 64 bytes long, we can keep an array of 8 64 bit pointers
//To improve cache performance
#define SKIP_LIST_HEIGHT_LIMIT 28
#define FIRST_BIT_MASK 0x1

template<typename T, typename V>
class SkipNode {

protected:
    std::shared_ptr<T> key;
    std::shared_ptr<V> value;

    std::vector<SkipNode<T, V> *> height_levels;

    std::unique_ptr<SkipNode<T, V>> nextNode;

    int level;

public:
    SkipNode(std::shared_ptr<T> key, std::shared_ptr<V> value, int level) : key(std::move(key)),
                                                                            value(std::move(value)),
                                                                            height_levels(SKIP_LIST_HEIGHT_LIMIT),
                                                                            level(level) {
    }

    ~SkipNode() {
        this->nextNode.reset();
    }

    std::shared_ptr<T> getKey() const {
        return key;
    }

    std::shared_ptr<V> getValue() const {
        return value;
    }

    void setValue(std::shared_ptr<V> value) {
        this->value = std::move(value);
    }

    const T *getKeyVal() {
        return key.get();
    }

    void setBaseNext(std::unique_ptr<SkipNode<T, V>> next) {
        this->nextNode = std::move(next);
        this->height_levels[0] = this->nextNode.get();
    }

    void setNext(int nodeHeight, SkipNode<T, V> *next) {
        this->height_levels[nodeHeight] = next;
    }

    std::unique_ptr<SkipNode<T, V>> getNextOwnership() {
        return std::move(this->nextNode);
    }

    int getLevel() {
        return level;
    }

    SkipNode<T, V> *getNextNode(int level) {
        return height_levels[level];
    }

};

template<typename T, typename V>
class SkipList : public OrderedMap<T, V> {

private:
    std::random_device randomEngine;
    std::mt19937 generator;
    std::uniform_int_distribution<unsigned int> distribution;

    std::unique_ptr<SkipNode<T, V>> rootNode;

    SkipNode<T, V> *lastNode;

    unsigned int listLevel;

    unsigned int listSize;

    unsigned long timeTakenFound, timeTakenInsert;

public:
    SkipList() : randomEngine(),
                 rootNode(initializeNodeRoot(SKIP_LIST_HEIGHT_LIMIT)),
                 listSize(0),
                 listLevel(0),
                 lastNode(nullptr),
                 timeTakenFound(0),
                 timeTakenInsert(0) {
        this->generator = std::mt19937(randomEngine());
    }

    SkipList(std::unique_ptr<SkipNode<T, V>> root) : randomEngine(),
                                                     rootNode(std::move(root)),
                                                     listSize(0),
                                                     listLevel(0),
                                                     lastNode(nullptr) {
        this->generator = std::mt19937(randomEngine());
    }

    ~SkipList() override {

        std::unique_ptr<SkipNode<T,V>> current = std::move(this->rootNode);

        while (current.get()->getNextNode(0) != nullptr) {
            //By reassigning the unique_ptr, the previous one gets deleted
            current = current.get()->getNextOwnership();
        }
    }

protected:
    virtual int getListLevel() const {
        return this->listLevel;
    }

    virtual void setListLevel(int listLevel) {
        this->listLevel = listLevel;
    }

    int generateLevel() {
        unsigned int random = distribution(generator);

        //Maybe get the amount of consecutive bits that have been set to 1 to get the correct level,
        //Instead of generating a new number every single time?

        int level = 0;

        while ((random & FIRST_BIT_MASK) != 0 && level < SKIP_LIST_HEIGHT_LIMIT) {
            level++;

            random = random >> 1;
        }

        // std::cout << "Generated level: " << level << std::endl;

        return level;
    }

    virtual std::unique_ptr<SkipNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value) {
        return std::make_unique<SkipNode<T, V>>(std::move(key), std::move(value), generateLevel());
    }

    virtual std::unique_ptr<SkipNode<T, V>> initializeNodeRoot(int level) {
        return std::make_unique<SkipNode<T, V>>(std::shared_ptr<T>(), std::shared_ptr<V>(), level);
    }

    SkipNode<T, V> *getRoot() {
        return this->rootNode.get();
    }

    void setRootNode(std::unique_ptr<SkipNode<T, V>> root) {
        this->rootNode = root;
    }

    SkipNode<T, V> *findNode(const T &key, SkipNode<T, V> **toUpdate) {
        SkipNode<T, V> *current = this->getRoot();

        //Start in the highest level
        for (int currentLevel = this->getListLevel(); currentLevel >= 0; currentLevel--) {

            SkipNode<T, V> *next = current->getNextNode(currentLevel);

            while (next != nullptr &&
                   *next->getKeyVal() < key) {
                //Find the largest key in the current level that is smaller than the key we're looking for
                current = next;
                next = next->getNextNode(currentLevel);
            }

            if (toUpdate != nullptr) {
                //When we are done going through a level, leave a reference to the last node we iterated in the level,
                //So that we can update it if the new node we inserted has a level >= to the currentLevel
                toUpdate[currentLevel] = current;
            }
        }

        //Since we find the largest node that's smaller than key, if the node that follows it is not the node we are looking
        //For, then that node does not exist in the list
        return current->getNextNode(0);
    }

    void traverseList(std::vector<node_info<T, V>> *destination) {

        SkipNode<T, V> *current = getRoot()->getNextNode(0);

        while (current != nullptr) {

            destination->emplace_back(std::make_tuple(current->getKey(), current->getValue()));

            current = current->getNextNode(0);
        }

    }

public:
    virtual void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {

        SkipNode<T, V> *update[SKIP_LIST_HEIGHT_LIMIT] = {nullptr};

        const T &keyRef = *key;

        SkipNode<T, V> *current = findNode(keyRef, update);

        if (current == nullptr || !(*current->getKeyVal() == keyRef)) {
            std::unique_ptr<SkipNode<T, V>> newNodeOwnership = initializeNode(std::move(key), std::move(value));

            SkipNode<T, V> *createdNode = newNodeOwnership.get();

            if (createdNode->getLevel() > getListLevel()) {

                for (int i = getListLevel() + 1; i <= createdNode->getLevel(); i++) {
                    update[i] = this->getRoot();
                }

                setListLevel(createdNode->getLevel());
            }

            for (int nodeLevel = 0; nodeLevel <= createdNode->getLevel(); nodeLevel++) {
                SkipNode<T, V> *lastChecked = update[nodeLevel];

                if (nodeLevel == 0) {
                    auto nextNodeOwnership = lastChecked->getNextOwnership();

                    createdNode->setBaseNext(std::move(nextNodeOwnership));

                    lastChecked->setBaseNext(std::move(newNodeOwnership));
                } else {
                    createdNode->setNext(nodeLevel, lastChecked->getNextNode(nodeLevel));

                    lastChecked->setNext(nodeLevel, createdNode);
                }
            }

            if (createdNode->getNextNode(0) == nullptr) {
                this->lastNode = createdNode;
            }

            this->listSize++;

        } else {
            current->setValue(std::move(value));
        }
    }

    virtual bool hasKey(const T &key) override {

        SkipNode<T, V> *node = findNode(key, nullptr);

        return node != nullptr && (*node->getKeyVal()) == key;
    }

    virtual std::optional<std::shared_ptr<V>> get(const T &key) override {
        SkipNode<T, V> *result = findNode(key, nullptr);

        if (result != nullptr && (*result->getKeyVal()) == key) {
            return result->getValue();
        }

        return std::nullopt;
    }

    unsigned int size() override {
        return this->listSize;
    }

    virtual std::optional<std::shared_ptr<V>> remove(const T &key) override {
        SkipNode<T, V> *update[SKIP_LIST_HEIGHT_LIMIT] = {nullptr};

        SkipNode<T, V> *current = findNode(key, update);

        if (current == nullptr || !(*current->getKeyVal() == key)) {
            return std::nullopt;
        } else {

            //We want to keep the reference alive because we still need to access the next nodes
            //When we are done moving the references is when we know this node can be disposed of
            std::unique_ptr<SkipNode<T, V>> nodeOwnership;

            for (int nodeLevel = 0; nodeLevel <= current->getLevel(); nodeLevel++) {

                SkipNode<T, V> *lastChecked = update[nodeLevel];

                if (nodeLevel == 0) {
                    nodeOwnership = lastChecked->getNextOwnership();

                    lastChecked->setBaseNext(std::move(nodeOwnership->getNextOwnership()));
                } else {
                    lastChecked->setNext(nodeLevel, current->getNextNode(nodeLevel));
                }
            }

            if (current == this->lastNode) {
                this->lastNode = update[0];
            }

            this->listSize--;

            while (getListLevel() > 0 && getRoot()->getNextNode(getListLevel()) == nullptr) {
                setListLevel(getListLevel() - 1);
            }

            return nodeOwnership.get()->getValue();
        }
    }

    std::optional<node_info<T, V>> peekLargest() override {

        if (this->lastNode == nullptr || this->lastNode == getRoot()) {
            return std::nullopt;
        } else {

            auto node = std::make_tuple(this->lastNode->getKey(), this->lastNode->getValue());

            return node;
        }
    }

    std::optional<node_info<T, V>> peekSmallest() override {

        auto *root = this->getRoot();

        if (root->getNextNode(0) == nullptr) {
            return std::nullopt;
        } else {

            auto *currentNode = root->getNextNode(0);

            auto nodeValue = std::make_tuple(currentNode->getKey(), currentNode->getValue());

            return nodeValue;
        }
    }

    std::unique_ptr<std::vector<std::shared_ptr<T>>> keys() override {

        auto keyVector = std::make_unique<std::vector<std::shared_ptr<T>>>(this->size());

        std::vector<node_info<T, V>> nodeCache(this->size());

        traverseList(&nodeCache);

        for (node_info<T, V> &start : nodeCache) {
            keyVector->emplace_back(std::move(std::get<0>(start)));
        }

        return keyVector;
    }

    std::unique_ptr<std::vector<std::shared_ptr<V>>> values() override {
        auto valueVector = std::make_unique<std::vector<std::shared_ptr<V>>>(this->size());

        std::vector<node_info<T, V>> nodeCache(this->size());

        traverseList(&nodeCache);

        for (node_info<T, V> &start : nodeCache) {
            valueVector->emplace_back(std::move(std::get<1>(start)));
        }

        return valueVector;
    }

    std::unique_ptr<std::vector<node_info<T, V>>> entries() override {
        auto valueVector = std::make_unique<std::vector<node_info<T, V>>>(this->size());

        traverseList(valueVector.get());

        return valueVector;
    }

    std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) override {

        SkipNode<T, V> *node = findNode(base, nullptr);

        std::unique_ptr<std::vector<node_info<T, V>>> result = std::make_unique<std::vector<node_info<T, V>>>();

        //Since we know that findNode returns a node such that node.getKey() >= base, we
        //Want to get all of the nodes until we reach the end of our search space
        while (node != nullptr && *node->getKeyVal() <= max) {
            result.get()->push_back(std::make_tuple(node->getKey(), node->getValue()));

            //Continue searching the list
            node = node->getNextNode(0);
        }

        return std::move(result);
    }

    std::optional<node_info<T, V>> popSmallest() override {
        auto *root = this->getRoot();

        if (root->getNextNode(0) == nullptr) {
            return std::nullopt;
        } else {
            auto *currentNode = root->getNextNode(0);

            std::shared_ptr<T> key = currentNode->getKey();

            auto returned = remove(*currentNode->getKeyVal());

            if (returned) {
                return std::make_tuple(key, *returned);
            } else {
                return std::nullopt;
            }
        }
    }

    std::optional<node_info<T, V>> popLargest() override {

        if (this->lastNode == nullptr || this->lastNode == getRoot()) {
            return std::nullopt;
        } else {
            auto *last = this->lastNode;

            std::shared_ptr<T> key = last->getKey();

            auto returned = remove(*last->getKeyVal());

            if (returned) {
                return std::make_tuple(key, *returned);
            } else {
                return std::nullopt;
            }
        }
    }
};

#endif //TRABALHO1_SKIPLIST_H

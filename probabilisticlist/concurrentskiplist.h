#ifndef TRABALHO1_CONCURRENTSKIPLIST_H
#define TRABALHO1_CONCURRENTSKIPLIST_H

#include <mutex>
#include "skiplist.h"
#include <atomic>

template<typename T, typename V>
class ConcurrentSkipNode : public SkipNode<T, V> {

private:
    //This is used to make the remove operations appear atomic
    bool marked;
    //This is the linearization point for the add operation
    //It is true when all of the pointers from the node level have been filled
    bool fullyLinked;
    std::mutex lock;

public:

    ConcurrentSkipNode(std::shared_ptr<T> key, std::shared_ptr<V> value, int level, bool fullyLinked) : SkipNode<T, V>(std::move(key),
                                                                                                     std::move(value),
                                                                                                     level),
                                                                                      marked(false),
            //We start with fully linked = false,
            //As any new node has not been added to
            //The list
                                                                                      fullyLinked(fullyLinked),
                                                                                      lock() {

        lock.lock();
        std::cout << "Locked" << std::endl;
        lock.unlock();
        std::cout << "Un Locked" << std::endl;
    }

    bool isMarked() const {
        return this->marked;
    }

    bool isFullyLinked() const {
        return this->fullyLinked;
    }

    std::mutex &getLock() {
        return this->lock;
    }

    void setFullyLinked(bool fullyLinked) {
        this->fullyLinked = fullyLinked;
    }

    void setMarked(bool marked) {
        this->marked = marked;
    }

};

template<typename T, typename V>
class ConcurrentSkipList : public SkipList<T, V> {

private:
    std::atomic<std::int32_t> treeSize;
    std::atomic<std::int32_t> treeLevel;

public:
    ConcurrentSkipList() : SkipList<T, V>(initializeNodeRoot(SKIP_LIST_HEIGHT_LIMIT)),
                           treeSize(0),
                           treeLevel(0){

    }

protected:

    std::unique_ptr<SkipNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value) override {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::move(key), std::move(value), this->generateLevel(), false);
    }

    std::unique_ptr<ConcurrentSkipNode<T, V>>
    initializeNodeWithLevel(std::shared_ptr<T> key, std::shared_ptr<V> value, int level) {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::move(key), std::move(value), level, false);
    }

    std::unique_ptr<SkipNode<T, V>> initializeNodeRoot(int level) override {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::shared_ptr<T>(), std::shared_ptr<V>(), level, true);
    }

    int concurrentFindNode(const T &key, ConcurrentSkipNode<T, V> **predecessors,
                           ConcurrentSkipNode<T, V> **successors) {

        //The first level where the key was found
        int levelFound = -1;

        auto *predecessor = (ConcurrentSkipNode<T, V> *) this->getRoot();

        for (int level = this->getListLevel(); level >= 0; level--) {

            auto *current = (ConcurrentSkipNode<T, V> *) predecessor->getNextNode(level);

            while (current != nullptr && key > *current->getKeyVal()) {
                predecessor = current;

                current = (ConcurrentSkipNode<T, V> *) current->getNextNode(level);
            }

            if (levelFound == -1 && (current != nullptr && key == *current->getKeyVal())) {
                levelFound = level;
            }

            if (predecessors != nullptr && successors != nullptr) {
                predecessors[level] = predecessor;
                successors[level] = current;
            }
        }

        return levelFound;
    }

private:

    bool canBeDeleted(ConcurrentSkipNode<T, V> *node, int levelFound) {
        return node->isFullyLinked() && !node->isMarked() && levelFound == node->getLevel();
    }

protected:
    int getListLevel() const override {
        return this->treeLevel.load();
    }

    void setListLevel(int listLevel) override {
        this->treeLevel.store(listLevel);
    }

public:

    int size() override {
        return this->treeSize.load();
    }

    bool hasKey(const T &key) override {

        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        int levelFound = concurrentFindNode(key, predecessors, successors);

        ConcurrentSkipNode<T, V> *node = successors[levelFound];

        return levelFound != -1 && node->isFullyLinked() && !node->isMarked();
    }

    std::optional<std::shared_ptr<V>> get(const T &key) override {

        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        int levelFound = concurrentFindNode(key, predecessors, successors);

        ConcurrentSkipNode<T, V> *node = successors[levelFound];

        if (levelFound != -1 && node->isFullyLinked() && !node->isMarked()) {
            return node->getValue();
        }

        return std::nullopt;
    }

    void add(std::shared_ptr<T> key, std::shared_ptr<V> value) override {
        int nodeLevel = this->generateLevel();

        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        while (true) {

            int levelFound = concurrentFindNode(*(key.get()), predecessors, successors);

            if (levelFound != -1) {
                //The already exists in the list

                auto *node = successors[levelFound];

                if (!node->isMarked()) {
                    //Node is not being removed

                    //Wait for the node to finish being added, if necessary
                    while (!node->isFullyLinked()) {}

                    node->setValue(std::move(value));

                    return;
                }

                //If the node was being removed, then we try again
                continue;
            }

            ConcurrentSkipNode<T, V> *predecessor = nullptr, *successor = nullptr,
            //The predecessor that was used before we change it to our new node
            *previousPredecessor = nullptr;

            bool isValid = true;

            //When this array gets disposed because we have left the scope, the
            //Locks will all be unlocked. (Sort of like a finally would do)
            //Since the scope of these locks is inside the while, they will get unlocked
            //Everytime we cycle the while, as is intended
            std::vector<std::unique_lock<std::mutex>> locks(SKIP_LIST_HEIGHT_LIMIT);

            //Lets acquire the locks of the nodes in every level until our new node level
            for (int level = 0; isValid && (level <= nodeLevel); level++) {

                predecessor = predecessors[level];
                successor = successors[level];

                if (predecessor != previousPredecessor) {
                    //We check this so we never lock the same node twice (Which would lead to a dead lock)

                    std::unique_lock<std::mutex> lock(predecessor->getLock());

                    locks.push_back(std::move(lock));

                    previousPredecessor = predecessor;
                }

                //After acquiring the lock, make sure no other thread made any changes to this part of the list
                //That would require a new search
                isValid = !predecessor->isMarked() && (successor == nullptr || !successor->isMarked()) &&
                          predecessor->getNextNode(level) == successor;
            }

            if (!isValid) {
                //Try again
                continue;
            }

            std::unique_ptr<ConcurrentSkipNode<T, V>> node = initializeNodeWithLevel(std::move(key), std::move(value),
                                                                                     nodeLevel);

            ConcurrentSkipNode<T, V> *nodeP = node.get();

            for (int level = 0; level <= nodeLevel; level++) {

                if (level == 0) {
                    auto successorControl = predecessors[level]->getNextOwnership();

                    nodeP->setBaseNext(std::move(successorControl));

                    predecessors[level]->setBaseNext(std::move(node));
                } else {
                    nodeP->setNext(level, successors[level]);

                    predecessors[level]->setNext(level, nodeP);
                }
            }

            this->treeSize++;

            while (true) {
                //Atomically update the height of the list
                int currentHeight = this->treeLevel.load();

                if (currentHeight < nodeLevel) {
                    if (this->treeLevel.compare_exchange_weak(currentHeight, nodeLevel)) break;
                } else {
                    break;
                }
            }

            nodeP->setFullyLinked(true);

            break;
        }
    }

    std::optional<std::shared_ptr<V>> remove(const T &key) override {

        ConcurrentSkipNode<T, V> *toDelete = nullptr;

        bool isMarked = false;

        int topLevel = -1;

        std::unique_ptr<std::lock_guard<std::mutex>> toDeleteLock;

        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        while (true) {
            int levelFound = concurrentFindNode(key, predecessors, successors);

            if (isMarked || (levelFound != -1 && canBeDeleted(successors[levelFound], levelFound))) {

                if (!isMarked) {
                    toDelete = successors[levelFound];

                    topLevel = levelFound;

                    toDeleteLock = std::make_unique<std::lock_guard<std::mutex>>(toDelete->getLock());

                    if (toDelete->isMarked()) {
                        return std::nullopt;
                    }

                    toDelete->setMarked(true);
                    isMarked = true;
                }

                ConcurrentSkipNode<T, V> *predecessor = nullptr, *successor = nullptr, *previousPredecessor = nullptr;

                std::vector<std::unique_lock<std::mutex>> locks(SKIP_LIST_HEIGHT_LIMIT);
                bool valid = true;

                for (int level = 0; valid && level <= toDelete->getLevel(); level++) {
                    predecessor = predecessors[level];
                    successor = successors[level];

                    if (predecessor != previousPredecessor) {
                        locks.emplace_back(predecessor->getLock());

                        previousPredecessor = predecessor;
                    }

                    valid = !predecessor->isMarked() && predecessor->getNextNode(level) == successor;
                }

                if (!valid) continue;

                std::unique_ptr<SkipNode<T, V>> toDeleteOwnership;

                for (int level = topLevel; level >= 0; level--) {

                    if (level == 0) {
                        std::unique_ptr<SkipNode<T, V>> nextNode = toDelete->getNextOwnership();

                        toDeleteOwnership = predecessors[level]->getNextOwnership();

                        predecessors[level]->setBaseNext(std::move(nextNode));
                    } else {
                        predecessors[level]->setNext(level, toDelete->getNextNode(level));
                    }
                }

                this->treeSize--;

                int tLevel;

                while ((tLevel = this->treeLevel.load()) > 0 && this->getRoot()->getNextNode(tLevel) == nullptr) {
                    this->treeLevel--;
                }

                return toDeleteOwnership.get()->getValue();
            } else {
                return std::nullopt;
            }
        }
    }

};

#endif //TRABALHO1_CONCURRENTSKIPLIST_H

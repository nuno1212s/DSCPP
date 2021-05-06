#ifndef TRABALHO1_CONCURRENTSKIPLIST_H
#define TRABALHO1_CONCURRENTSKIPLIST_H

#include <mutex>
#include "skiplist.h"
#include <atomic>

template<typename T, typename V>
class ConcurrentSkipNode : public SkipNode<T, V> {

private:
    //This is used to make the remove operations appear atomic (When the node is marked, it is in the process of being removed
    bool marked;
    //This is the linearization point for the add operation
    //It is true when all of the pointers from the node level have been filled
    bool fullyLinked;
    std::mutex lock;

public:

    //We start with fully linked = false,
    //As any new node has not been added to
    //The list
    ConcurrentSkipNode(std::shared_ptr<T> key, std::shared_ptr<V> value, int level, bool fullyLinked) :
            SkipNode<T, V>(
                    std::move(key),
                    std::move(value),
                    level),
            marked(false),
            fullyLinked(fullyLinked), lock() {
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
class ConcurrentSkipList : public OrderedMap<T, V> {

private:

    std::random_device randomEngine;
    std::mt19937 generator;
    std::uniform_int_distribution<unsigned int> distribution;

    std::unique_ptr<SkipNode<T, V>> rootNode;

    std::atomic<SkipNode<T, V> *> lastNode;

    std::atomic_uint32_t treeSize;
    std::atomic_uint32_t treeLevel;

public:
    ConcurrentSkipList() : randomEngine(),
                           rootNode(initializeNodeRoot(SKIP_LIST_HEIGHT_LIMIT)),
                           treeSize(0),
                           treeLevel(0),
                           lastNode(nullptr) {
        this->generator = std::mt19937(randomEngine());
    }

    ~ConcurrentSkipList() {
        this->rootNode.reset();
    }

protected:
    int generateLevel() {
        int random = distribution(generator);

        //Maybe get the amount of consecutive bits that have been set to 1 to get the correct level,
        //Instead of generating a new number every single time?

        int level = 0;

        while ((random & FIRST_BIT_MASK) != 0 && level < SKIP_LIST_HEIGHT_LIMIT) {
            level++;

            random = random << 1;
        }

        return level;
    }

    std::unique_ptr<SkipNode<T, V>> initializeNode(std::shared_ptr<T> key, std::shared_ptr<V> value) {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::move(key), std::move(value), this->generateLevel(),
                                                          false);
    }

    std::unique_ptr<ConcurrentSkipNode<T, V>>
    initializeNodeWithLevel(std::shared_ptr<T> key, std::shared_ptr<V> value, int level) {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::move(key), std::move(value), level, false);
    }

    std::unique_ptr<SkipNode<T, V>> initializeNodeRoot(int level) {
        return std::make_unique<ConcurrentSkipNode<T, V>>(std::shared_ptr<T>(), std::shared_ptr<V>(), level, true);
    }

    /**
     * Find the largest node that is <= to the key
     *
     * When the node found does not == the key, the returned levelFound is = -1
     *
     * @param key
     * @param predecessors
     * @param successors
     * @return
     */
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

    void listTraversalHelper(std::vector<node_info<T, V>> *results) {

        ConcurrentSkipNode<T, V> *root = this->getRoot();

        ConcurrentSkipNode<T, V> *next = root, *previous = root;

        do {
            next = (ConcurrentSkipNode<T, V> *) next->getNextNode(0);

            if (next->isMarked() || !next->isFullyLinked()) {
                //Node is being removed or node is still being added,
                //Wait until this operation is finished so we don't get any wrong values

                if (previous->isMarked()) {
                    //If the previous is being removed and the next is also being removed then they might reference
                    //The same node or even if they don't, that memory is going to be freed shortly, so we have to restart
                    //The collection of the nodes.
                    results->clear();

                    listTraversalHelper(results);

                    return;
                }

                next = previous;

                continue;
            } else {
                results->push_back(std::make_tuple(next->getKey(), next->getValue()));
            }

            previous = next;
        } while (next != nullptr);
    }

    void rangeSearchHelper(const T &base, const T &max, std::vector<node_info<T, V>> *results) {
        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        int levelFound = concurrentFindNode(base, predecessors, successors);

        ConcurrentSkipNode<T, V> *current = predecessors[0], *previous = current;

        while (current != nullptr && *current->getKeyVal() <= max) {

            if (current->isMarked() || !current->isFullyLinked()) {

                if (previous->isMarked()) {
                    //When we have the current and previous nodes marked
                    //Then we have no valid starting point for our search
                    //So we clear the current results
                    //And start over.
                    results->clear();

                    rangeSearchHelper(base, max, results);
                    return;
                }

                current = previous;

                continue;
            }

            previous = current;
            current = (ConcurrentSkipNode<T, V>*) current->getNextNode(0);
        }

    }

protected:
    int getListLevel() const {
        return this->treeLevel.load();
    }

    void setListLevel(int listLevel) {
        this->treeLevel.store(listLevel);
    }

    ConcurrentSkipNode<T, V> *getRoot() {
        return (ConcurrentSkipNode<T, V> *) this->rootNode.get();
    }

public:

    unsigned int size() override {
        return this->treeSize.load();
    }

    bool hasKey(const T &key) override {

        ConcurrentSkipNode<T, V> *predecessors[SKIP_LIST_HEIGHT_LIMIT], *successors[SKIP_LIST_HEIGHT_LIMIT];

        int levelFound = concurrentFindNode(key, predecessors, successors);

        ConcurrentSkipNode<T, V> *node = successors[levelFound];

        //The node has to be fully linked (Fully inserted) and not in the process of being removed
        return levelFound != -1 && node->isFullyLinked() && !node->isMarked() && (*node->getKeyVal() == key);
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

            SkipNode<T, V> *last = nullptr;

            do {
                last = lastNode.load();

                if (!(predecessors[0] == last || last == nullptr)) {
                    //If the predecessor to our new node is the previous last node and
                    //the last node is != null, then we ignore this node, as it cannot be the new
                    //Last node
                    break;
                }

            } while (!this->lastNode.compare_exchange_weak(last, node.get()));

            this->treeSize++;

            nodeP->setFullyLinked(true);

            while (true) {
                //Atomically update the height of the list
                unsigned int currentHeight = this->treeLevel.load();

                if (currentHeight < nodeLevel) {
                    if (this->treeLevel.compare_exchange_weak(currentHeight, nodeLevel)) break;
                } else {
                    break;
                }
            }

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
                        //If the node is already being removed
                        return std::nullopt;
                    }

                    toDelete->setMarked(true);
                    isMarked = true;
                }

                ConcurrentSkipNode<T, V> *predecessor = nullptr, *successor = nullptr, *previousPredecessor = nullptr;

                std::vector<std::unique_lock<std::mutex>> locks(SKIP_LIST_HEIGHT_LIMIT);
                bool valid = true;

                //Acquire the locks to all the predecessors of the node we want to remove.
                for (int level = 0; valid && level <= toDelete->getLevel(); level++) {
                    predecessor = predecessors[level];
                    successor = successors[level];

                    if (predecessor != previousPredecessor) {
                        std::unique_lock<std::mutex> lock(predecessor->getLock());

                        locks.push_back(std::move(lock));

                        previousPredecessor = predecessor;
                    }

                    valid = !predecessor->isMarked() && predecessor->getNextNode(level) == successor;
                }

                if (!valid) continue;

                std::unique_ptr<SkipNode<T, V>> toDeleteOwnership;
                //Now that we have all of the locks in our control, remove the node from the list
                for (int level = topLevel; level >= 0; level--) {

                    if (level == 0) {
                        std::unique_ptr<SkipNode<T, V>> nextNode = toDelete->getNextOwnership();

                        toDeleteOwnership = predecessors[level]->getNextOwnership();

                        predecessors[level]->setBaseNext(std::move(nextNode));
                    } else {
                        predecessors[level]->setNext(level, toDelete->getNextNode(level));
                    }
                }

                //Since we have the lock on all of the predecessors of the last node,
                //We know that no other operation can take place, so this does not need
                //A big amount of synchronization
                if (this->lastNode.load() == toDeleteOwnership.get()) {
                    this->lastNode.store(predecessors[0]);
                }

                this->treeSize--;

                unsigned int tLevel, newLevel;

                do {
                    tLevel = this->treeLevel.load();

                    newLevel = tLevel;

                    while (newLevel > 0 && this->getRoot()->getNextNode(tLevel) == nullptr) {
                        newLevel--;
                    }

                } while (!this->treeLevel.compare_exchange_weak(tLevel, newLevel));

                return toDeleteOwnership.get()->getValue();
            } else {
                return std::nullopt;
            }
        }
    }

    std::optional<node_info<T, V>> peekLargest() override {
        while (true) {

            ConcurrentSkipNode<T, V> *load = (ConcurrentSkipNode<T, V> *) this->lastNode.load();
            if (load == nullptr) {
                return std::nullopt;
            } else {

                //If the node is being removed or added, retry
                if (!load->isFullyLinked() || load->isMarked()) continue;

                return std::make_tuple(load->getKey(), load->getValue());
            }
        }
    }

    std::optional<node_info<T, V>> peekSmallest() override {

        ConcurrentSkipNode<T, V> *root = this->getRoot();

        ConcurrentSkipNode<T, V> *next;

        do {
            next = (ConcurrentSkipNode<T, V> *) root->getNextNode(0);

            if (next == nullptr) return std::nullopt;

        } while (next->isMarked() || !next->isFullyLinked());

        return std::make_tuple(next->getKey(), next->getValue());
    }

    std::unique_ptr<std::vector<std::shared_ptr<T>>> keys() override {

        std::unique_ptr<std::vector<std::shared_ptr<T>>> results =
                std::make_unique<std::vector<std::shared_ptr<T>>>(this->size());

        std::vector<node_info<T, V>> cache(this->size());

        listTraversalHelper(&cache);

        for (node_info<T, V> &start : cache) {
            results->push_back(std::move(std::get<0>(start)));
        }

        return std::move(results);
    }

    std::unique_ptr<std::vector<std::shared_ptr<V>>> values() override {

        std::unique_ptr<std::vector<std::shared_ptr<V>>> results =
                std::make_unique<std::vector<std::shared_ptr<V>>>(this->size());

        std::vector<node_info<T, V>> cache(this->size());

        listTraversalHelper(&cache);

        for (node_info<T, V> &start : cache) {
            results->push_back(std::move(std::get<1>(start)));
        }

        return std::move(results);
    }

    std::unique_ptr<std::vector<node_info<T, V>>> entries() override {
        std::unique_ptr<std::vector<node_info<T, V>>> results = std::make_unique<std::vector<node_info<T, V>>>(
                this->size());

        listTraversalHelper(results.get());

        return std::move(results);
    }

    std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) override {

        std::unique_ptr<std::vector<node_info<T, V>>> results = std::make_unique<std::vector<node_info<T, V>>>(
                this->size());

        rangeSearchHelper(base, max, results.get());

        return std::move(results);
    }

    std::optional<node_info<T, V>> popSmallest() override {

        ConcurrentSkipNode<T, V> *root = this->getRoot();

        ConcurrentSkipNode<T, V> *next;

        do {
            next = (ConcurrentSkipNode<T, V>*) root->getNextNode(0);

            if (next == nullptr) return std::nullopt;

        } while (next->isMarked() || !next->isFullyLinked());

        auto peekResult = std::make_tuple(next->getKey(), next->getValue());

        auto result = remove(*next->getKeyVal());

        if (result) {
            return peekResult;
        }

        return popSmallest();
    }

    std::optional<node_info<T, V>> popLargest() override {
        while (true) {

            ConcurrentSkipNode<T, V> *load = (ConcurrentSkipNode<T, V> *) this->lastNode.load();
            if (load == nullptr) {
                return std::nullopt;
            } else {

                //If the node is being removed or added, retry
                if (!load->isFullyLinked() || load->isMarked()) continue;

                auto peekResult = std::make_tuple(load->getKey(), load->getValue());

                auto result = remove(*load->getKeyVal());

                if (result) {
                    return peekResult;
                }
            }
        }
    }

};

#endif //TRABALHO1_CONCURRENTSKIPLIST_H

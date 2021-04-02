#ifndef TRABALHO1_CONCURRENTSKIPLIST_H
#define TRABALHO1_CONCURRENTSKIPLIST_H

#include <mutex>
#include "skiplist.h"

template<typename T, typename V>
class ConcurrentSkipNode : public SkipNode<T, V> {

private:
    bool marked;
    bool fullyLinked;
    std::mutex lock;



};

#endif //TRABALHO1_CONCURRENTSKIPLIST_H

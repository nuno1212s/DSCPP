#ifndef TRABALHO1_TREES_H
#define TRABALHO1_TREES_H

#include "../datastructures.h"

template<class T>
class TreeSet : public Set<T> {

public:
    virtual std::optional<std::shared_ptr<T>> peekSmallest() = 0;

    virtual std::optional<std::shared_ptr<T>> peekLargest() = 0;

    virtual std::optional<std::shared_ptr<T>> popSmallest() = 0;

    virtual std::optional<std::shared_ptr<T>> popLargest() = 0;

    virtual std::vector<std::shared_ptr<T>> rangeSearch(const T &base, const T &max) = 0;

};

template<typename T, typename V>
class TreeMap : public Map<T, V> {

public:
    virtual std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) = 0;

    virtual std::optional<node_info<T, V>> peekSmallest() = 0;

    virtual std::optional<node_info<T, V>> peekLargest() = 0;

    virtual std::optional<node_info<T, V>> popSmallest() = 0;

    virtual std::optional<node_info<T, V>> popLargest() = 0;

    virtual int getTreeHeight() = 0;
};

#endif //TRABALHO1_TREES_H

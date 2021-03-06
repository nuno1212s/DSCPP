#ifndef TRABALHO1_DATASTRUCTURES_H
#define TRABALHO1_DATASTRUCTURES_H

#include <vector>
#include <memory>
#include <optional>

template<typename T>
class Set {
public:
    virtual void add(const T &key) = 0;

    virtual bool contains(const T &key) = 0;

    virtual std::shared_ptr<T> find(const T &key) = 0;

    virtual std::shared_ptr<T> remove(const T &key) = 0;

    virtual unsigned int size() = 0;

};

template<typename T, typename V>
using node_info = std::tuple<std::shared_ptr<T>, std::shared_ptr<V>>;

template<typename T, typename V, class A = std::allocator<node_info<T, V>>>
class Map {

public:
    virtual void add(std::shared_ptr<T> key, std::shared_ptr<V> value) = 0;

    virtual bool hasKey(const T &key) = 0;

    virtual std::optional<std::shared_ptr<V>> get(const T &key) = 0;

    virtual std::optional<std::shared_ptr<V>> remove(const T &key) = 0;

    virtual std::unique_ptr<std::vector<std::shared_ptr<T>>> keys() = 0;

    virtual std::unique_ptr<std::vector<std::shared_ptr<V>>> values() = 0;

    virtual std::unique_ptr<std::vector<node_info<T, V>>> entries() = 0;

    virtual unsigned int size() = 0;

};

template<typename T, typename V>
class OrderedMap : public Map<T, V> {

public:
    virtual ~OrderedMap() {};

    virtual std::unique_ptr<std::vector<node_info<T, V>>> rangeSearch(const T &base, const T &max) = 0;

    virtual std::optional<node_info<T, V>> peekSmallest() = 0;

    virtual std::optional<node_info<T, V>> peekLargest() = 0;

    virtual std::optional<node_info<T, V>> popSmallest() = 0;

    virtual std::optional<node_info<T, V>> popLargest() = 0;

};

template<typename T>
class Filter {
public:
    virtual ~Filter() {};

    virtual bool test(const T &key) = 0;

    virtual void add(const T &key) = 0;

    virtual unsigned int size() = 0;

};

//Randomly generated large prime number
#define SEED 0x7A92F67B

class HashFunction {
public:
    virtual unsigned int hashObject(const void *, uint32_t size, int seed) = 0;

};

#endif //TRABALHO1_DATASTRUCTURES_H

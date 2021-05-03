#ifndef TRABALHO1_CONCURRENTBLOOMFILTER_H
#define TRABALHO1_CONCURRENTBLOOMFILTER_H

#include "../datastructures.h"
#include <atomic>
#include <mutex>

#define DEFAULT_HASH_FUNCTIONS 3
#define DEFAULT_BLOOM_FILTER_SIZE 512000 //512KB default size

template<typename T>
class ConcurrentBloomFilter : public Filter<T> {

private:
    unsigned int byteSize;

    std::atomic_uint32_t items;

    std::unique_ptr<std::vector<uint64_t>> data;

    std::unique_ptr<std::vector<std::mutex>> mutexesPerRegion;

    unsigned int calculatePositionFromHash(unsigned int hash) {
        return hash % (byteSize * UINT8_WIDTH);
    }

    void setBitToOne(uint64_t &toChange, unsigned int position) {

        //An int containing a bit set to one, this bit is located in the LSB (So should be the right most bit in little
        //Endian machines)
        //Move the bit to the correct position
        uint64_t oneBit = 1 << position;

        //Bitwise OR the bit to set that position to 1
        toChange |= oneBit;
    }

    bool getBitValue(const uint64_t &value, unsigned int position) {

        //Move the bit to the correct position
        uint64_t oneBit = 1 << position;

        //By doing a Bitwise AND with a byte that only has the position we want to check at 1, then
        //The value will only be bigger than 0 when that bit is also set to 1 on the bloomfilter
        oneBit &= value;

        return oneBit > 0;
    }

public:

    ConcurrentBloomFilter(unsigned int byteSize = DEFAULT_BLOOM_FILTER_SIZE) :
            byteSize(byteSize), items(0) {

        data = std::make_unique<std::vector<uint64_t>>(this->byteSize / 8);

        mutexesPerRegion = std::make_unique<std::vector<std::mutex>> > (this->byteSize / 8);
    }

    bool test(const T &key) override {
        /*
         * We do not need to synchronize here, as the intermediate states for adding an element are accepted states
         * For testing if it contains. We might get a NO if when we test if contains x while we add x to the filter, but
         * This is fine.
         */
        for (int i = 0; i < DEFAULT_HASH_FUNCTIONS; i++) {

            MurmurHash hashFunc;

            unsigned int hash = hashFunc.hashObject(key, sizeof(T));

            hash = calculatePositionFromHash(hash);

            unsigned int bytePosition = hash / UINT8_WIDTH;

            //For the result to be yes, then all bit results from all the hash functions
            //Have to be set to one.
            //If any of them is set to 0, then the key is definitely not in the filter
            if (!getBitValue(this->data[bytePosition], hash % UINT8_WIDTH)) {
                return false;
            }
        }

        return true;
    }

    void add(const T &key) override {

        for (int i = 0; i < DEFAULT_HASH_FUNCTIONS; i++) {

            MurmurHash hashFunc;

            unsigned int hash = hashFunc.hashObject(key, sizeof(T));

            hash = calculatePositionFromHash(hash);

            //The position of the byte in the
            unsigned int bytePosition = hash / UINT64_WIDTH;

            //Acquire the lock for the region we want to change
            std::unique_lock<std::mutex> lock(mutexesPerRegion[bytePosition]);

            setBitToOne(this->data[bytePosition], hash % UINT64_WIDTH);
        }

        items++;

    }

    unsigned int size() override {
        return items.load();
    }

};

#endif //TRABALHO1_CONCURRENTBLOOMFILTER_H

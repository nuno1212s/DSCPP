#ifndef TRABALHO1_BLOOMFILTER_H
#define TRABALHO1_BLOOMFILTER_H

#include "../datastructures.h"
#include "hashes/MurmurHash3.h"
#include "hashes/SpookyV2.h"
#include <math.h>

#define DEFAULT_HASH_FUNCTIONS 3
#define DEFAULT_BLOOM_FILTER_SIZE 512000 //512KB default size

unsigned int getRecommendedSizeFor(int hashFunctions, int itemCount, float failRate) {
    return (unsigned int) ((hashFunctions * itemCount) / log10(2));
}

template<typename T>
class BloomFilter : public Filter<T> {

private:
    /**
     * The amount of bytes that make up this bloomfilter
     */
    unsigned int byteSize;

    /*
     * The number of items that are stored in this bloomfilter
     */
    unsigned int items;

    /**
     * Unique ptr to the data, which should be byteSize bytes long
     */
    std::unique_ptr<std::vector<uint8_t>> data;

private:
    unsigned int getHashFunction(int hashFunc, const void *key, uint32_t size) {

        static MurmurHash murmurHash;

        static SpookyHashImpl spookyHash;

        if (hashFunc % 2 == 0) {
            return murmurHash.hashObject(key, size, hashFunc);
        } else {
            return spookyHash.hashObject(key, size, hashFunc);
        }
    }

    unsigned int calculatePositionFromHash(unsigned int hash) {

        //Calculate the exact bit where the hash is pointing to
        unsigned int finalHash = hash % (byteSize * UINT8_WIDTH);

        return finalHash;
    }

    void setBitToOne(uint8_t &toChange, unsigned int position) {

        //An int containing a bit set to one, this bit is located in the LSB (So should be the right most bit in little
        //Endian machines)
        //Move the bit to the correct position
        uint8_t oneBit = 1 << position;

        //Bitwise OR the bit to set that position to 1
        toChange |= oneBit;
    }

    bool getBitValue(const uint8_t &value, unsigned int position) {

        //Move the bit to the correct position
        uint8_t oneBit = 1 << position;

        //By doing a Bitwise AND with a byte that only has the position we want to check at 1, then
        //The value will only be bigger than 0 when that bit is also set to 1 on the bloomfilter
        oneBit &= value;

        return oneBit > 0;
    }

public:
    BloomFilter(unsigned int byteSize = DEFAULT_BLOOM_FILTER_SIZE)
            : byteSize(byteSize) {
        data = std::make_unique<std::vector<uint8_t>>(this->byteSize);
    };

    ~BloomFilter() {
        data.reset();
    }

    bool test(const T &key) override {

        for (int i = 0; i < DEFAULT_HASH_FUNCTIONS; i++) {

            unsigned int hash = getHashFunction(i, &key, sizeof(T));

            hash = calculatePositionFromHash(hash);

            unsigned int bytePosition = hash / UINT8_WIDTH;

            //For the result to be yes, then all bit results from all the hash functions
            //Have to be set to one.
            //If any of them is set to 0, then the key is definitely not in the filter
            if (!getBitValue((*this->data)[bytePosition], hash % UINT8_WIDTH)) {
                return false;
            }
        }

        return true;
    }

    void add(const T &key) override {

        for (int i = 0; i < DEFAULT_HASH_FUNCTIONS; i++) {

            unsigned int hash = getHashFunction(i, &key, sizeof(T));

            hash = calculatePositionFromHash(hash);

            unsigned int bytePosition = hash / UINT8_WIDTH;

            setBitToOne((*this->data)[bytePosition], hash % UINT8_WIDTH);
        }

        items++;
    }

    unsigned int size() override {
        return this->items;
    }

    unsigned int bitSize() {
        return byteSize * UINT8_WIDTH;
    };

};


#endif //TRABALHO1_BLOOMFILTER_H

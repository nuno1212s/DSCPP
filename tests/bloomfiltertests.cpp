#include "gtest/gtest.h"
#include "../filters/bloomfilter.h"

#define TEST_SIZE 1000000

TEST(BloomFilterTest, N_CONTAINS) {

    std::unique_ptr<BloomFilter<int>> bloom = std::make_unique<BloomFilter<int>>();

    for (int i = 0; i < TEST_SIZE; i++) {
        bloom->add(i);
    }

    for (int i = 0; i < TEST_SIZE * 2; i++) {

        bool result = bloom->test(i);

        if (!result) {
            //If the bloom filter returns false, then the number cannot belong to the bloom filter, meaning
            //It has to be >= than the test size
            ASSERT_TRUE(i >= TEST_SIZE);
        } else {
            //If the bloom filter returns true, then the number might belong to the bloom filter,
            //But there is no way to factually verify it.
        }

    }

}

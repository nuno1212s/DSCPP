#include "gtest/gtest.h"
#include "../filters/bloomfilter.h"
#include <chrono>

#define TEST_SIZE 100
#define TESTS 5
#define TEST_INCR 10

TEST(BloomFilterTest, N_CONTAINS) {

    int testSize = TEST_SIZE;

    for (int j = 0; j <= TESTS; j++) {


        std::unique_ptr<BloomFilter<int>> bloom = std::make_unique<BloomFilter<int>>();

        auto start = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < testSize; i++) {
            bloom->add(i);
        }

        for (int i = 0; i < testSize * 2; i++) {

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

        testSize *= TEST_INCR;

        auto end = std::chrono::high_resolution_clock::now();

        std::cout << "Took " << std::chrono::duration_cast<std::chrono::milliseconds>(end - start).count()
                  << " ms to complete." << std::endl;
    }
}

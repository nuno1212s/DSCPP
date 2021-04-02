#include "gtest/gtest.h"
#include "../probabilisticlist/skiplist.h"

TEST(SkipListTests, TestInsert) {

    auto skipList = std::make_unique<SkipList<int, int>>();

    auto value = std::make_shared<int>(42);

    skipList->add(std::make_shared<int>(1), value);

    ASSERT_TRUE(skipList->hasKey(1));

    ASSERT_EQ(skipList->size(), 1);
}

TEST(SkipListTests, TestInsertRemove) {

    auto skipList = std::make_unique<SkipList<int, int>>();

    auto value = std::make_shared<int>(42);

    skipList->add(std::make_shared<int>(1), value);

    ASSERT_TRUE(skipList->hasKey(1));

    ASSERT_EQ(skipList->size(), 1);

    skipList->remove(1);

    ASSERT_FALSE(skipList->hasKey(1));

    ASSERT_EQ(skipList->size(), 0);

}
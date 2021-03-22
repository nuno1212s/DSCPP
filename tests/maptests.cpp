#include "gtest/gtest-param-test.h"

#include "../datastructures.h"

#include "../trees/avltree.h"

void testMapInsertAndContains(std::unique_ptr<Map<int, int>> map) {

    Map<int, int> *testMap = map.get();

//    testMap->add(1, 2);
//    testMap->add(2, 3);
//    testMap->add(3, 4);

    assert(testMap->hasKey(1));

    assert(testMap->hasKey(2));

    assert(testMap->hasKey(3));

}
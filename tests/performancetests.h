
#ifndef TRABALHO1_PERFORMANCETESTS_H
#define TRABALHO1_PERFORMANCETESTS_H

#include "../datastructures.h"
#include <fstream>

enum T_Type {

    INSERT,
    REMOVE,
    CONTAINS,
    N_CONTAINS,
    POP_LARGEST,
    POP_SMALLEST,
    SIZE

};

typedef struct TestCase {
    T_Type type;

    int value;

    TestCase (T_Type tType, int value) : type(tType), value(value) {}
} TCase;

void executeTests(std::string filename);

std::unique_ptr<std::vector<TCase>> parseTestCases(std::fstream &fp);

void runTests(OrderedMap<int, int> *map, std::vector<TCase> &tests);

#endif //TRABALHO1_PERFORMANCETESTS_H

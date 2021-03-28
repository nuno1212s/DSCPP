#include "datastructures.h"
#include "trees/avltree.h"
#include <iostream>
#include <memory>

enum TestAction {

    INSERT,
    REMOVE,
    CONTAINS,
    POP_SMALLEST,
    POP_LARGEST

};

TestAction readFromString(std::string &string) {

    if (string == "I") {
        return TestAction::INSERT;
    } else if (string == "R") {
        return TestAction::REMOVE;
    } else if (string == "C") {
        return TestAction::CONTAINS;
    } else if (string == "PS") {
        return TestAction::POP_SMALLEST;
    } else if (string == "PL") {
        return TestAction::POP_LARGEST;
    }

}
cmake_minimum_required(VERSION 3.17)
project(Trabalho1)

set(CMAKE_CXX_STANDARD 17)

add_executable(Trabalho1 main.cpp trees/binarytrees.h trees/avltree.h datastructures.h tests/maptests.cpp trees/trees.h)
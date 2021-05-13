cmake_minimum_required(VERSION 3.17)
project(Trabalho1)

set(CMAKE_CXX_STANDARD 17)

# Download and unpack googletest at configure time
configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "CMake step for googletest failed: ${result}")
endif()
execute_process(COMMAND ${CMAKE_COMMAND} --build .
        RESULT_VARIABLE result
        WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/googletest-download )
if(result)
    message(FATAL_ERROR "Build step for googletest failed: ${result}")
endif()

# Prevent overriding the parent project's compiler/linker
# settings on Windows
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

# Add googletest directly to our build. This defines
# the gtest and gtest_main targets.
add_subdirectory(${CMAKE_CURRENT_BINARY_DIR}/googletest-src
        ${CMAKE_CURRENT_BINARY_DIR}/googletest-build
        EXCLUDE_FROM_ALL)

# Now simply link against gtest or gtest_main as needed. Eg
add_executable(Trabalho1 trees/binarytrees.h trees/avltree.h datastructures.h tests/maptests.cpp
        trees/redblacktree.h trees/splaytree.h tests/avltests.cpp tests/rebblacktests.cpp trees/treaps.h
        tests/treaptests.cpp probabilisticlist/skiplist.h probabilisticlist/concurrentskiplist.h tests/skiplisttests.cpp
        filters/bloomfilter.h filters/hashes/MurmurHash3.cpp filters/hashes/MurmurHash3.h filters/hashes/SpookyV2.cpp
        filters/concurrentbloomfilter.h tests/orderedmapperftests.cpp tests/bloomfiltertests.cpp)

target_link_libraries(Trabalho1 gtest_main)
add_test(NAME example_test COMMAND Trabalho1)
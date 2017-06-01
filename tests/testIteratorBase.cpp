#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <string>
#include <new>
#include <cstdlib>
#include "../tinystl/iteratorbase.h"

namespace tinystl {
    template<>
    struct IteratorTraits<typename std::string::iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::string::value_type;
        using DifferenceType = typename std::string::difference_type;
        using Pointer = typename std::string::value_type*;
        using Reference = typename std::string::reference;
    };
}

TEST(iteratorBase, distance) {
    std::string str = "hello world!";
    for(auto it = str.begin(); it != str.end(); ++it) {
        ASSERT_EQ(tinystl::distance(it, str.end()),
                  std::distance(it, str.end()));
        tinystl::distance(it, str.end());
    }
}

TEST(iteratorBase, distance2) {
    int vec[] = {1, 2, 3, 4, 5, 6, 7};
    int *beg = vec;
    int *end = beg + sizeof(vec) / sizeof(int);
    while(beg != end) {
        ASSERT_EQ(tinystl::distance(beg, end),
                  std::distance(beg, end));
        ++beg;
    }
}

TEST(iteartorBase, advance) {
    std::string str(100000, 'a');
    auto prev = str.begin();
    tinystl::advance( prev, -1);
    for(auto it = str.begin(); it != str.end(); tinystl::advance(it, 1)) {
        ASSERT_EQ(tinystl::distance(prev, it), 1);
        tinystl::advance(prev, 1);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

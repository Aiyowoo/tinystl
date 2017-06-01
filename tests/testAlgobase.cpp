#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <string>
#include <cassert>
#include <deque>
#include <new>
#include <cstdlib>
#include "../tinystl/algobase.h"

namespace tinystl {
    template<>
    struct IteratorTraits<typename std::string::iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::string::value_type;
        using DifferenceType = typename std::string::difference_type;
        using Pointer = typename std::string::value_type*;
        using Reference = typename std::string::reference;
    };

    template<>
    struct IteratorTraits<typename std::deque<int>::iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::deque<int>::value_type;
        using DifferenceType = typename std::deque<int>::difference_type;
        using Pointer = typename std::deque<int>::value_type*;
        using Reference = typename std::deque<int>::reference;
    };
}

TEST(algobase, copy) {
    std::string src(100000, '1');
    std::string dest(src.size(), ' ');
    auto r = tinystl::copy(src.begin(), src.end(), dest.begin());
    ASSERT_EQ(r, dest.end());
    for(auto it = dest.begin(); it != dest.end(); ++it) {
        ASSERT_EQ(*it, '1');
    }

    std::deque<int> datas(100000, 2);
    std::deque<int> destDatas(100000);
    std::deque<int>::iterator ret = tinystl::copy(datas.begin(), datas.end(), destDatas.begin());
    ASSERT_EQ(ret, destDatas.end());
    for(auto it = destDatas.begin(); it != destDatas.end(); ++it) {
        ASSERT_EQ(*it, 2);
    }

    int s[100];
    int d[100];
    for(int i = 0; i < sizeof(s) / sizeof(int); ++i) {
        s[i] = i;
    }
    ASSERT_EQ(tinystl::copy(std::begin(s), std::end(s), std::begin(d)),
              std::end(d));
    for(int i = 0; i < sizeof(s) / sizeof(int); ++i) {
        ASSERT_EQ(d[i], i);
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

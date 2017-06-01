#include <gtest/gtest.h>
#include <string>
#include <new>
#include <cstdlib>
#include "../tinystl/iterator.h"

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
    struct IteratorTraits<typename std::string::const_iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::string::value_type;
        using DifferenceType = typename std::string::difference_type;
        using Pointer = typename std::string::const_pointer;
        using Reference = typename std::string::const_reference;
    };
}

TEST(iterator, testReverseIterator) {
    std::string str = "hello world";
    auto it = tinystl::ReverseIterator<std::string::iterator>(str.end());
    auto end = tinystl::ReverseIterator<std::string::iterator>(str.begin());
    std::string newStr;
    while(it != end) {
        newStr.push_back(*it++);
    }
    std::reverse(str.begin(), str.end());

    ASSERT_EQ(str, newStr);


    auto cit = tinystl::ReverseIterator<std::string::const_iterator>(str.cend());
    auto cend = tinystl::ReverseIterator<std::string::const_iterator>(str.cbegin());
    std::string newStr2;
    while(cit != cend) {
        newStr2.push_back(*cit++);
    }
    std::reverse(str.begin(), str.end());
    ASSERT_EQ(str, newStr2);
}

TEST(iterator, testReverseIteratorLess) {
    std::string str = "hello world";
    auto rbeg = tinystl::ReverseIterator<std::string::iterator>(str.end());
    auto rend = tinystl::ReverseIterator<std::string::iterator>(str.begin());
    ASSERT_LT(rbeg, rend);
    ASSERT_LE(rbeg, rend);
    ASSERT_NE(rbeg, rend);
    ASSERT_GE(rend, rbeg);
    ASSERT_GT(rend, rbeg);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

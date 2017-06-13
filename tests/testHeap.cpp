#include <gtest/gtest.h>
#include <vector>
#include <algorithm>
#include "../tinystl/algorithm.h"
#include "../tinystl/vector.h"
#include "../tinystl/algobase.h"

namespace tinystl {

    template<>
    struct IteratorTraits<std::vector<int>::iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::pointer;
        using Reference = typename std::vector<int>::reference;
    };

    template<>
    struct IteratorTraits<std::vector<int>::const_iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::const_pointer;
        using Reference = typename std::vector<int>::const_reference;
    };
}

template<typename T>
inline bool operator==(const std::vector<T> &lhs, const tinystl::Vector<T> &rhs) {
    return lhs.size() == rhs.size() &&
        tinystl::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template<typename T>
inline bool operator==(const tinystl::Vector<T> &lhs, const std::vector<T> &rhs) {
    return rhs == lhs;
}

TEST(Heap, operators) {
    std::vector<int> s = {3, 4, 1, 2, 5, 2};
    tinystl::Vector<int> t(s.cbegin(), s.cend());
    ASSERT_TRUE(s == t);

    std::make_heap(s.begin(), s.end());
    tinystl::makeHeap(t.begin(), t.end());
    ASSERT_TRUE(s == t);

    ASSERT_TRUE(std::is_heap(s.begin(), s.end()));
    ASSERT_TRUE(tinystl::isHeap(t.begin(), t.end()));

    std::pop_heap(s.begin(), s.end());
    tinystl::popHeap(t.begin(), t.end());
    ASSERT_TRUE(s == t);

    std::pop_heap(s.begin(), s.end() - 1);
    tinystl::popHeap(t.begin(), t.end() - 1);
    ASSERT_TRUE(s == t);

    std::make_heap(s.begin(), s.end());
    tinystl::makeHeap(t.begin(), t.end());
    ASSERT_TRUE(s == t);
    std::sort_heap(s.begin(), s.end());
    tinystl::sortHeap(t.begin(), t.end());
    ASSERT_TRUE(s == t);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

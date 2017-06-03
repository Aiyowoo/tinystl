#include <gtest/gtest.h>
#include <vector>
#include "../tinystl/vector.h"
#include "../tinystl/algobase.h"
#include "../tinystl/iteratortraits.h"

namespace tinystl {
    template<>
    struct IteratorTraits<typename std::vector<int>::iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::pointer;
        using Reference = typename std::vector<int>::reference;
    };

    template<>
    struct IteratorTraits<typename std::vector<int>::const_iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::const_pointer;
        using Reference = typename std::vector<int>::const_reference;
    };

    template<>
    struct IteratorTraits<typename std::vector<int>::reverse_iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::pointer;
        using Reference = typename std::vector<int>::reference;
    };

    template<>
    struct IteratorTraits<typename std::vector<int>::const_reverse_iterator> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = typename std::vector<int>::value_type;
        using DifferenceType = typename std::vector<int>::difference_type;
        using Pointer = typename std::vector<int>::const_pointer;
        using Reference = typename std::vector<int>::const_reference;
    };
}

TEST(vector, defaultConstructor) {
    tinystl::Vector<int> a;
    std::vector<int> b;

    ASSERT_EQ(a.size(), b.size());
}

TEST(vector, fillInitialize) {
    tinystl::Vector<int> a(100, 1);
    std::vector<int> b(100, 1);
    ASSERT_EQ(a.size(), b.size());
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), b.cbegin()));
}

TEST(vector, rangeInitialize) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());
    ASSERT_EQ(a.size(), data.size());
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, copyConstructor) {
    tinystl::Vector<int> a(100, 2);
    tinystl::Vector<int> b = a;
    ASSERT_EQ(a, b);
}

TEST(vector, begin) {
    tinystl::Vector<int> a(100, 1);
    tinystl::Vector<int>::Iterator beg = a.begin();
    ASSERT_EQ(*beg, 1);
    *beg = 100;

    const tinystl::Vector<int> b(100, 1);
    auto begin = b.begin();
    ASSERT_EQ(*begin, 1);
}

TEST(vector, cbegin) {
    tinystl::Vector<int> a(100, 1);
    auto beg = a.cbegin();
    ASSERT_EQ(*beg, 1);

    // *beg = 1; 不可赋值
}

TEST(vector, endAndCend) {
    tinystl::Vector<int> a(100, 1);
    auto beg = a.begin();
    auto end = a.end();
    ASSERT_EQ(100, tinystl::distance(beg, end));
    ASSERT_EQ(100, tinystl::distance(a.cbegin(), a.cend()));
}

TEST(vector, reverseIterator) {
    std::vector<int> a = {1, 2, 3, 4, 5};
    tinystl::Vector<int> b(a.cbegin(), a.cend());
    std::vector<int> c(a.crbegin(), a.crend());
    tinystl::Vector<int> d(b.crbegin(), b.crend());
    tinystl::Vector<int> e(b.rbegin(), b.rend());

    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), b.cbegin()));
    ASSERT_TRUE(tinystl::equal(c.cbegin(), c.cend(), d.cbegin()));
    ASSERT_TRUE(tinystl::equal(c.cbegin(), c.cend(), e.cbegin()));
}

TEST(vector, empty) {
    tinystl::Vector<int> a;
    ASSERT_TRUE(a.empty());
    a.pushBack(0);
    ASSERT_FALSE(a.empty());
}

TEST(vector, n) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());
    ASSERT_EQ(data.size(), a.size());
    for(int i = 0; i < data.size(); ++i) {
        ASSERT_EQ(data[i], a[i]);
    }
}

TEST(vector, at) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());
    ASSERT_EQ(data.size(), a.size());
    for(int i = 0; i < data.size(); ++i) {
        ASSERT_EQ(data[i], a.at(i));
    }
    ASSERT_THROW(a.at(1000), std::out_of_range);
}

TEST(vector, pushBack) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a;
    for(auto value: data) {
        a.pushBack(value);
    }
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, swap) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());
    tinystl::Vector<int> b;
    a.swap(b);
    ASSERT_TRUE(a.empty());
    ASSERT_TRUE(tinystl::equal(b.cbegin(), b.cend(), data.cbegin()));
}

TEST(vector, insert) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());
    data.insert(data.begin(), 1);
    a.insert(a.begin(), 1);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.insert(data.end(), 1);
    a.insert(a.end(), 1);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.insert(data.begin(), 10, 0);
    a.insert(a.begin(), 10, 0);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    std::vector<int> newData = {2, 3, 4};
    data.insert(data.end(), newData.begin(), newData.end());
    a.insert(a.end(), newData.begin(), newData.end());
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, erase) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());

    data.erase(data.begin());
    a.erase(a.begin());
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.erase(data.begin(), data.begin() + 2);
    a.erase(a.begin(), a.begin() + 2);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, resize) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(data.cbegin(), data.cend());

    data.resize(100);
    a.resize(100);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.resize(1000, 2);
    a.resize(1000, 2);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.resize(5);
    a.resize(5);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, clear) {
    tinystl::Vector<int> a(100, 1);
    a.clear();
    ASSERT_TRUE(a.empty());

    tinystl::Vector<int> empty;
    empty.clear();
    ASSERT_TRUE(empty.empty());
}

TEST(vector, assign) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(100, 1);

    a.assign(data.cbegin(), data.cend());
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));

    data.assign(1000, 0);
    a.assign(1000, 0);
    ASSERT_TRUE(tinystl::equal(a.cbegin(), a.cend(), data.cbegin()));
}

TEST(vector, compare) {
    std::vector<int> data = {1, 2, 3, 4, 5};
    tinystl::Vector<int> a(100, 1);
    tinystl::Vector<int> b(100, 1);
    ASSERT_TRUE(a == b);
    ASSERT_FALSE(a != b);

    a.assign(10, 0);
    ASSERT_TRUE(a != b);
    ASSERT_FALSE(a == b);

    ASSERT_TRUE(a < b);
    ASSERT_TRUE(a <= b);

    ASSERT_FALSE(a > b);
    ASSERT_FALSE(a >= b);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

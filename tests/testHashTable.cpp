#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "../tinystl/algobase.h"
#include "../tinystl/hashtable.h"
#include "../tinystl/alloc.h"

template<typename T>
struct HashFun {
    const T& operator()(const T &value) const {
        return value;
    }
};

template<typename T>
struct KeyExtractor {
    const T& operator()(const T &value) const {
        return value;
    }
};

using HashTable = tinystl::HashTable<int, int, HashFun<int>,
                                     KeyExtractor<int>, tinystl::Equal<int>,
                                     tinystl::Alloc>;

TEST(HashTable, constructors) {
    HashTable a;
    ASSERT_TRUE(a.empty());
    ASSERT_TRUE(a.size() == 0);
}

TEST(HashTable, maintainers) {
    HashTable a;
    auto res = a.insertUnique(1);
    ASSERT_FALSE(a.empty());
    ASSERT_EQ(a.size(), 1);
    ASSERT_EQ(*res.first, 1);
    ASSERT_TRUE(res.second);

    res = a.insertUnique(2);
    ASSERT_FALSE(a.empty());
    ASSERT_EQ(a.size(), 2);
    ASSERT_EQ(*res.first, 2);
    ASSERT_TRUE(res.second);

    res = a.insertUnique(1);
    ASSERT_FALSE(a.empty());
    ASSERT_EQ(a.size(), 2);
    ASSERT_EQ(*res.first, 1);
    ASSERT_FALSE(res.second);

    int data[] = {1, 2, 3, 4, 5, 1, 1, 2, 3};
    a.insertUnique(std::begin(data), std::end(data));
    ASSERT_EQ(a.size(), 5);

    auto it = a.insertEqual(6);
    ASSERT_EQ(*it, 6);
    ASSERT_EQ(a.size(), 6);

    it = a.insertEqual(1);
    ASSERT_EQ(*it, 1);
    ASSERT_EQ(a.size(), 7);

    a.insertEqual(std::begin(data), std::end(data));
    ASSERT_EQ(a.size(), 16);

    // erase
    a.insertUnique(0);
    a.erase(a.begin());
    ASSERT_EQ(a.size(), 16);
    it = a.find(0);
    ASSERT_TRUE(it == a.end());

    auto range = a.equalRange(1);
    ASSERT_EQ(tinystl::distance(range.first, range.second), 5);

    ASSERT_EQ(a.count(1), 5);

    HashTable::SizeType count = a.erase(1);
    ASSERT_EQ(count, 5);
    it = a.find(1);
    ASSERT_TRUE(it == a.end());

    it = a.begin();
    int oldValue = *it;
    ++it;
    a.erase(it, a.end());
    ASSERT_EQ(a.size(), 1);
    ASSERT_EQ(*a.begin(), oldValue);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

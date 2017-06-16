#include <gtest/gtest.h>
#include <iostream>
#include <vector>
#include "../tinystl/rbtree.h"
#include "../tinystl/algobase.h"

struct KeyOfValue {
    const int& operator()(const int &x) {
        return x;
    }
};

using RBTree = tinystl::RBTree<int, int, KeyOfValue, tinystl::Less<int>>;

TEST(RBTree, constructors) {
    RBTree t;
    ASSERT_TRUE(t.empty());
    ASSERT_TRUE(t.rbVerify());

    auto compare = tinystl::Less<int>();
    RBTree t2(compare);
    ASSERT_TRUE(t.empty());
    ASSERT_TRUE(t.rbVerify());
}

TEST(RBTree, operators) {
    RBTree t;
    auto res = t.insertUnique(1);
    ASSERT_TRUE(res.second);
    ASSERT_EQ(*res.first, 1);
    ASSERT_FALSE(t.empty());
    ASSERT_EQ(t.size(), 1);
    ASSERT_TRUE(t.rbVerify());

    res = t.insertUnique(2);
    ASSERT_TRUE(res.second);
    ASSERT_EQ(*res.first, 2);
    ASSERT_FALSE(t.empty());
    ASSERT_EQ(t.size(), 2);
    ASSERT_TRUE(t.rbVerify());

    res = t.insertUnique(0);
    ASSERT_TRUE(res.second);
    ASSERT_EQ(*res.first, 0);
    ASSERT_FALSE(t.empty());
    ASSERT_EQ(t.size(), 3);
    ASSERT_TRUE(t.rbVerify());

    res = t.insertUnique(1);
    ASSERT_FALSE(res.second);
    ASSERT_EQ(*res.first, 1);
    ASSERT_FALSE(t.empty());
    ASSERT_EQ(t.size(), 3);
    ASSERT_TRUE(t.rbVerify());

    auto it = t.insertEqual(5);
    ASSERT_EQ(*it, 5);
    ASSERT_EQ(t.size(), 4);
    ASSERT_TRUE(t.rbVerify());

    it = t.insertEqual(1);
    ASSERT_EQ(*it, 1);
    ASSERT_EQ(t.size(), 5);
    ASSERT_TRUE(t.rbVerify());

    // copy constructor
    RBTree t2 = t;
    ASSERT_EQ(t2.size(), 5);
    ASSERT_TRUE(t2.rbVerify());
    t2.insertEqual(10);
    ASSERT_TRUE(t2.rbVerify());
    ASSERT_EQ(t.size(), 5);
    ASSERT_EQ(t2.size(), 6);

    // copy operator
    t2 = t;
    ASSERT_EQ(t2.size(), 5);
    ASSERT_TRUE(t2.rbVerify());
    t2.insertEqual(10);
    ASSERT_TRUE(t2.rbVerify());
    ASSERT_EQ(t.size(), 5);
    ASSERT_EQ(t2.size(), 6);

    t.swap(t2);
    ASSERT_TRUE(t.rbVerify());
    ASSERT_TRUE(t2.rbVerify());
    ASSERT_EQ(t2.size(), 5);
    ASSERT_EQ(t.size(), 6);
    t.swap(t2);
    ASSERT_TRUE(t.rbVerify());
    ASSERT_TRUE(t2.rbVerify());
    ASSERT_EQ(t.size(), 5);
    ASSERT_EQ(t2.size(), 6);

    // iterator
    ASSERT_EQ(tinystl::distance(t.begin(), t.end()), t.size());
    for(auto it = t.begin(); it != t.end();) {
        auto cur = it++;
        if(it != t.end())
            ASSERT_LE(*cur, *it);
    }

    ASSERT_EQ(tinystl::distance(t.cbegin(), t.cend()), t.size());
    for(auto it = t.cbegin(); it != t.cend();) {
        auto cur = it++;
        if(it != t.cend()) {
            ASSERT_LE(*cur, *it);
        }
    }

    ASSERT_EQ(tinystl::distance(t.rbegin(), t.rend()), t.size());
    for(auto it = t.rbegin(); it != t.rend();) {
        auto cur = it++;
        if(it != t.rend()) {
            ASSERT_GE(*cur, *it);
        }
    }

    ASSERT_EQ(tinystl::distance(t.crbegin(), t.crend()), t.size());
    for(auto it = t.crbegin(); it != t.crend();) {
        auto cur = it++;
        if(it != t.crend()) {
            ASSERT_GE(*cur, *it);
        }
    }

    // find
    auto findIt = t.find(0);
    ASSERT_EQ(*findIt, 0);
    findIt = t.find(1);
    ASSERT_EQ(*findIt, 1);
    findIt = t.find(-1);
    ASSERT_TRUE(findIt == t.end());

    findIt = t.lowerBound(1);
    while(findIt != t.end()) {
        ASSERT_GE(*findIt++, 1);
    }

    findIt = t.upperBound(2);
    while(findIt != t.begin()) {
        ASSERT_LE(*--findIt, 2);
    }

    auto range = t.equalRange(0);
    ASSERT_EQ(tinystl::distance(range.first, range.second), 1);
    range = t.equalRange(-1);
    ASSERT_EQ(tinystl::distance(range.first, range.second), 0);
    range = t.equalRange(1);
    ASSERT_EQ(tinystl::distance(range.first, range.second), 2);
    while(range.first != range.second) {
        ASSERT_EQ(*range.first++, 1);
    }

    // erase
    auto beg = t.begin();
    ++beg;
    ++beg;
    ++beg;
    t.erase(beg);
    ASSERT_EQ(t.size(), 4);
    ASSERT_TRUE(t.rbVerify());
    for(auto it = t.cbegin(); it != t.cend(); ++it) {
        ASSERT_NE(*it, 2);
    }

    auto count = t.erase(1);
    ASSERT_TRUE(t.rbVerify());
    ASSERT_EQ(count, 2);
    ASSERT_EQ(t.size(), 2);
    for(auto it = t.cbegin(); it != t.cend(); ++it) {
        ASSERT_NE(*it, 1);
    }

    t.clear();
    ASSERT_TRUE(t.empty());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include "../tinystl/set.h"
#include <gtest/gtest.h>

TEST(Set, constructors) {
    tinystl::Set<int> s;
    ASSERT_TRUE(s.empty());

    int data[] = {1, 2, 3, 4, 1};
    tinystl::Set<int> s2(std::begin(data), std::end(data));
    ASSERT_FALSE(s2.empty());
    ASSERT_EQ(s2.size(), 4);
}

TEST(Set, insert) {
    tinystl::Set<int> s;
    int data[] = {1, 2, 3, 3, 2, 1, 0};
    for(auto it = std::begin(data); it != std::end(data); ++it) {
        s.insert(*it);
    }
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 4);
}

TEST(Set, iterators) {
    int data[] = {1, 2, 3, 3, 2, 1, 0};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    int i = 0;
    for(auto it = s.begin(); it != s.end(); ++it) {
        ASSERT_EQ(*it, i++);
    }
    i = 0;
    for(auto it = s.cbegin(); it != s.cend(); ++it) {
        ASSERT_EQ(*it, i++);
    }
    i = 4;
    for(auto it = s.rbegin(); it != s.rend(); ++it) {
        ASSERT_EQ(*it, --i);
    }
    i = 4;
    for(auto it = s.crbegin(); it != s.crend(); ++it) {
        ASSERT_EQ(*it, --i);
    }
}

TEST(Set, erase) {
    int data[] = {1, 2, 3, 3, 2, 1, 0};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    ASSERT_EQ(s.size(), 4);
    ASSERT_EQ(*s.begin(), 0);
    s.erase(s.begin());
    ASSERT_EQ(s.size(), 3);
    ASSERT_EQ(*s.begin(), 1);
    int count = s.erase(1);
    ASSERT_EQ(s.size(), 2);
    ASSERT_EQ(*s.begin(), 2);
    s.insert(4);
    ASSERT_EQ(s.size(), 3);
    ASSERT_EQ(*s.begin(), 2);
    s.erase(s.begin(), --s.end());
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 1);
}

TEST(Set, clear) {
    tinystl::Set<int> s;
    s.insert(1);
    s.insert(2);
    ASSERT_FALSE(s.empty());
    ASSERT_EQ(s.size(), 2);
    s.clear();
    ASSERT_TRUE(s.empty());
    ASSERT_EQ(s.size(), 0);

    int data[] = {1, 2, 3, 4, 1, 2, 3, 4};
    tinystl::Set<int> s2(std::begin(data), std::end(data));
    ASSERT_FALSE(s2.empty());
    ASSERT_EQ(s2.size(), 4);
    s2.clear();
    ASSERT_TRUE(s2.empty());
    ASSERT_EQ(s2.size(), 0);

    s2.insert(1);
    ASSERT_FALSE(s2.empty());
    ASSERT_EQ(s2.size(), 1);
}

TEST(Set, find) {
    int data[] = {1, 2, 3, 4, 1, 2, 3, 4};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    auto it = s.find(1);
    ASSERT_EQ(*it, 1);
    it = s.find(1231);
    ASSERT_TRUE(it == s.end());
}

TEST(Set, count) {
    int data[] = {1, 2, 3, 4, 1, 2, 3, 4};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    auto count = s.count(1);
    ASSERT_EQ(count, 1);
    count = s.count(-1);
    ASSERT_EQ(count, 0);
}

TEST(Set, equalRange) {
    int data[] = {1, 2, 3, 4, 1, 2, 3, 4};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    auto res = s.equalRange(1);
    ASSERT_EQ(tinystl::distance(res.first, res.second), 1);
    ASSERT_EQ(*res.first, 1);
}

TEST(Set, equalOperator) {
    int data[] = {1, 2, 3, 4, 1, 2, 3, 4};
    tinystl::Set<int> s(std::begin(data), std::end(data));
    tinystl::Set<int> s2(std::begin(data), std::end(data));
    ASSERT_TRUE(s == s2);
    s2.insert(-1);
    ASSERT_TRUE(s > s2);
    ASSERT_TRUE(s >= s2);
    ASSERT_TRUE(s2 < s);
    ASSERT_TRUE(s2 <= s);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

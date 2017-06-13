#include <gtest/gtest.h>
#include <iostream>
#include <deque>
#include "../tinystl/deque.h"

TEST(Deque, constructors) {
    tinystl::Deque<int> d;
    ASSERT_EQ(d.size(), 0);
    tinystl::Deque<int> d2(10, 2);
    ASSERT_EQ(d2.size(), 10);
    tinystl::Deque<int> d3(11);
    ASSERT_EQ(d3.size(), 11);
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d4(std::begin(data), std::end(data));
    ASSERT_EQ(d4.size(), 5);
}

TEST(Deque, assign) {
    tinystl::Deque<int> d;
    d.assign(10, 20);
    auto it = d.begin();
    *it = 20;
    it++;
    for(auto value: d) {
        ASSERT_EQ(value, 20);
    }

    int data[] = {1, 2, 3, 4, 5};
    d.assign(std::begin(data), std::end(data));
    ASSERT_EQ(d.size(), 5);
    for(int i = 0; i < 5; ++i) {
        ASSERT_EQ(d[i], data[i]);
    }
}

TEST(Deque, getItem) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    ASSERT_EQ(d[0], 1);
}

TEST(Deque, at) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    for(int i = 0; i < 5; ++i) {
        ASSERT_EQ(d[i], d.at(i));
    }
    ASSERT_THROW(d.at(7), std::out_of_range);
}

TEST(Deque, frontAndBack) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    ASSERT_EQ(d.front(), d[0]);
    ASSERT_EQ(d.back(), 5);
}

TEST(Deque, beginAndEnd) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    ASSERT_EQ(tinystl::distance(d.begin(), d.end()), 5);
    ASSERT_EQ(tinystl::distance(d.cbegin(), d.cend()), 5);
    ASSERT_EQ(tinystl::distance(d.rbegin(), d.rend()), 5);
    ASSERT_EQ(tinystl::distance(d.crbegin(), d.crend()), 5);

    int i = 0;
    for(auto it = d.begin(); it != d.end(); ++it) {
        ASSERT_EQ(++i, *it);
    }

    for(auto it = d.rbegin(); it != d.rend(); ++it) {
        ASSERT_EQ(i--, *it);
    }
}

TEST(Deque, empty) {
    tinystl::Deque<int> d;
    ASSERT_TRUE(d.empty());
    tinystl::Deque<int> d2(10);
    ASSERT_FALSE(d2.empty());
}

TEST(Deque, clear) {
    tinystl::Deque<int> d;
    d.clear();
    ASSERT_TRUE(d.empty());
    d.assign(10, 2);
    ASSERT_FALSE(d.empty());
    d.clear();
    ASSERT_TRUE(d.empty());
}

template<typename T>
inline bool equal(const tinystl::Deque<T> &lhs, const std::deque<T> &rhs) {
    return lhs.size() == rhs.size() &&
        tinystl::equal(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
}

template<typename T>
inline bool equal(const std::deque<T> &lhs, const tinystl::Deque<T> &rhs) {
    return equal(rhs, lhs);
}

TEST(Deque, insert) {
    tinystl::Deque<int> d;
    d.insert(d.begin(), 10);
    ASSERT_EQ(d.size(), 1);
    ASSERT_EQ(d.front(), 10);
    d.insert(d.begin(), 10, 1);
    ASSERT_EQ(d.size(), 11);

    std::deque<int> sd;
    tinystl::Deque<int> td;
    sd.insert(sd.begin(), 10, 1);
    td.insert(td.begin(), 10, 1);
    ASSERT_TRUE(equal(sd, td));
    sd.insert(sd.begin() + 8, 10, 2);
    td.insert(td.begin() + 8, 10, 2);
    ASSERT_TRUE(equal(sd, td));
    sd.insert(sd.begin() + 2, 10, 3);
    td.insert(td.begin() + 2, 10, 3);
    ASSERT_TRUE(equal(sd, td));

    int data[] = {1, 2, 3, 4, 5};
    sd.insert(sd.begin(), std::begin(data), std::end(data));
    td.insert(td.begin(), std::begin(data), std::end(data));
    ASSERT_TRUE(equal(sd, td));
    sd.insert(sd.begin() + 2, std::begin(data), std::end(data));
    td.insert(td.begin() + 2, std::begin(data), std::end(data));
    ASSERT_TRUE(equal(sd, td));
    sd.insert(sd.end() - 3, std::begin(data), std::end(data));
    td.insert(td.end() - 3, std::begin(data), std::end(data));
    ASSERT_TRUE(equal(sd, td));
}

TEST(Deque, erase) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    d.erase(d.begin());
    ASSERT_EQ(d.size(), 4);
    ASSERT_EQ(d.front(), 2);
    d.erase(d.end() - 1);
    ASSERT_EQ(d.size(), 3);
    ASSERT_EQ(d.back(), 4);
    d.erase(d.begin() + 1);
    ASSERT_EQ(d.size(), 2);
}

TEST(Deque, pushpop) {
    tinystl::Deque<int> d;
    d.pushBack(1);
    ASSERT_EQ(d.size(), 1);
    d.pushFront(2);
    ASSERT_EQ(d.size(), 2);
    ASSERT_EQ(d.front(), 2);
    ASSERT_EQ(d.back(), 1);
    d.popFront();
    ASSERT_EQ(d.size(), 1);
    d.popBack();
    ASSERT_EQ(d.size(), 0);
}

TEST(Deque, resize) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::Deque<int> d(std::begin(data), std::end(data));
    d.resize(10, 0);
    ASSERT_EQ(d.size(), 10);
    ASSERT_EQ(d.front(), 1);
    d.resize(3);
    ASSERT_EQ(d.size(), 3);
    ASSERT_EQ(d.front(), 1);
}

TEST(Deque, swap) {
    tinystl::Deque<int> d;
    tinystl::Deque<int> d2(10, 3);
    ASSERT_TRUE(d.empty());
    d.swap(d2);
    ASSERT_TRUE(d2.empty());
    ASSERT_EQ(d.size(), 10);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

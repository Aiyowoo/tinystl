#include <gtest/gtest.h>
#include <iostream>
#include "../tinystl/list.h"

TEST(Iterator, convert) {
    tinystl::List<int>::Iterator it;
    tinystl::List<int>::ConstIterator cit(it);
    tinystl::List<int>::ConstIterator cit2(it);
    ASSERT_TRUE(cit == cit2);
}

TEST(List, constructors) {
    tinystl::List<int> a;
    ASSERT_TRUE(a.empty());

    tinystl::List<int> b(10, 4);
    ASSERT_EQ(b.size(), 10);
    for(auto it = b.cbegin(); it != b.cend(); ++it) {
        ASSERT_EQ(*it, 4);
    }

    tinystl::List<int> c(10);
    ASSERT_EQ(c.size(), 10);
    for(auto it = c.cbegin(); it != c.cend(); ++it) {
        ASSERT_EQ(*it, int());
    }

    tinystl::List<int> d(b.cbegin(), b.cend());
    ASSERT_EQ(b, d);

    tinystl::List<int> e(d);
    ASSERT_EQ(d, e);
}

TEST(List, assign) {
    tinystl::List<int> a(10, 4);
    tinystl::List<int> b;
    b = a;
    ASSERT_EQ(b, a);

    b.assign(100, 5);
    ASSERT_EQ(b.size(), 100);
    b.assign(a.cbegin(), a.cend());
    ASSERT_EQ(a, b);
    b.assign(10, 4);
    ASSERT_EQ(a, b);
}

TEST(List, ElementAccess) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::List<int> a(data, data + 5);
    ASSERT_EQ(a.front(), 1);
    ASSERT_EQ(a.back(), 5);

    const tinystl::List<int> b(data, data + 5);
    ASSERT_EQ(a.front(), 1);
    ASSERT_EQ(a.back(), 5);
}

TEST(List, iterator) {
    int a[] = {1, 2, 3, 4, 5};
    tinystl::List<int> l(a, a + 5);
    int i = 0;
    auto first = l.begin();
    while(first != l.end()) {
        ASSERT_EQ(*first++, a[i++]);
    }

    i = 0;
    auto it = l.cbegin();
    while(it != l.cend()) {
        ASSERT_EQ(*it++, a[i++]);
    }

    i = 5;
    auto rit = l.rbegin();
    while(rit != l.rend()) {
        ASSERT_EQ(*rit++, a[--i]);
    }

    i = 5;
    auto crit = l.crbegin();
    while(crit != l.crend()) {
        ASSERT_EQ(*crit++, a[--i]);
    }

    ASSERT_FALSE(l.empty());
    ASSERT_TRUE(tinystl::List<int>().empty());
}

TEST(List, pushpop) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::List<int> l;
    for(auto value: data) {
        l.pushBack(value);
    }
    int i = 0;
    ASSERT_EQ(l.size(), 5);
    for(auto value: l) {
        ASSERT_EQ(value, data[i++]);
    }
    l.popBack();
    ASSERT_EQ(l.back(), 4);

    l.pushFront(0);
    ASSERT_EQ(l.front(), 0);

    l.pushFront(-1);
    ASSERT_EQ(l.front(), -1);

    l.popFront();
    ASSERT_EQ(l.front(), 0);
}

TEST(List, insert) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::List<int> l;
    l.insert(l.end(), 1);
    ASSERT_EQ(l.size(), 1);
    ASSERT_EQ(l.front(), l.back());

    l.insert(l.begin(), 2);
    ASSERT_EQ(l.size(), 2);
    ASSERT_EQ(l.front(), 2);

    l.insert(++l.begin(), 2, 3);
    ASSERT_EQ(l.size(), 4);
    int res[] = {2, 3, 3, 1};
    ASSERT_TRUE(tinystl::equal(l.begin(), l.end(), res));

    l.insert(l.begin(), data, data + 5);
    ASSERT_EQ(l.size(), 9);
}

TEST(List, resize) {
    tinystl::List<int> l;
    l.resize(10);
    ASSERT_EQ(l.size(), 10);

    l.resize(1);
    ASSERT_EQ(l.size(), 1);

    l.resize(0);
    ASSERT_EQ(l.size(), 0);

    l.resize(10, 3);
    for(auto value: l) {
        ASSERT_EQ(value, 3);
    }
}

TEST(List, swap) {
    tinystl::List<int> l(10, 2), r;
    l.swap(r);
    ASSERT_EQ(l.size(), 0);
    ASSERT_EQ(r.size(), 10);
    for(auto value: r) {
        ASSERT_EQ(value, 2);
    }
}

TEST(List, merge) {
    int ldata[] = {0, 1, 3, 5, 7, 9};
    int rdata[] = {2, 4, 6, 8, 10};
    tinystl::List<int> l(ldata, ldata + 6);
    tinystl::List<int> r(rdata, rdata + 5);
    l.merge(r);
    ASSERT_EQ(l.size(), 11);
    int i = 0;
    for(auto value: l) {
        ASSERT_EQ(value, i);
        ++i;
    }
}

TEST(List, splice) {
    int ldata[] = {1, 3, 5, 7, 9};
    int rdata[] = {2, 4, 6, 8, 10};
    tinystl::List<int> l(ldata, ldata + 5);
    tinystl::List<int> r(rdata, rdata + 5);
    l.splice(l.begin(), r);
    int res[] = {2, 4, 6, 8, 10, 1, 3, 5, 7, 9};
    int i = 0;
    for(auto value: l) {
        ASSERT_EQ(value, res[i++]);
    }

    auto it = l.begin();
    tinystl::advance(it, 5);
    r.splice(r.begin(), l, l.begin(), it);
    ASSERT_EQ(l.size(), 5);
    ASSERT_EQ(r.size(), 5);
    i = 0;
    for(auto value: l) {
        ASSERT_EQ(value, ldata[i++]);
    }
    i = 0;
    for(auto value: r) {
        ASSERT_EQ(value, rdata[i++]);
    }
}

TEST(List, remove) {
    int data[] = {1, 2, 3, 4, 5, 5, 8, 5};
    tinystl::List<int> l(data, data + sizeof(data) / sizeof(int));
    l.remove(5);
    ASSERT_EQ(l.size(), 5);
    for(auto value: l) {
        ASSERT_NE(value, 5);
    }

    l.removeIf([](int x) {return x < 5;});
    ASSERT_EQ(l.size(), 1);
}

TEST(List, unique) {
    int data[] = {1, 2, 3, 3, 4, 4, 4, 5};
    tinystl::List<int> l(data, data + sizeof(data) / sizeof(int));
    l.unique();
    ASSERT_EQ(l.size(), 5);
    int i = 0;
    for(auto value: l) {
        ASSERT_EQ(value, ++i);
    }
}

TEST(List, reverse) {
    int data[] = {1, 2, 3, 4, 5};
    tinystl::List<int> l(data, data + sizeof(data) / sizeof(int));
    l.reverse();
    int i = 5;
    for(auto value: l) {
        ASSERT_EQ(value, i--);
    }
}

TEST(List, sort) {
    int data[] = {2, 3, 9, 7, 4, 1, 99};
    tinystl::List<int> l(data, data + sizeof(data) / sizeof(int));
    l.sort();
    for(auto first = l.begin(), next = l.begin(); ++next != l.end();) {
        ASSERT_TRUE(*first < *next);
        first = next;
    }
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

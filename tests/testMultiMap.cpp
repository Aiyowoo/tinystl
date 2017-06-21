#include "../tinystl/multimap.h"
#include "../tinystl/pair.h"
#include <gtest/gtest.h>

TEST(Map, simple) {
    tinystl::MultiMap<int, int> m;
    ASSERT_TRUE(m.empty());
    ASSERT_EQ(m.size(), 0);

    int data[] = {0, 1, 2, 3, 4};
    for(auto it = std::begin(data); it != std::end(data); ++it) {
        m.insert(tinystl::makePair(*it, *it));
    }
    ASSERT_EQ(m.size(), 5);

    auto resIter = m.find(1);
    ASSERT_TRUE(resIter != m.end());
    ASSERT_EQ(resIter->first, 1);
    ASSERT_EQ(resIter->second, 1);
    resIter = m.find(-1000);
    ASSERT_TRUE(resIter == m.end());

    resIter = m.find(1);
    m.erase(resIter);
    ASSERT_EQ(m.size(), 4);
    resIter = m.find(1);
    ASSERT_TRUE(resIter == m.end());

    auto r = m.equalRange(2);
    ASSERT_EQ(tinystl::distance(r.first, r.second), 1);
    ASSERT_EQ(r.first->first, 2);

    int count = m.count(2);
    ASSERT_EQ(count, 1);

    tinystl::MultiMap<int, int> mm;
    m.swap(mm);
    ASSERT_TRUE(m.empty());
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

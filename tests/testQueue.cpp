#include <gtest/gtest.h>
#include "../tinystl/queue.h"

TEST(Queue, maintainers) {
    tinystl::Queue<int> q;
    ASSERT_TRUE(q.empty());
    q.push(1);
    ASSERT_EQ(q.size(), 1);
    q.push(2);
    ASSERT_EQ(q.size(), 2);
    ASSERT_EQ(q.front(), 1);
    ASSERT_EQ(q.back(), 2);

    q.pop();
    ASSERT_EQ(q.front(), 2);
    ASSERT_EQ(q.size(), 1);

    tinystl::Queue<int> q2;
    for(auto i: {1, 2, 3, 4, 5, 6}) {
        q2.push(i);
    }

    q = q2;
    ASSERT_EQ(q, q2);
    ASSERT_EQ(q.size(), 6);
    q.pop();
    ASSERT_GE(q, q2);
    ASSERT_GT(q, q2);
    using tinystl::swap;
    swap(q, q2);
    ASSERT_EQ(q.size(), 6);
    ASSERT_EQ(q2.size(), 5);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>
#include "../tinystl/priorityQueue.h"

TEST(PriorityQueue, maintainers) {
    tinystl::PriorityQueue<int> p;
    for(int i: {2, 3, 4, 1, 4, 2}) {
        p.push(i);
    }
    ASSERT_EQ(p.size(), 6);
    ASSERT_EQ(p.top(), 4);
    p.pop();
    ASSERT_EQ(p.top(), 4);
    ASSERT_EQ(p.size(), 5);
    p.pop();
    ASSERT_EQ(p.top(), 3);
    ASSERT_EQ(p.size(), 4);
    p.push(10);
    ASSERT_EQ(p.top(), 10);
    ASSERT_EQ(p.size(), 5);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

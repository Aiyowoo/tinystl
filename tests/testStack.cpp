#include <gtest/gtest.h>
#include "../tinystl/stack.h"

TEST(Stack, stack) {
    tinystl::Stack<int> s;
    ASSERT_TRUE(s.empty());
    s.push(10);
    s.push(20);
    ASSERT_EQ(s.top(), 20);
    ASSERT_EQ(s.size(), 2);
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_EQ(s.top(), 10);
    ASSERT_FALSE(s.empty());
    s.pop();
    ASSERT_TRUE(s.empty());

    tinystl::Stack<int> s2;
    s2.push(2);
    s2.push(1);
    s = s2;
    ASSERT_EQ(s.size(), 2);
    s.pop();
    ASSERT_EQ(s2.size(), 2);

    using tinystl::swap;
    swap(s, s2);
    ASSERT_EQ(s.size(), 2);
    ASSERT_EQ(s2.size(), 1);
}

TEST(Stack, operators) {
    tinystl::Stack<int> s1, s2;
    ASSERT_EQ(s1, s2);
    s1.push(1);
    ASSERT_GT(s1, s2);
    ASSERT_GE(s1, s2);
    s2.push(2);
    ASSERT_LT(s1, s2);
    ASSERT_LE(s1, s2);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <string>
#include <new>
#include <cstdlib>
#include "../tinystl/alloc.h"
#include "../tinystl/construct.h"

TEST(construct, constructInt) {
    using IntAlloc = tinystl::SimpleAlloc<int>;
    int *intPtr = IntAlloc::allocate();
    int *oldPtr = intPtr;
    tinystl::construct(intPtr, 1);
    ASSERT_EQ(oldPtr, intPtr);
    ASSERT_EQ(*intPtr, 1);
    IntAlloc::deallocate(intPtr);

    int *intArray = IntAlloc::allocate(1000);
    int *ptr = intArray;
    for(int i = 0; i < 1000; ++i) {
        tinystl::construct(ptr, i);
        ++ptr;
    }
    ptr = intArray;
    for(int i = 0; i < 1000; ++i) {
        ASSERT_EQ(*ptr++, i);
    }
    tinystl::destroy(intArray, intArray + 1000);
    IntAlloc::deallocate(intArray, 1000);
}

TEST(construct, constructString) {
    using StringAlloc = tinystl::SimpleAlloc<std::string>;
    std::string *ptr = StringAlloc::allocate();
    tinystl::construct(ptr, "hello world!");
    std::string result("hello world!");
    *ptr += "nice to meet you!";
    result += "nice to meet you!";
    ASSERT_EQ(*ptr, result);

    std::string *stringArray = StringAlloc::allocate(1000);
    std::string *end = stringArray + 1000;
    for(auto it = stringArray; it != end; ++it) {
        tinystl::construct(it);
    }
    for(auto it = stringArray; it != end; ++it) {
        ASSERT_EQ(*it, "");
    }

    tinystl::destroy(stringArray, end);
    StringAlloc::deallocate(stringArray, 1000);
    StringAlloc::deallocate(ptr);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

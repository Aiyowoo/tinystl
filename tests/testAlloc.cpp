#include "../tinystl/alloc.h"
#include <gtest/gtest.h>
#include <iostream>
#include <limits>
#include <new>
#include <cstdlib>

TEST(MallocAllocator, littleSize) {
    using Alloc = tinystl::DebugAlloc<tinystl::DefaultAllocator>;
    int *ptr = static_cast<int *>(Alloc::allocate(sizeof(int)));
    ASSERT_NE(ptr, nullptr);
    for(int i = 0; i < 100000; ++i) {
        *ptr = i;
    }
    Alloc::deallocate(ptr, sizeof(int));

    for(std::size_t size = 1; size < 1000; ++size) {
        char *buf = static_cast<char *>(Alloc::allocate(size));
        for(int i = 0; i < size; ++i) {
            buf[i] = 1;
        }
        for(int i = 0; i < size; ++i) {
            ASSERT_EQ(buf[i], 1);
        }
        Alloc::deallocate(buf, size);
    }
}

TEST(MallocAllocator, largeSize) {
    using Alloc = tinystl::DebugAlloc<tinystl::DefaultAllocator>;
    for(std::size_t size = 2 << 20; size < 4 << 20; size += 2 << 20) {
        char *buf = static_cast<char *>(Alloc::allocate(size));
        for(int i = 0; i < 1000; ++i) {
            buf[i] = 2;
        }
        for(int i = 0; i < 1000; ++i) {
            ASSERT_EQ(buf[i], 2);
        }
        Alloc::deallocate(buf, size);
    }
}

TEST(MallocAllocator, reallocate) {
    using Alloc = tinystl::DebugAlloc<tinystl::DefaultAllocator>;
    auto checkCharsAsInt = [](char *temp, int count, int target) {
        for(int i = 0; i < count; ++i) {
            ASSERT_EQ(temp[i], target);
        }
    };
    for(std::size_t i = 10; i < (2 << 10); ++i) {
        char *ptr = static_cast<char *>(Alloc::allocate(i));
        for(int j = 0; j < i; ++j) {
            ptr[j] = 3;
        }

        char *temp = static_cast<char *>(Alloc::reallocate(ptr, i, 2 * i));
        ASSERT_NE(temp, nullptr);
        ptr = temp;
        checkCharsAsInt(ptr, i, 3);

        temp = static_cast<char *>(Alloc::reallocate(ptr, 2 * i, i));
        ASSERT_NE(temp, nullptr);
        ptr = temp;
        checkCharsAsInt(ptr, i, 3);
    }
}

TEST(MallocAllocator, outOfMemory) {
    using Alloc = tinystl::DebugAlloc<tinystl::DefaultAllocator>;
    ASSERT_THROW(Alloc::allocate(std::numeric_limits<long>::max()),
                 std::bad_alloc);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

TEST(SimpleAlloc, allocate) {
    using Alloc = tinystl::SimpleAlloc<int, tinystl::MallocAllocator>;
    int *a = Alloc::allocate();
    int *as = Alloc::allocate(100000);
    ASSERT_NE(a, nullptr);
    ASSERT_NE(a, nullptr);
    *a = 10;
    for(int i = 0; i < 100000; ++i) {
        as[i] = 10;
    }
    ASSERT_EQ(*a, 10);
    Alloc::deallocate(a);
    for(int i = 0; i < 100000; ++i) {
        ASSERT_EQ(as[i], 10);
    }
    Alloc::deallocate(as, 100000);
}

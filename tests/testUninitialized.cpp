#include <gtest/gtest.h>
#include <string>
#include <new>
#include <cstdlib>
#include "../tinystl/uninitialized.h"

TEST(uninitialized, copyPOD) {
    int a[10] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};
    int *b = static_cast<int *>(std::malloc(sizeof(int) * 10));
    int *c = static_cast<int *>(std::malloc(sizeof(int) * 10));

    tinystl::uninitializeCopy(std::begin(a), std::end(a), b);
    std::uninitialized_copy(std::begin(a), std::end(a), c);
    for(std::size_t i = 0; i < sizeof(a) / sizeof(int); ++i) {
        ASSERT_EQ(b[i], c[i]);
    }

    std::free(b);
    std::free(c);
}

TEST(uninitialized, fillPOD) {
    int *b = static_cast<int *>(std::malloc(sizeof(int) * 10));
    int *c = static_cast<int *>(std::malloc(sizeof(int) * 10));

    tinystl::uninitializedFill(b, b + 10, 2);
    std::uninitialized_fill(c, c + 10, 2);

    for(int i = 0; i < 10; ++i) {
        ASSERT_EQ(b[i], c[i]);
    }

    std::free(b);
    std::free(c);
}

TEST(uninitialized, fillNPOD) {
    int *b = static_cast<int *>(std::malloc(sizeof(int) * 10));
    int *c = static_cast<int *>(std::malloc(sizeof(int) * 10));

    tinystl::uninitializedFillN(b, 10, 2);
    std::uninitialized_fill_n(c, 10, 2);

    for(int i = 0; i < 10; ++i) {
        ASSERT_EQ(b[i], c[i]);
    }

    std::free(b);
    std::free(c);
}

int main(int argc, char *argv[])
{
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}

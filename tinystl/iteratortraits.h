#ifndef ITERATORTRAITS_H
#define ITERATORTRAITS_H

#include <cstddef>

namespace tinystl {
    struct InputIteratorTag {};
    struct OuputIteratorTag {};
    // output iterator 与 forward iterator的区别是:
    // 1) foward iterator 支持比较操作:
    //    ForwardIterator a, b;
    //    if a == b: *a == *b is true or a, b不能解引用
    //    if a == b: then ++a == ++b;
    // 2) forward itertor 可能不是可写的
    // 3) Input Iterator可能只能遍历一次
    struct ForwardIteratorTag: public InputIteratorTag {};
    struct BidirectionalIteratorTag: public ForwardIteratorTag {};
    struct RandomAccessIteratorTag: public BidirectionalIteratorTag {};

    template<typename T>
    struct IteratorTraits {
        using IteratorCategory = typename T::IteratorCategory;
        using ValueType = typename T::ValueType;
        using DifferenceType = typename T::DifferenceType;
        using Pointer = typename T::Pointer;
        using Reference = typename T::Reference;
    };

    // 针对指针的特化
    template<typename T>
    struct IteratorTraits<T *> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;
        using Pointer = T*;
        using Reference = T&;
    };

    template<typename T>
    struct IteratorTraits<const T*> {
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;
        using Pointer = const T*;
        using Reference = const T&;
    };
}

#endif

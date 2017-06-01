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

    // 感觉这个是多余的
    // 写到后来发现并不多余,因为会去掉顶层const属性,所以T并不是const的.
    // 而const T* 会表示ConstIterator,所以要求解引用返回的是一个const
    // 所以Reference = const T&
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

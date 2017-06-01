#ifndef CONSTRUCT_H
#define CONSTRUCT_H

#include "typetraits.h"
#include "iteratortraits.h"

namespace tinystl {

    template<typename T1, typename T2>
    inline void construct(T1 *ptr, const T2 &value) {
        new (static_cast<void *>(ptr)) T1(value);
    }

    template<typename T>
    inline void construct(T *ptr) {
        new (static_cast<void *>(ptr)) T;
    }

    template<typename T>
    inline void destroy(T *ptr) {
        ptr->~T();
    }

    template<typename ForwardIterator>
    inline void destroyAux(ForwardIterator beg, ForwardIterator end, TrueType) {}

    template<typename ForwardIterator>
    inline void destroyAux(ForwardIterator beg, ForwardIterator end, FalseType) {
        while(beg != end) {
            destroy(&*beg);
            ++beg;
        }
    }

    template<typename ForwardIterator>
    inline void destroy(ForwardIterator beg, ForwardIterator end) {
        using ValueType = typename IteratorTraits<ForwardIterator>::ValueType;
        destroyAux(beg, end, typename TypeTraits<ValueType>::hasTrivialDestructor());
    }
}

#endif

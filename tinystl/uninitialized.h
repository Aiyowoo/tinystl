#ifndef UNINITIALIZED_H
#define UNINITIALIZED_H

#include "typetraits.h"
#include "iteratortraits.h"
#include "algobase.h"
#include "construct.h"

namespace tinystl {
// ---------------------------------------uninitialized_copy---------------------------------

// 仅能在copy constructor和copy assignment是一样的时候能使用

    template<typename InputIterator, typename ForwardIterator>
    inline ForwardIterator uninitializedCopyAux(InputIterator first,
                                                InputIterator last,
                                                ForwardIterator result,
                                                TrueType) {
        return tinystl::copy(first, last, result);
    }

    template<typename InputIterator, typename ForwardIterator>
    inline ForwardIterator uninitializedCopyAux(InputIterator first,
                                               InputIterator last,
                                               ForwardIterator result,
                                               FalseType) {
        ForwardIterator cur = result;
        try {
            while(first != last) {
                construct(&(*cur), *first);
                ++cur;
                ++first;
            }
        } catch(...) {
            destroy(result, cur);
            throw;
        }
        return cur;
    }

    template<typename InputIterator, typename ForwardIterator>
    inline ForwardIterator uninitializedCopy(InputIterator first, InputIterator last,
                                            ForwardIterator result) {
        return uninitializedCopyAux(first, last, result,
                                   typename TypeTraits<typename IteratorTraits<ForwardIterator>::ValueType>::isPODType());
    }

    // -------------------------------------uninitializedFill----------------------------------

    template<typename ForwardIterator, typename T>
    inline void uninitializedFillAux(ForwardIterator first, ForwardIterator last,
                                     const T &value, TrueType) {
        return fill(first, last, value);
    }

    template<typename ForwardIterator, typename T>
    inline void uninitializedFillAux(ForwardIterator first, ForwardIterator last,
                                     const T &value, FalseType) {
        ForwardIterator cur = first;
        try {
            while(cur != last) {
                construct(&(*cur), value);
                ++cur;
            }
        } catch(...) {
            destroy(first, cur);
            throw;
        }
    }

    template<typename ForwardIterator, typename T>
    inline void uninitializedFill(ForwardIterator first, ForwardIterator last,
                                  const T &value) {
        using ValueType = typename IteratorTraits<ForwardIterator>::ValueType;
        uninitializedFillAux(first, last, value,
                             typename TypeTraits<ValueType>::isPODType());
    }

    // --------------------------------uninitializedFillN----------------------------------

    template<typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator uninitializedFillNAux(ForwardIterator first,
                                              Size count,
                                              const T &value, TrueType) {
        return fillN(first, count, value);
    }

    template<typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator uninitializedFillNAux(ForwardIterator first,
                                              Size count,
                                              const T &value, FalseType) {
        ForwardIterator cur = first;
        try {
            while(count--) {
                construct(&(*cur), value);
                ++cur;
            }
        } catch(...) {
            destroy(first, cur);
            throw;
        }
        return cur;
    }

    template<typename ForwardIterator, typename Size, typename T>
    inline ForwardIterator uninitializedFillN(ForwardIterator first, Size count,
                                              const T &value) {
        using ValueType = typename IteratorTraits<ForwardIterator>::ValueType;
        return uninitializedFillNAux(first, count, value,
                                     typename TypeTraits<ValueType>::isPODType());
    }
}

#endif

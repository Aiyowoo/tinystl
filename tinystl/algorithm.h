#ifndef ALGORITHM_H
#define ALGORITHM_H

#include "iteratortraits.h"
#include "iteratorbase.h"
#include "algobase.h"

namespace tinystl {

    // ----------------------------------------------------------------------
    // heap

    template<typename RandomIterator, typename DifferenceType>
    void _adjustHeap(RandomIterator first, DifferenceType parent,
                     DifferenceType len) {
        DifferenceType greaterChild = 2 * parent + 2;
        while(greaterChild < len) {
            if(*(first + greaterChild) < *(first + (greaterChild - 1))) {
                --greaterChild;
            }
            if(*(first + parent) < *(first + greaterChild)) {
                swap(*(first + parent), *(first + greaterChild));
                parent = greaterChild;
                greaterChild = 2 * (parent + 1);
            } else {
                break;
            }
        }
        if(greaterChild == len) {
            --greaterChild;
            if(*(first + parent) < *(first + greaterChild)) {
                swap(*(first + parent), *(first + greaterChild));
            }
        }
    }

    template<typename RandomIterator, typename DifferenceType,
             typename Compare>
    void _adjustHeap(RandomIterator first, DifferenceType parent,
                     DifferenceType len, Compare comp) {
        DifferenceType greaterChild = 2 * parent + 2;
        while(greaterChild < len) {
            if(comp(*(first + greaterChild), *(first + (greaterChild - 1)))) {
                --greaterChild;
            }
            if(comp(*(first + parent), *(first + greaterChild))) {
                swap(*(first + parent), *(first + greaterChild));
                parent = greaterChild;
                greaterChild = 2 * (parent + 1);
            } else {
                break;
            }
        }
        if(greaterChild == len) {
            --greaterChild;
            if(comp(*(first + parent), *(first + greaterChild))) {
                swap(*(first + parent), *(first + greaterChild));
            }
        }
    }

    // makeHeap 建的是最大堆
    template<typename RandomIterator>
    void makeHeap(RandomIterator first, RandomIterator last) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        DifferenceType parent = len / 2;
        while(true) {
            _adjustHeap(first, parent, len);
            if(parent == 0) {
                break;
            }
            --parent;
        }
    }

    template<typename RandomIterator, typename Compare>
    void makeHeap(RandomIterator first, RandomIterator last,
                  Compare comp) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        DifferenceType parent = len / 2;
        while(true) {
            _adjustHeap(first, parent, len, comp);
            if(parent == 0) {
                break;
            }
            --parent;
        }
    }

    template<typename RandomIterator>
    bool isHeap(RandomIterator first, RandomIterator last) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        DifferenceType parent = 0;
        DifferenceType secondChild = parent * 2 + 2;
        while(secondChild < len) {
            if(*(first + secondChild) < *(first + (secondChild - 1))) {
                --secondChild;
            }
            if(*(first + parent) < *(first + secondChild)) {
                return false;
            }
            ++parent;
            secondChild = 2 * (parent + 1);
        }
        if(secondChild == len) {
            if(*(first + parent) < *(first + (secondChild - 1))) {
                return false;
            }
        }
        return true;
    }

    template<typename RandomIterator, typename Compare>
    inline bool isHeap(RandomIterator first, RandomIterator last,
                       Compare comp) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        DifferenceType parent = 0;
        DifferenceType secondChild = parent * 2 + 2;
        while(secondChild < len) {
            if(comp(*(first + secondChild), *(first + (secondChild - 1)))) {
                --secondChild;
            }
            if(comp(*(first + parent) < *(first + secondChild))) {
                return false;
            }
            ++parent;
            secondChild = 2 * (parent + 1);
        }
        if(secondChild == len) {
            if(comp(*(first + parent), *(first + (secondChild - 1)))) {
                return false;
            }
        }
        return true;
    }

    template<typename RandomIterator>
    inline void popHeap(RandomIterator first, RandomIterator last) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        swap(*first, *(last - 1));
        DifferenceType len = tinystl::distance(first, last);
        _adjustHeap(first, static_cast<DifferenceType>(0), len - 1);
    }

    template<typename RandomIterator, typename Compare>
    inline void popHeap(RandomIterator first, RandomIterator last,
                        Compare comp) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        swap(*first, *(last - 1));
        DifferenceType len = tinystl::distance(first, last);
        _adjustHeap(first, static_cast<DifferenceType>(0), len - 1, comp);
    }

    template<typename RandomIterator>
    inline void pushHeap(RandomIterator first, RandomIterator last) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        if(len <= 1) {
            return;
        }
        DifferenceType cur = len - 1;
        DifferenceType parent = (cur - 1) / 2;
        while(cur) {
            if(*(first + parent) < *(first + cur)) {
                swap(*(first + parent), *(first + cur));
                cur = parent;
                parent = (cur - 1) / 2;
            } else {
                break;
            }
        }
    }

    template<typename RandomIterator, typename Compare>
    inline void pushHeap(RandomIterator first, RandomIterator last,
                         Compare comp) {
        using DifferenceType = typename IteratorTraits<RandomIterator>::DifferenceType;
        DifferenceType len = tinystl::distance(first, last);
        if(len <= 1) {
            return;
        }
        DifferenceType cur = len - 1;
        DifferenceType parent = (cur - 1) / 2;
        while(cur) {
            if(comp(*(first + parent), *(first + cur))) {
                swap(*(first + parent), *(first + cur));
                cur = parent;
                parent = (cur - 1) / 2;
            } else {
                break;
            }
        }
    }

    template<typename RandomIterator>
    inline void sortHeap(RandomIterator first, RandomIterator last) {
        while(first != last) {
            popHeap(first, last);
            --last;
        }
    }

    template<typename RandomIterator, typename Compare>
    inline void sortHeap(RandomIterator first, RandomIterator last,
                         Compare comp) {
        while(first != last) {
            popHeap(first, last, comp);
            --last;
        }
    }

}

#endif

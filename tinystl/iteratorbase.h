#ifndef ITERATORBASE_H
#define ITERATORBASE_H

#include "iteratortraits.h"

namespace tinystl {
    template<typename InputIterator>
    inline typename IteratorTraits<InputIterator>::DifferenceType
    distanceAux(InputIterator first, InputIterator last,
                InputIteratorTag) {
        typename InputIterator::DifferenceType n = 0;
        while(first != last) {
            ++first;
            ++n;
        }
        return n;
    }

    template<typename RandomAccessIterator>
    inline typename IteratorTraits<RandomAccessIterator>::DifferenceType
    distanceAux(RandomAccessIterator first, RandomAccessIterator last,
                RandomAccessIteratorTag) {
        return last - first;
    }

    template<typename Iterator>
    inline typename IteratorTraits<Iterator>::DifferenceType
    distance(Iterator first, Iterator last) {
        return distanceAux(first, last, typename IteratorTraits<Iterator>::IteratorCategory());
    }

    template<typename InputIterator, typename Distance>
    void advanceAux(InputIterator &iter, Distance n, InputIteratorTag) {
        while(n--) {
            ++iter;
        }
    }

    template<typename BidirectionalIterator, typename Distance>
    void advanceAux(BidirectionalIterator &iter, Distance n, BidirectionalIteratorTag) {
        if(n > 0) {
            while(n--) {
                ++iter;
            }
        } else {
            while(n++) {
                --iter;
            }
        }
    }

    template<typename RandomAccessIterator, typename Distance>
    void advanceAux(RandomAccessIterator &iter, Distance n,
                    RandomAccessIteratorTag) {
        iter += n;
    }

    template<typename Iterator, typename Distance>
    void advance(Iterator &iter, Distance n) {
        advanceAux(iter, n, typename IteratorTraits<Iterator>::IteratorCategory());
    }
}

#endif

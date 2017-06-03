#ifndef ALGOBASE_H
#define ALGOBASE_H

#include <cstring>
#include "iteratortraits.h"
#include "typetraits.h"

namespace tinystl {
    // --------------------------------------------------------------------------------
    // swap
    template<typename T>
    inline void swap(T &lhs, T &rhs) {
        T temp = lhs;
        lhs = rhs;
        rhs = temp;
    }

    // --------------------------------------------------------------------------------
    // max
    template<typename T>
    inline const T& max(const T &lhs, const T &rhs) {
        return lhs < rhs? rhs: lhs;
    }

    // --------------------------------------------------------------------------------
    // min
    template<typename T>
    inline const T& min(const T &lhs, const T &rhs) {
        return lhs < rhs? lhs: rhs;
    }

    // --------------------------------------------------------------------------------
    // compare
    template<typename InputIterator1, typename InputIterator2>
    inline bool equal(InputIterator1 first1, InputIterator1 last1,
               InputIterator2 first2) {
        while(first1 != last1) {
            if(*first1  != *first2) {
                return false;
            }
            ++first1;
            ++first2;
        }
        return true;
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool less(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2) {
        while(first1 != last1) {
            if(!(*first1 < *first2)) {
                return false;
            }
            ++first1;
            ++first2;
        }
        return true;
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool greater(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2) {
        while(first1 != last1) {
            if(*first1 < *first2) {
                return false;
            }
        }
        return true;
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool lessEqual(InputIterator1 first1, InputIterator1 last1,
                          InputIterator2 first2) {
        return !greater(first1, last1, first2);
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool greaterEqual(InputIterator1 first1, InputIterator1 last1,
                             InputIterator2 first2) {
        return !less(first1, last1, first2);
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool equal(InputIterator1 first1, InputIterator1 last1,
                      InputIterator2 first2, InputIterator2 last2) {
        while(first1 != last1 && first2 != last2) {
            if(*first1 != *first2) {
                return false;
            }
            ++first1;
            ++first2;
        }
        if(first1 != last1 || first2 != last2) {
            return false;
        }
        return true;
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool less(InputIterator1 first1, InputIterator1 last1,
                     InputIterator2 first2, InputIterator2 last2) {
        while(first1 != last1 && first2 != last2) {
            if(*first1 < *first2) {
                return true;
            } else if(*first1 > *first2) {
                return false;
            }
            ++first1;
            ++first2;
        }

        return first1 == last1 && first2 != last2;
    }

    template<typename InputIterator1, typename InputIterator2>
    inline bool greater(InputIterator1 first1, InputIterator1 last1,
                        InputIterator2 first2, InputIterator2 last2) {
        while(first1 != last1 && first2 != last2) {
            if(*first1 > *first2) {
                return true;
            }
            if(*first1 < *first2) {
                return false;
            }
            ++first1;
            ++first2;
        }
        return first1 != last1 && first2 == last2;
    }

    // ---------------------------------------copy begin---------------------------------------
    // 只有指针才能使用memmove来做copy
    template<typename RandomAccessIterator, typename ForwardIterator>
    inline ForwardIterator copyAux(RandomAccessIterator first,
                                  RandomAccessIterator last,
                                  ForwardIterator result,
                                  RandomAccessIteratorTag) {
        using Distance = typename IteratorTraits<RandomAccessIterator>::DifferenceType;
        Distance n = last - first;
        while(n--) {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    template<typename InputIterator, typename ForwardIterator>
    inline ForwardIterator copyAux(InputIterator first, InputIterator last,
                                  ForwardIterator result, InputIteratorTag) {
        while(first != last) {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    template<typename InputIterator, typename ForwardIterator>
    inline ForwardIterator copy(InputIterator first, InputIterator last,
                                ForwardIterator result) {
        return copyAux(first, last, result,
                       typename IteratorTraits<InputIterator>::IteratorCategory());
    }

    template<typename T>
    inline T* copyAux(const T *first, const T *last, T *result, TrueType) {
        std::memmove(result, first, sizeof(T) * (last - first));
        return result + (last - first);
    }

    template<typename T>
    inline T* copyAux(const T *first, const T *last, T *result, FalseType) {
        return copyAux(first, last, result,
                       typename IteratorTraits<T *>::IteratorCategory());
    }

    template<typename T>
    inline T* copy(const T *first, const T *last, T *result) {
        using ValueType = typename IteratorTraits<T *>::ValueType;
        return copyAux(first, last, result,
                       typename TypeTraits<ValueType>::hasTrivialAssignmentOperator());
    }

    // ------------------------------------copy end--------------------------------------

    // ------------------------------------copy n begin----------------------------------

    template<typename InputIterator, typename Size, typename OutputIterator>
    inline OutputIterator copyNAux(InputIterator first, Size count,
                                   OutputIterator result) {
        while(count--) {
            *result = *first;
            ++result;
            ++first;
        }
        return result;
    }

    template<typename InputIterator, typename Size, typename OutputIterator>
    inline OutputIterator copyN(InputIterator first, Size count, OutputIterator result) {
        return copyNAux(first, count, result);
    }

    template<typename T, typename Size>
    inline T* copyNAux(const T *first, Size count, T *result, TrueType) {
        std::memmove(result, first, sizeof(T) * count);
        return result + count;
    }

    template<typename T, typename Size>
    inline T* copyNAux(const T* first, Size count, T *result, FalseType) {
        return copyNAux(first, count, result);
    }

    template<typename T, typename Size>
    inline T* copyN(const T *first, Size count, T *result) {
        using ValueType = typename IteratorTraits<T *>::ValueType;
        return copyNAux(first, count, result,
                        typename TypeTraits<ValueType>::hasTrivialDestructor());
    }

    // ------------------------------------copy n end------------------------------------------

    // ------------------------------------fill begin------------------------------------------

    // 普通模板
    template<typename ForwardIterator, typename T>
    inline void fill(ForwardIterator first, ForwardIterator last, const T &value) {
        while(first != last) {
            *first = value;
            ++first;
        }
    }

    template<>
    inline void fill(char *first, char *last, const char &value) {
        std::memset(first, value, last - first);
    }

    template<>
    inline void fill(signed char *first, signed char *last, const signed char &value) {
        std::memset(first, value, last - first);
    }

    template<>
    inline void fill(unsigned char *first, unsigned char *last, const unsigned char &value) {
        std::memset(first, value, last - first);
    }

    // ------------------------------------fill end--------------------------------------------

    // ------------------------------------fillN start-----------------------------------------

    template<typename OutputIterator, typename Size, typename T>
    inline OutputIterator fillN(OutputIterator first, Size count, const T &value) {
        while(count--) {
            *first = value;
            ++first;
        }
        return first;
    }

    template<typename Size>
    inline char* fillN(char *first, Size count, const char &value) {
        fill(first, first + count, value);
        return first + count;
    }

    template<typename Size>
    inline signed char* fillN(signed char *first, Size count, const signed char &value) {
        fill(first, first + count, value);
        return first + count;
    }

    template<typename Size>
    inline unsigned char* fillN(unsigned char *first, Size count, const unsigned char &value) {
        fill(first, first + count, value);
        return first + count;
    }

    // ------------------------------------fillN end-------------------------------------------

    // -----------------------------copyBackward-------------------------------

    template<typename BidirectionalIterator1, typename BidirectionalIterator2>
    inline BidirectionalIterator2 copyBackwardAux(BidirectionalIterator1 first,
                                                  BidirectionalIterator1 last,
                                                  BidirectionalIterator2 result,
                                                  BidirectionalIteratorTag) {
        while(first != last) {
            *--result = *--last;
        }
        return result;
    }

    template<typename RandomAccessIterator, typename BidirectionalIterator>
    inline BidirectionalIterator copyBackwardAux(RandomAccessIterator first,
                                                 RandomAccessIterator last,
                                                 BidirectionalIterator result,
                                                 RandomAccessIteratorTag) {
        auto n = last - first;
        while(n--) {
            *--result = *--last;
        }
        return result;
    }

    template<typename BidirectionalIterator1, typename BidirectionalIterator2>
    inline BidirectionalIterator2 copyBackward(BidirectionalIterator1 first,
                                               BidirectionalIterator1 last,
                                               BidirectionalIterator2 result) {
        using IteratorCategory = typename IteratorTraits<BidirectionalIterator1>::IteratorCategory;
        return copyBackwardAux(first, last, result, IteratorCategory());
    }
}

#endif

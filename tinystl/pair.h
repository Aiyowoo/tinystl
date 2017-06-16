#ifndef PAIR_H
#define PAIR_H

#include "algobase.h"

namespace tinystl {

    template<typename T1, typename T2>
    struct Pair {
        using FirstType = T1;
        using SecondType = T2;

        Pair(): first(T1()), second(T2()) {}
        Pair(const T1 &_first, const T2 &_second)
            : first(_first), second(_second) {}

        void swap(Pair<FirstType, SecondType> &other) {
            using tinystl::swap;
            swap(first, other->first);
            swap(second, other->second);
        }

        FirstType first;
        SecondType second;
    };

    template<typename T1, typename T2>
    void swap(Pair<T1, T2> &lhs, Pair<T1, T2> &rhs) {
        lhs.swap(rhs);
    }

    template<typename T1, typename T2>
    inline bool operator==(const Pair<T1, T2> &lhs,
                           const Pair<T1, T2> &rhs) {
        return lhs.first == rhs.first && lhs.second == rhs.second;
    }

    template<typename T1, typename T2>
    inline bool operator!=(const Pair<T1, T2> &lhs,
                           const Pair<T1, T2> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T1, typename T2>
    inline bool operator<(const Pair<T1, T2> &lhs,
                          const Pair<T1, T2> &rhs) {
        return lhs.first < rhs.first ||
                           (lhs.first == rhs.first && lhs.second < rhs.second);
    }

    template<typename T1, typename T2>
    inline bool operator>(const Pair<T1, T2> &lhs,
                          const Pair<T1, T2> &rhs) {
        return lhs.first > rhs.first ||
            (lhs.first == rhs.first && lhs.second > rhs.second);
    }

    template<typename T1, typename T2>
    inline bool operator<=(const Pair<T1, T2> &lhs,
                           const Pair<T1, T2> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T1, typename T2>
    inline bool operator>=(const Pair<T1, T2> &lhs,
                           const Pair<T1, T2> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T1, typename T2>
    Pair<T1, T2> makePair(const T1 &first, const T2 &second) {
        return Pair<T1, T2>(first, second);
    }

}

#endif

#ifndef ITERATOR_H
#define ITERATOR_H

#include "iteratortraits.h"

// ConstIterator 和 Iterator的区别只是Reference和Pointer的类型不同,所以可以用同一个ReverseIterator

namespace tinystl {

    template<typename Iterator>
    class ReverseIterator {
    public:
        using IteratorCategory = typename IteratorTraits<Iterator>::IteratorCategory;
        using ValueType = typename IteratorTraits<Iterator>::ValueType;
        using DifferenceType = typename IteratorTraits<Iterator>::DifferenceType;
        using Pointer = typename IteratorTraits<Iterator>::Pointer;
        using Reference = typename IteratorTraits<Iterator>::Reference;

        using Self = ReverseIterator<Iterator>;

    public:
        ReverseIterator() = default;
        explicit ReverseIterator(Iterator iter): current(iter) {}
        ReverseIterator(const Self &other): current(other.current) {}

        template<typename IteratorType>
        ReverseIterator(const ReverseIterator<IteratorType> &other)
            :current(other.base()) {}

        Iterator base() const {
            return current;
        }

        Reference operator*() const {
            Iterator temp = current;
            return *--temp;
        }

        Pointer operator->() const {
            return &(operator*());
        }

        Self& operator++() {
            --current;
            return *this;
        }

        Self operator++(int) {
            Self temp = *this;
            --current;
            return temp;
        }

        Self& operator--() {
            ++current;
            return *this;
        }

        Self operator--(int) {
            Self temp = *this;
            ++current;
            return temp;
        }

        Self operator+(DifferenceType n) const {
            return Self(current - n);
        }

        Self operator-(DifferenceType n) const {
            return Self(current + n);
        }

        Self& operator+=(DifferenceType n) {
            current -= n;
            return *this;
        }

        Self& operator-=(DifferenceType n) {
            current += n;
            return *this;
        }

        Reference operator[](DifferenceType n) {
            return *(*this + n);
        }

    private:
        Iterator current;
    };

    template<typename Iterator>
    inline bool operator==(const ReverseIterator<Iterator> &lhs,
                           const ReverseIterator<Iterator> &rhs) {
        return lhs.base() == rhs.base();
    }

    template<typename Iterator>
    inline bool operator!=(const ReverseIterator<Iterator> &lhs,
                           const ReverseIterator<Iterator> &rhs) {
        return !(lhs == rhs);
    }

    template<typename Iterator>
    inline bool operator<(const ReverseIterator<Iterator> &lhs,
                          const ReverseIterator<Iterator> &rhs) {
        return lhs.base() > rhs.base();
    }

    template<typename Iterator>
    inline bool operator>(const ReverseIterator<Iterator> &lhs,
                          const ReverseIterator<Iterator> &rhs) {
        return lhs.base() < rhs.base();
    }

    template<typename Iterator>
    inline bool operator<=(const ReverseIterator<Iterator> &lhs,
                           const ReverseIterator<Iterator> &rhs) {
        return lhs < rhs || lhs == rhs;
    }

    template<typename Iterator>
    inline bool operator>=(const ReverseIterator<Iterator> &lhs,
                           const ReverseIterator<Iterator> &rhs) {
        return lhs > rhs || lhs == rhs;
    }

    template<typename Iterator>
    inline typename IteratorTraits<Iterator>::DifferenceType
    operator-(const ReverseIterator<Iterator> &lhs,
              const ReverseIterator<Iterator> &rhs) {
        return rhs.base() - lhs.base();
    }

    template<typename Iterator>
    inline ReverseIterator<Iterator>
    operator+(typename IteratorTraits<Iterator>::DifferenceType n,
              const ReverseIterator<Iterator> &iter) {
        return iter - n;
    }

}

#endif

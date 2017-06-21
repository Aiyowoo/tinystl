#ifndef MAP_H
#define MAP_H

#include <stdexcept>
#include "rbtree.h"
#include "algobase.h"
#include "alloc.h"
#include "pair.h"

namespace tinystl {

    template<typename Key, typename T, typename Compare=Less<Key>,
             typename _Alloc=Alloc>
    class Map {
    public:
        using KeyType = Key;
        using MappedType = T;
        using ValueType = Pair<KeyType, MappedType>;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
        using KeyCompare = Compare;

        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;

    protected:
        struct _KeyOfValue {
            const KeyType& operator()(const ValueType &value) const {
                return value.first;
            }
        };
        using _Container = RBTree<KeyType, ValueType, _KeyOfValue,
                                  Compare, _Alloc>;

    private:
        using __Self = Map<Key, T, Compare, _Alloc>;

    public:
        using Iterator = typename _Container::Iterator;
        using ConstIterator = typename _Container::ConstIterator;
        using ReverseIterator = typename _Container::ReverseIterator;
        using ConstReverseIterator = typename _Container::ConstReverseIterator;

        Map() = default;
        Map(const Compare &compare): __container(compare) {}
        template<typename InputIterator>
        Map(InputIterator first, InputIterator second, const Compare &compare=Compare()) {
            __container.insert(first, second);
        }
        Map(const __Self&) = default;
        __Self& operator=(const __Self&) = default;

        MappedType& at(const KeyType &key) {
            Iterator it = __container.find(key);
            _rangeCheck(it);
            return it->second;
        }

        const MappedType& at(const KeyType &key) const {
            ConstIterator it = __container.find(key);
            _rangeCheck(it);
            return it->second;
        }

        MappedType& operator[](const KeyType &key) {
            Iterator it = find(key);
            if(it == end()) {
                it = insert(makePair<KeyType, MappedType>(key, MappedType())).first;
            }
            return it->second;
        }

        Iterator begin() { return __container.begin(); }
        ConstIterator begin() const { return __container.begin(); }
        ConstIterator cbegin() const { return __container.cbegin(); }
        Iterator end() { return __container.end(); }
        ConstIterator end() const { return __container.end(); }
        ConstIterator cend() const { return __container.cend(); }
        ReverseIterator rbegin() { return __container.rbegin(); }
        ConstReverseIterator rbegin() const { return __container.rbegin(); }
        ConstReverseIterator crbegin() const { return __container.crbegin(); }
        ReverseIterator rend() { return __container.rend(); }
        ConstReverseIterator rend() const { return __container.rend(); }
        ConstReverseIterator crend() const { return __container.crend(); }

        bool empty() const { return __container.empty(); }
        SizeType size() const { return __container.size(); }
        SizeType maxSize() const { return __container.maxSize(); }

        void clear() { __container.clear(); }

        Pair<Iterator, bool> insert(const ValueType &value) {
            return __container.insertUnique(value);
        }
        Iterator insert(Iterator hint, const ValueType &value) {
            return __container.insertUnique(hint, value);
        }
        template<typename InputIterator>
        void insert(InputIterator first, InputIterator last) {
            __container.insertUnique(first, last);
        }

        void erase(Iterator pos) { __container.erase(pos); }
        void erase(Iterator first, Iterator last) { __container.erase(first, last); }

        SizeType erase(const KeyType &key) { return __container.erase(key); }

        void swap(__Self &other) {
            using tinystl::swap;
            swap(__container, other.__container);
        }

        SizeType count(const KeyType &key) const {
            return __container.count(key);
        }
        Iterator find(const KeyType &key) {
            return __container.find(key);
        }
        ConstIterator find(const KeyType &key) const {
            return __container.find(key);
        }

        Iterator lowBound(const KeyType &key) {
            return __container.lowBound(key);
        }
        ConstIterator lowBound(const KeyType &key) const {
            return __container.lowBound(key);
        }

        Iterator upperBound(const KeyType &key) {
            return __container.upperBound(key);
        }
        ConstIterator upperBound(const KeyType &key) const {
            return __container.upperBound(key);
        }

        Pair<Iterator, Iterator> equalRange(const KeyType &key) {
            return __container.equalRange(key);
        }
        Pair<ConstIterator, ConstIterator> equalRange(const KeyType &key) const {
            return __container.equalRange(key);
        }

        template<typename Key1, typename T1, typename Compare1, typename _Alloc1>
        friend bool operator==(const Map<Key1, T1, Compare1, _Alloc1> &lhs,
                               const Map<Key1, T1, Compare1, _Alloc1> &rhs) ;

        template<typename Key1, typename T1, typename Compare1, typename _Alloc1>
        friend bool operator<(const Map<Key1, T1, Compare1, _Alloc1> &lhs,
                              const Map<Key1, T1, Compare1, _Alloc1> &rhs);

    protected:
        void _rangeCheck(Iterator it) const {
            if(it == end()) {
                throw std::out_of_range("map");
            }
        }

        void _rangeCheck(ConstIterator it) const {
            if(it == cend()) {
                throw std::out_of_range("map");
            }
        }

    private:
        _Container __container;
    };

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator==(const Map<Key, T, Compare, _Alloc> &lhs,
                           const Map<Key, T, Compare, _Alloc> &rhs) {
        return lhs.__container == rhs.__container;
    }

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator!=(const Map<Key, T, Compare, _Alloc> &lhs,
                           const Map<Key, T, Compare, _Alloc> &rhs) {
        return !(lhs == rhs);
    }

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator<(const Map<Key, T, Compare, _Alloc> &lhs,
                          const Map<Key, T, Compare, _Alloc> &rhs) {
        return lhs.__container < rhs.__container;
    }

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator>(const Map<Key, T, Compare, _Alloc> &lhs,
                          const Map<Key, T, Compare, _Alloc> &rhs) {
        return rhs < lhs;
    }

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator<=(const Map<Key, T, Compare, _Alloc> &lhs,
                           const Map<Key, T, Compare, _Alloc> &rhs) {
        return !(lhs > rhs);
    }

    template<typename Key, typename T, typename Compare, typename _Alloc>
    inline bool operator>=(const Map<Key, T, Compare, _Alloc> &lhs,
                           const Map<Key, T, Compare, _Alloc> &rhs) {
        return !(lhs < rhs);
    }

}

#endif

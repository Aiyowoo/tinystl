#ifndef DEQUE_H
#define DEQUE_H

#include <stdexcept>
#include "alloc.h"
#include "iteratortraits.h"
#include "algobase.h"
#include "typetraits.h"
#include "uninitialized.h"

namespace tinystl {
    enum { BUFFER_SIZE = 512 };
    constexpr std::size_t bufferSize(const std::size_t objSize) {
        return objSize < BUFFER_SIZE? BUFFER_SIZE / objSize: static_cast<std::size_t>(1);
    }

    template<typename T, typename Ref, typename PointerType>
    class DequeIterator {
    public:
        using IteratorCategory = RandomAccessIteratorTag;
        using ValueType = T;
        using Reference = Ref;
        using Pointer = PointerType;
        using DifferenceType = std::ptrdiff_t;

    protected:
        using _MapPointer = T**;
        using _Self = DequeIterator<T, Ref, PointerType>;

    public:
        DequeIterator(): __node(nullptr), __first(nullptr),
                         __cur(nullptr), __last(__first + bufferSize(sizeof(T))) {}
        // 道理与ListIterator相同
        DequeIterator(const DequeIterator<T, typename RemoveConst<Ref>::ResultType,
                      typename RemoveConst<PointerType>::ResultType> &other)
            : __node(other.__node), __first(other.__first),
              __cur(other.__cur), __last(other.__last) {}
        DequeIterator(_MapPointer node, T *first, T *cur, T *last)
            : __node(node), __first(first), __cur(cur), __last(last) {}

        _Self& operator=(const DequeIterator<T, typename RemoveConst<Ref>::ResultType,
                         typename RemoveConst<PointerType>::ResultType> &other) {
            __node = other.__node;
            __first = other.__first;
            __cur = other.__cur;
            __last = other.__last;
        }

        _Self& operator++() {
            ++__cur;
            if(__cur == __last) {
                _setMapNode(__node + 1);
                __cur = __first;
            }
        }

        _Self operator++(int) {
            _Self temp = *this;
            operator++();
            return temp;
        }

        _Self operator--() {
            if(__cur == __first) {
                _setMapNode(__node - 1);
                __cur = __last;
            }
            --__cur;
            return *this;
        }

        _Self operator--(int) {
            _Self temp = *this;
            operator--();
            return temp;
        }

        _Self& operator+=(DifferenceType n) {
            const DifferenceType restOfCurrentNode = __last - __cur;
            if(n < restOfCurrentNode) {
                __cur += n;
            } else {
                n -= restOfCurrentNode;
                _setMapNode(__node + 1 + (n / bufferSize(sizeof(ValueType))));
                __cur = __first + (n % bufferSize(sizeof(ValueType)));
            }
            return *this;
        }

        _Self operator+(DifferenceType n) {
            _Self temp = *this;
            return temp += n;
        }

        _Self& operator-=(DifferenceType n) {
            const DifferenceType restOfCurrentNode = __cur - __first;
            if(n <= restOfCurrentNode) {
                __cur -= n;
            } else {
                n -= restOfCurrentNode + 1;
                _setMapNode(__node - 1 - (n / bufferSize(sizeof(ValueType))));
                __cur = __last - 1 - (n % bufferSize(sizeof(ValueType)));
            }
            return *this;
        }

        _Self operator-(DifferenceType n) {
            _Self temp = *this;
            return temp -= n;
        }

        Reference operator*() const {
            return *__cur;
        }

        Pointer operator->() const {
            return __cur;
        }

        DequeIterator<T, typename RemoveConst<Ref>::ResultType,
                      typename RemoveConst<PointerType>::ResultType>
        removeConst() const {
            return DequeIterator<T, typename RemoveConst<Ref>::ResultType,
                                 typename RemoveConst<PointerType>::ResultType>(__node, __first, __cur, __last);
        }

        void _setMapNode(_MapPointer newNode) {
            __node = newNode;
            __first = *__node;
            __last = __first + bufferSize(sizeof(T));
        }

        _MapPointer __node;
        T *__first;
        T *__cur;
        T *__last;
    };

    template<typename T, typename Ref, typename PointerType>
    inline DequeIterator<T, Ref, PointerType> operator+(typename DequeIterator<T, Ref, PointerType>::DifferenceType n,
                                                        const DequeIterator<T, Ref, PointerType> &iter) {
        return iter + n;
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline typename DequeIterator<T, LRef, LPointerType>::DifferenceType
    operator-(const DequeIterator<T, LRef, LPointerType> &lhs,
              const DequeIterator<T, RRef, RPointerType> &rhs) {
        typename DequeIterator<T, LRef, LPointerType>::DifferenceType dis =
            (lhs.__node - rhs.__node - 1) * bufferSize(sizeof(T));
        dis += lhs.__cur - lhs.__first;
        dis += rhs.__last - rhs.__cur;
        return dis;
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator==(const DequeIterator<T, LRef, LPointerType> &lhs,
                           const DequeIterator<T, RRef, RPointerType> &rhs) {
        return lhs.__cur == rhs.__cur;
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator!=(const DequeIterator<T, LRef, LPointerType> &lhs,
                           const DequeIterator<T, RRef, RPointerType> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator<(const DequeIterator<T, LRef, LPointerType> &lhs,
                          const DequeIterator<T, RRef, RPointerType> &rhs) {
        return lhs.__node < rhs.__node ||
                            (lhs.__node == rhs.__node && lhs.__cur < rhs.__cur);
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator>(const DequeIterator<T, LRef, LPointerType> &lhs,
                          const DequeIterator<T, RRef, RPointerType> &rhs) {
        return lhs.__node > rhs.__node ||
            (lhs.__node == rhs.__node && lhs.__cur > rhs.__cur);
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator<=(const DequeIterator<T, LRef, LPointerType> &lhs,
                           const DequeIterator<T, RRef, RPointerType> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator>=(const DequeIterator<T, LRef, LPointerType> &lhs,
                           const DequeIterator<T, RRef, RPointerType> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T, typename _Alloc>
    class DequeBase {
    public:
        using Iterator = DequeIterator<T, T&, T*>;
        using ConstIterator = DequeIterator<T, const T&, const T*>;
        DequeBase(): _mapPointer(nullptr), _mapSize(0) {}
        DequeBase(std::size_t count) {
            _initializeMap(count);
        }
        ~DequeBase() {
            if(_mapPointer) {
                _deallocateNodes(_start.__node, _finish.__node);
                _deallocateMap(_mapPointer, _mapSize);
            }
        }

    protected:
        using MapAllocator = SimpleAlloc<T*, _Alloc>;
        using NodeAllocator = SimpleAlloc<T, _Alloc>;

        enum { INITIALIZE_MAP_SIZE = 8 };

        T* _allocateANode() {
            return NodeAllocator::allocate(bufferSize(sizeof(T)));
        }

        void _deallocateANode(T *ptr) {
            NodeAllocator::deallocate(ptr, bufferSize(sizeof(T)));
        }

        T** _allocateMap(std::size_t n) {
            return MapAllocator::allocate(n);
        }

        void _deallocateMap(T **ptr, std::size_t n) {
            MapAllocator::deallocate(ptr, n);
        }

        void _allocateNodes(T **first, T **last) {
            T **cur = first;
            try {
                while(cur != last) {
                    *cur = _allocateANode();
                    ++cur;
                }
            } catch(...) {
                while(first != cur) {
                    _deallocateANode(*first);
                    ++first;
                }
                throw;
            }
        }

        void _deallocateNodes(T **first, T **last) {
            while(first != last) {
                _deallocateANode(*first++);
            }
        }

        void _initializeMap(std::size_t elementCount) {
            const std::size_t nodesCount = elementCount / bufferSize(sizeof(T)) + 1;
            _mapSize = max(static_cast<std::size_t>(INITIALIZE_MAP_SIZE), nodesCount + 2);
            _mapPointer = _allocateMap(_mapSize);
            T **first = _mapPointer + (_mapSize - nodesCount) / 2;
            T **last = first + nodesCount;
            try {
                _allocateNodes(first, last);
            } catch(...) {
                _deallocateNodes(first, last);
                _deallocateMap(_mapPointer, _mapSize);
                _mapPointer = nullptr;
                _mapSize = 0;
                throw;
            }
            _start._setMapNode(first);
            _start.__cur = _start.__first;
            _finish._setMapNode(last - 1);
            _finish.__cur = _finish.__first + (elementCount % bufferSize(sizeof(T)));
        }

        T ** _mapPointer;
        std::size_t _mapSize;

        Iterator _start;
        Iterator _finish;
    };

    template<typename T, typename _Alloc=Alloc>
    class Deque: public DequeBase<T, _Alloc> {
    public:
        using ValueType = T;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
        using Reference = T&;
        using ConstReference = const T&;
        using Pointer = T*;
        using ConstPointer = const T*;
        using Iterator = DequeIterator<T, T&, T*>;
        using ConstIterator = DequeIterator<T, const T&, const T*>;
        using ReverseIterator = ReverseIteratorTemplate<Iterator>;
        using ConstReverseIterator = ReverseIteratorTemplate<ConstIterator>;
    protected:
        using _Base = DequeBase<T, _Alloc>;
        using _Self = Deque<T, _Alloc>;
        using MapPointer = T**;

        using _Base::_initializeMap;
        using _Base::_allocateANode;
        using _Base::_deallocateANode;
        using _Base::_allocateMap;
        using _Base::_deallocateMap;
        using _Base::_deallocateNodes;

        using _Base::_mapPointer;
        using _Base::_mapSize;
        using _Base::_start;
        using _Base::_finish;

    public:
        Deque(): _Base(0) {}
        Deque(SizeType count, const T &value);
        explicit Deque(SizeType count);
        template<typename InputIterator>
        Deque(InputIterator first, InputIterator last);
        Deque(const _Self &other);
        ~Deque();

        _Self& operator=(const _Self &other);
        void assign(SizeType count, const T &value);
        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last);

        Reference operator[](int pos) { return *(_start + pos); }
        ConstReference operator[](int pos) const { return *(_start + pos); }
        Reference at(int pos) { _checkPos(pos); return (*this)[pos]; }
        ConstReference at(int pos) const { _checkPos(pos); return (*this)[pos]; }
        Reference front() { return *begin(); }
        ConstReference front() const { return *cbegin(); }
        Reference back() { return *(--end()); }
        ConstReference back() const { return *(--cend()); }

        Iterator begin() { return _start; }
        ConstIterator begin() const { return ConstIterator(_start); }
        ConstIterator cbegin() const { return ConstIterator(_start); }
        Iterator end() { return _finish; }
        ConstIterator end() const { return ConstIterator(_finish); }
        ConstIterator cend() const { return ConstIterator(_finish); }

        ReverseIterator rbegin() { return ReverseIterator(end()); }
        ConstReverseIterator rbegin() const { return ConstReverseIterator(cend()); }
        ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); };
        ReverseIterator rend() { return ReverseIterator(begin()); }
        ConstReverseIterator rend() const { return ConstReverseIterator(cbegin()); }
        ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); }

        bool empty() const { return _start == _finish; }
        SizeType size() const { return _finish - _start; }
        SizeType maxSize() const { return static_cast<SizeType>(-1) / sizeof(T); };

        void clear() { erase(cbegin(), cend()); }
        Iterator insert(ConstIterator pos, const T &value);
        Iterator insert(ConstIterator pos, SizeType count, const T &value);
        template<typename InputIterator>
        Iterator insert(ConstIterator pos, InputIterator first, InputIterator last);

        Iterator erase(ConstIterator pos);
        Iterator erase(ConstIterator first, ConstIterator last);

        void pushBack(const T &value);
        void popBack();
        void pushFront(const T &value);
        void popFront();
        void resize(SizeType count, const T &value);
        void resize(SizeType count);

        void swap(_Self &other);

    protected:
        void _checkPos(int pos) {
            if(pos >= size()) {
                throw std::out_of_range("Deque");
            }
        }

        void _fillInitialize(SizeType count, const T &value) {
            uninitializedFill(_start, _finish, value);
        }

        template<typename InputIterator>
        void _rangeInitialize(InputIterator first, InputIterator last);
        template<typename Integer>
        void _rangeInitializeAux(Integer count, Integer value, TrueType);
        template<typename InputIterator>
        void _rangeInitializeAux(InputIterator first, InputIterator last,
                                 FalseType);
        template<typename InputIterator>
        void _rangeInitializeAux2(InputIterator first, InputIterator last,
                                  InputIteratorTag);
        template<typename ForwardIterator>
        void _rangeInitializeAux2(ForwardIterator first, ForwardIterator last, ForwardIteratorTag);

        void _fillAssign(SizeType count, const T &value);
        template<typename InputIterator>

        void _rangeAssign(InputIterator first, InputIterator last);
        template<typename Integer>
        void _rangeAssignAux(Integer count, Integer value, TrueType);
        template<typename InputIterator>
        void _rangeAssignAux(InputIterator first, InputIterator last,
                             FalseType);

        Iterator _reserveElementAtFront(SizeType count = 1);
        Iterator _reserveElementAtBack(SizeType count = 1);
        void _reserveNodeAtFront(SizeType count = 1);
        void _reserveNodeAtBack(SizeType count = 1);
        void _reserveMapAtFront(SizeType nodeToAddCount = 1);
        void _reserveMapAtBack(SizeType nodeToAddCount = 1);
        void _newElementsAtFront(SizeType count);
        void _newElementsAtBack(SizeType count);
        void _reallocateMapToAddNodes(SizeType count, bool addAtFront);

        void _pushBackAux(const T &value);
        void _pushFrontAux(const T &value);

        Iterator _fillInsert(Iterator pos, SizeType count, const T &value);
        Iterator _fillInsertAux(Iterator pos, SizeType count, const T &value);

        template<typename InputIterator>
        Iterator _rangeInsert(Iterator pos, InputIterator first, InputIterator last);

        template<typename Integer>
        Iterator _rangeInsertAux(Iterator pos, Integer count,
                                 Integer value, TrueType);
        template<typename InputIterator>
        Iterator _rangeInsertAux(Iterator pos, InputIterator first,
                                 InputIterator last, FalseType);
        template<typename InputIterator>
        Iterator _rangeInsertAux2(Iterator pos, InputIterator first,
                                  InputIterator last, InputIteratorTag);
        template<typename ForwardIterator>
        Iterator _rangeInsertAux2(Iterator pos, ForwardIterator first,
                                  ForwardIterator last, ForwardIteratorTag);

        Iterator _erase(Iterator pos);
        Iterator _erase(Iterator first, Iterator last);

        void _popBackAux();
        void _popFrontAux();
    };

    template<typename T, typename _Alloc>
    inline Deque<T, _Alloc>::Deque(SizeType count, const T &value): _Base(count) {
        _fillInitialize(count, value);
    }

    template<typename T, typename _Alloc>
    inline Deque<T, _Alloc>::Deque(SizeType count): Deque(count, T()) {}

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline Deque<T, _Alloc>::Deque(InputIterator first, InputIterator last) {
        _rangeInitialize(first, last);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Deque<T, _Alloc>::_rangeInitialize(InputIterator first,
                                                   InputIterator last) {
        _rangeInitializeAux(first, last,
                            typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename _Alloc>
    template<typename Integer>
    inline void Deque<T, _Alloc>::_rangeInitializeAux(Integer count, Integer value, TrueType) {
        _initializeMap(count);
        _fillInitialize(count, value);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Deque<T, _Alloc>::_rangeInitializeAux(InputIterator first,
                                                      InputIterator last,
                                                      FalseType) {
        _rangeInitializeAux2(first, last,
                             typename IteratorTraits<InputIterator>::IteratorCategory());
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Deque<T, _Alloc>::_rangeInitializeAux2(InputIterator first,
                                                       InputIterator last,
                                                       InputIteratorTag) {
        insert(end(), first, last);
    }

    template<typename T, typename _Alloc>
    template<typename ForwardIterator>
    inline void Deque<T, _Alloc>::_rangeInitializeAux2(ForwardIterator first,
                                                       ForwardIterator last,
                                                       ForwardIteratorTag) {
        const SizeType count = tinystl::distance(first, last);
        _initializeMap(count);
        uninitializedCopy(first, last, _start);
    }

    template<typename T, typename _Alloc>
    inline Deque<T, _Alloc>::Deque(const _Self &other): _Base(other.size()) {
        uninitializedCopy(other.cbegin(), other.cend(), _start);
    }

    template<typename T, typename _Alloc>
    Deque<T, _Alloc>::~Deque() {
        destroy(begin(), end());
    }

    template<typename T, typename _Alloc>
    inline Deque<T, _Alloc>& Deque<T, _Alloc>::operator=(const Deque<T,
                                                         _Alloc> &other) {
        assign(other.cbegin(), other.cend());
    }

    template<typename T, typename _Alloc>
    void Deque<T, _Alloc>::assign(SizeType count, const T &value) {
        _fillAssign(count, value);
    }

    template<typename T, typename _Alloc>
    void Deque<T, _Alloc>::_fillAssign(SizeType count, const T &value) {
        if(size() > count) {
            fillN(begin(), count, value);
            erase(begin() + count, end());
        } else {
            fill(begin(), end(), value);
            insert(end(), count - size(), value);
        }
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    void Deque<T, _Alloc>::assign(InputIterator first, InputIterator last) {
        _rangeAssign(first, last);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    void Deque<T, _Alloc>::_rangeAssign(InputIterator first, InputIterator last) {
        _rangeAssignAux(first, last,
                        typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename _Alloc>
    template<typename Integer>
    void Deque<T, _Alloc>::_rangeAssignAux(Integer count, Integer value,
                                           TrueType) {
        _fillAssign(count, value);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    void Deque<T, _Alloc>::_rangeAssignAux(InputIterator first, InputIterator last,
                                           FalseType) {
        Iterator first1 = begin();
        Iterator last1 = end();
        while(first1 != last1 && first != last) {
            *first1++ = *first++;
        }
        if(first1 == last1) {
            insert(end(), first, last);
        } else {
            erase(first1, end());
        }
    }

    template<typename T, typename _Alloc>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_reserveElementAtFront(SizeType count) {
        const SizeType restItemCountOfCurrentNode = _start.__cur - _start.__first;
        if(count > restItemCountOfCurrentNode) {
            _newElementsAtFront(count - restItemCountOfCurrentNode);
        }
        return _start - count;
    }

    template<typename T, typename _Alloc>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_reserveElementAtBack(SizeType count) {
        // 减一是为了让_finish.__cur始终指向一个可用的地址
        const SizeType restItemCountOfCurrentNode = _finish.__last - _finish.__cur - 1;
        if(count > restItemCountOfCurrentNode) {
            _newElementsAtBack(count - restItemCountOfCurrentNode);
        }
        return _finish + count;
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_newElementsAtFront(SizeType count) {
        const SizeType nodeCountToAdd = (count + bufferSize(sizeof(T)) - 1) / bufferSize(sizeof(T));
        _reallocateMapToAddNodes(nodeCountToAdd, true);
        SizeType i = 1;
        try {
            for(i = 1; i <= nodeCountToAdd; ++i) {
                *(_start.__node - i) = _allocateANode();
            }
        } catch(...) {
            for(int j = 1; j < i; ++j) {
                _deallocateANode(*(_start.__node - j));
            }
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_newElementsAtBack(SizeType count) {
        const SizeType nodeCountToAdd = (count + bufferSize(sizeof(T)) - 1) / bufferSize(sizeof(T));
        _reallocateMapToAddNodes(nodeCountToAdd, false);
        SizeType i = 1;
        try {
            for(i = 1; i <= nodeCountToAdd; ++i) {
                *(_finish.__node + i) = _allocateANode();
            }
        } catch(...) {
            for(int j = 1; j < i; ++j) {
                _deallocateANode(*(_finish.__node + j));
            }
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_reallocateMapToAddNodes(SizeType addNodeCount,
                                                          bool addAtFront) {
        const SizeType oldNodeCount = _finish.__node - _start.__node + 1;
        const SizeType newNodeCount = oldNodeCount + addNodeCount;
        MapPointer newStart;
        if(_mapSize > 2 * newNodeCount) {
            newStart = _mapPointer + (_mapSize - newNodeCount) / 2 +
                (addAtFront? addNodeCount: 0);
            if(newStart < _start.__node) {
                copy(_start.__node, _finish.__node + 1, newStart);
            } else {
                copyBackward(_start.__node, _finish.__node + 1, newStart + oldNodeCount);
            }
        } else {
            const SizeType newMapSize = _mapSize + max(_mapSize, newNodeCount) + 2;
            const MapPointer newMapPointer = _allocateMap(newMapSize);
            newStart = newMapPointer + (newMapSize - newNodeCount) / 2 +
                (addAtFront? addNodeCount: 0);
            copy(_start.__node, _finish.__node + 1, newStart);

            _deallocateMap(_mapPointer, _mapSize);
            _mapPointer = newMapPointer;
            _mapSize = newMapSize;
        }
        _start._setMapNode(newStart);
        _finish._setMapNode(newStart + oldNodeCount - 1);
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_reserveMapAtFront(SizeType nodeToAddCount) {
        if(nodeToAddCount > (_start.__node - _mapPointer)) {
            _reallocateMapToAddNodes(1, true);
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_reserveMapAtBack(SizeType nodeToAddCount) {
        if(nodeToAddCount > _mapSize - (_finish.__node - _mapPointer) - 1) {
            _reallocateMapToAddNodes(1, false);
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::pushBack(const T &value) {
        if(_finish.__cur == _finish.__last - 1) {
            construct(_finish.__cur, value);
            ++_finish;
        } else {
            _pushBackAux(value);
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_pushBackAux(const T &value) {
        _reserveMapAtBack(1);
        *(_finish.__node + 1) = _allocateANode();
        try {
            construct(_finish.__cur, value);
        } catch(...) {
            _deallocateANode(*(_finish.__node + 1));
            throw;
        }
        ++_finish;
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::pushFront(const T &value) {
        if(_start.__cur != _start.__first) {
            construct(_start.__cur - 1, value);
            --_start;
        } else {
            _pushFrontAux(value);
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_pushFrontAux(const T &value) {
        _reserveMapAtFront(1);
        *(_start.__node - 1) = _allocateANode();
        T *last = *(_start.__node - 1) + bufferSize(sizeof(T));
        try {
            construct(last - 1, value);
        } catch(...) {
            _deallocateANode(*(_start.__node - 1));
            throw;
        }
        --_start;
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::insert(ConstIterator pos, const T &value) {
        if(pos == _start) {
            pushFront(value);
            return _start;
        }
        if(pos == _finish) {
            pushBack(value);
            return _finish - 1;
        }
        DifferenceType index = pos - _start;
        ValueType valueCopy = value;
        if(static_cast<SizeType>(index) < size() / 2) {
            pushFront(front());
            Iterator insertPos = _start + index;
            copy(_start + 2, insertPos + 1, _start + 1);
            *insertPos = valueCopy;
            return insertPos;
        } else {
            pushBack(back());
            Iterator insertPos = _start + index;
            copy(insertPos, _finish - 2, _finish - 1);
            *insertPos = valueCopy;
            return insertPos;
        }
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_fillInsertAux(Iterator pos, SizeType count,
                                     const T &value) {
        const SizeType frontElementCount = pos - _start;
        if(frontElementCount < size() / 2) {
            Iterator newStart = _reserveElementAtFront(count);
            if(frontElementCount <= count) {
                Iterator it = uninitializedCopy(_start, pos, newStart);
                uninitializedFill(it, _start, value);
                fill(_start, pos, value);
            } else {
                uninitializedCopy(_start, _start + count, newStart);
                copy(_start + count, pos, _start);
                fill(pos - count, pos, value);
            }
            _start = newStart;
        } else {
            Iterator newFinish = _reserveElementAtBack(count);
            uninitializedFill(_finish, newFinish, value);
            copyBackward(pos, _finish, newFinish);
            fillN(pos, count, value);
            _finish = newFinish;
        }
        return _start + frontElementCount;
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_fillInsert(Iterator pos, SizeType count,
                                  const T &value) {
        if(pos == _start) {
            Iterator newStart = _reserveElementAtFront(count);
            uninitializedFill(newStart, _start, value);
            _start = newStart;
            return _start;
        }
        if(pos == _finish) {
            Iterator newFinish = _reserveElementAtBack(count);
            uninitializedFill(_finish, newFinish, value);
            Iterator ret = _finish;
            _finish = newFinish;
            return ret;
        }
        return _fillInsertAux(pos, count, value);
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    inline Deque<T, _Alloc>::insert(ConstIterator pos, SizeType count, const T &value) {
        return _fillInsert(pos.removeConst(), count, value);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    typename Deque<T, _Alloc>::Iterator
    inline Deque<T, _Alloc>::insert(ConstIterator pos, InputIterator first,
                                    InputIterator last) {
        return _rangeInsert(pos.removeConst(), first, last);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_rangeInsert(Iterator pos, InputIterator first,
                                   InputIterator last) {
        return _rangeInsertAux(pos, first, last,
                               typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename _Alloc>
    template<typename Integer>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_rangeInsertAux(Iterator pos, Integer count,
                    Integer value, TrueType) {
        return _fillInsert(pos, static_cast<SizeType>(count), static_cast<T>(value));
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_rangeInsertAux(Iterator pos, InputIterator first,
                    InputIterator last, FalseType) {
        using IteratorType = typename IteratorTraits<InputIterator>::IteratorCategory;
        return _rangeInsertAux2(pos, first, last, IteratorType());
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_rangeInsertAux2(Iterator pos, InputIterator first,
                                       InputIterator last, InputIteratorTag) {
        const DifferenceType firstInsertIndex = pos - _start;
        while(first != last) {
            pos = insert(pos, *first);
            ++pos;
            ++first;
        }
        return _start + firstInsertIndex;
    }

    template<typename T, typename _Alloc>
    template<typename ForwardIterator>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_rangeInsertAux2(Iterator pos, ForwardIterator first,
                                       ForwardIterator last, ForwardIteratorTag) {
        const SizeType count = tinystl::distance(first, last);
        const SizeType frontElementCount = static_cast<SizeType>(pos - _start);
        if(frontElementCount < size() / 2) {
            Iterator newStart = _reserveElementAtFront(count);
            if(frontElementCount <= count) {
                Iterator it = uninitializedCopy(_start, pos, newStart);
                ForwardIterator mid = first + (count - frontElementCount);
                uninitializedCopy(first, mid, it);
                copy(mid, last, _start);
            } else {
                uninitializedCopy(_start, _start + count, newStart);
                Iterator it = copy(_start + count, pos, _start);
                copy(first, last, it);
            }
            _start = newStart;
        } else {
            Iterator newFinish = _reserveElementAtBack(count);
            uninitializedFill(_finish, newFinish, T());
            copyBackward(pos, _finish, newFinish);
            copy(first, last, pos);
            _finish = newFinish;
        }
        return _start + static_cast<DifferenceType>(frontElementCount);
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    inline Deque<T, _Alloc>::erase(ConstIterator pos) {
        return _erase(pos.removeConst());
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_erase(Iterator pos) {
        const SizeType frontElementCount = static_cast<SizeType>(pos - _start);
        if(frontElementCount < size() / 2) {
            copyBackward(_start, pos, pos + 1);
            popFront();
        } else {
            copy(pos + 1, _finish, pos);
            popBack();
        }
        return _start + static_cast<DifferenceType>(frontElementCount);
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::erase(ConstIterator first, ConstIterator last) {
        return _erase(first.removeConst(), last.removeConst());
    }

    template<typename T, typename _Alloc>
    typename Deque<T, _Alloc>::Iterator
    Deque<T, _Alloc>::_erase(Iterator first, Iterator last) {
        const SizeType frontElementCount = first - _start;
        const SizeType restCount = size() - static_cast<SizeType>(last - first);
        if(frontElementCount < restCount / 2) {
            Iterator newStart = copyBackward(_start, first, last);
            destroy(_start, newStart);
            _deallocateNodes(_start.__node, newStart.__node);
            _start = newStart;
        } else {
            Iterator newFinish = copy(last, _finish, first);
            destroy(newFinish, _finish);
            _deallocateNodes(newFinish.__node + 1, _finish.__node + 1);
            _finish = newFinish;
        }
        return _start + static_cast<SizeType>(frontElementCount);
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::popBack() {
        if(_finish.__cur != _finish.__first) {
            --_finish.__cur;
            destroy(_finish.__cur);
        } else {
            _popBackAux();
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_popBackAux() {
        _deallocateANode(*_finish.__node);
        _finish._setMapNode(_finish.__node - 1);
        _finish.__cur = _finish.__last - 1;
        destroy(_finish.__cur);
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::popFront() {
        if(_start.__cur != _start.__last - 1) {
            destroy(_start.__cur);
            ++_start.__cur;
        } else {
            _popFrontAux();
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::_popFrontAux() {
        destroy(_start.__cur);
        _deallocateANode(*_start.__node);
        _start._setMapNode(_start.__node + 1);
        _start.__cur = _start.__first;
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::resize(SizeType count, const T &value) {
        const SizeType oldCount = size();
        if(oldCount < count) {
            insert(cend(), count - oldCount, value);
        } else {
            erase(_finish - (oldCount - count), _finish);
        }
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::resize(SizeType count) {
        resize(count, T());
    }

    template<typename T, typename _Alloc>
    inline void Deque<T, _Alloc>::swap(_Self &other) {
        tinystl::swap(_mapPointer, other._mapPointer);
        tinystl::swap(_mapSize, other._mapSize);
        tinystl::swap(_start, other._start);
        tinystl::swap(_finish, other._finish);
    }

    template<typename T, typename _Alloc>
    inline bool operator==(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return lhs.size() == rhs.size() &&
            equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template<typename T, typename _Alloc>
    inline bool operator!=(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename _Alloc>
    inline bool operator>(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return greater(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename _Alloc>
    inline bool operator<(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return less(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename _Alloc>
    inline bool operator>=(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T, typename _Alloc>
    inline bool operator<=(const Deque<T, _Alloc> &lhs, const Deque<T, _Alloc> &rhs) {
        return !(lhs > rhs);
    }

}

#endif

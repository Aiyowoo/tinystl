#ifndef VECTOR_H
#define VECTOR_H

#include <stdexcept>
#include "alloc.h"
#include "algobase.h"
#include "construct.h"
#include "iterator.h"
#include "uninitialized.h"
#include "iteratorbase.h"

namespace tinystl {
    // note
    // 当constructor中抛出异常时:
    // 会先先将已经构造好的成员析构,
    // 然后逆向调用基类的析构函数

    // 所以在此,使用VectorBase来完成空间的管理
    template<typename T, typename _Alloc>
    class VectorBase {
    public:
        using Allocator = SimpleAlloc<T, _Alloc>;
        VectorBase(): _start(nullptr), _finish(nullptr), _endOfStorage(nullptr) {}
        VectorBase(std::size_t n): _start(nullptr), _finish(nullptr),
                                   _endOfStorage(nullptr) {
            _start = _allocate(n);
            _finish = _start;
            _endOfStorage = _start + n;
        }

        ~VectorBase() {
            _deallocate(_start, _endOfStorage - _start);
        }

    protected:
        T* _allocate(std::size_t n) {
            return Allocator::allocate(n);
        }

        void _deallocate(T *ptr, std::size_t n) {
            if(ptr) {
                Allocator::deallocate(ptr, n);
            }
        }

        T* _start;
        T* _finish;
        T* _endOfStorage;
    };

    template<typename T, typename _Alloc=Alloc>
    class Vector: protected VectorBase<T, _Alloc> {
    public:
        using ValueType = T;
        using Iterator = T*;
        using ConstIterator = const T*;
        using ReverseIterator = ReverseIteratorTemplate<Iterator>;
        using ConstReverseIterator = ReverseIteratorTemplate<ConstIterator>;
        using DifferenceType = std::ptrdiff_t;
        using SizeType = std::size_t;
        using Pointer = T*;
        using ConstPointer = const T*;
        using Reference = T&;
        using ConstReference = const T&;

    private:
        using Base = VectorBase<T, _Alloc>;
        using Self = Vector<T, _Alloc>;

    protected:
        // using 语句时名称在此作用域课件,函数的话会参与函数匹配过程
        using Base::_allocate;
        using Base::_deallocate;
        using Base::_start;
        using Base::_finish;
        using Base::_endOfStorage;

    public:
        Vector() = default;

        Vector(SizeType n, const T&value): Base(n) {
            _finish = uninitializedFillN(_start, n, value);
        }

        Vector(SizeType n): Base(n) {
            _finish = uninitializedFillN(_start, n, ValueType());
        }

        Vector(const Self &other): Base(other.size()) {
            _finish = uninitializedCopy(other.begin(), other.end(), _start);
        }

        template<typename InputIterator>
        Vector(InputIterator first, InputIterator last) {
            _initializeAux(first, last, typename IsInteger<InputIterator>::Integral());
        }

        Iterator begin() {
            return _start;
        }

        ConstIterator begin() const {
            return _start;
        }

        ConstIterator cbegin() const {
            return _start;
        }

        Iterator end() {
            return _finish;
        }

        ConstIterator end() const {
            return _finish;
        }

        ConstIterator cend() const {
            return _finish;
        }

        ReverseIterator rbegin() {
            return ReverseIterator(end());
        }

        ReverseIterator rend() {
            return ReverseIterator(begin());
        }

        ConstReverseIterator rbegin() const {
            return ConstReverseIterator(cend());
        }

        ConstReverseIterator rend() const {
            return ConstReverseIterator(cbegin());
        }


        ConstReverseIterator crbegin() const {
            return ConstReverseIterator(cend());
        }

        ConstReverseIterator crend() const {
            return ConstReverseIterator(cbegin());
        }

        SizeType size() const {
            return static_cast<SizeType>(cend() - cbegin());
        }

        SizeType maxSize() const {
            return SizeType(-1) / sizeof(ValueType);
        }

        SizeType capacity() const {
            return static_cast<SizeType>(_endOfStorage - begin());
        }

        bool empty() const {
            return end() == begin();
        }

        Reference operator[](SizeType n) {
            return *(begin() + n);
        }

        ConstReference operator[](SizeType n) const {
            return *(begin() + n);
        }

        Reference at(SizeType n) {
            _rangeCheck(n);
            return (*this)[n];
        }

        ConstIterator at(SizeType n) const {
            _rangeCheck(n);
            return (*this)[n];
        }

        Reference front() {
            return *begin();
        }

        ConstReference front() const {
            return *cbegin();
        }

        Reference back() {
            return *(end() - 1);
        }

        ConstReference back() const {
            return *(cend() - 1);
        }

        void pushBack(const ValueType &value) {
            if(end() != _endOfStorage) {
                construct(_finish, value);
                ++_finish;
            } else {
                _insertAux(end(), value);
            }
        }

        void swap(Self &other) {
            using tinystl::swap;
            swap(_start, other._start);
            swap(_finish, other._finish);
            swap(_endOfStorage, other._endOfStorage);
        }

        Iterator insert(ConstIterator pos, const ValueType &value) {
            SizeType n = pos - begin();
            if(_finish != _endOfStorage && pos == end()) {
                construct(pos, value);
                ++_finish;
            } else {
                _insertAux(pos, value);
            }
            return begin() + n;
        }

        void insert(Iterator pos, const ValueType &value) {
            if(_finish != _endOfStorage && pos == end()) {
                construct(pos, value);
                ++_finish;
            } else {
                _insertAux(pos, value);
            }
        }

        void insert(Iterator pos, SizeType n, const ValueType &value);

        Iterator insert(ConstIterator pos, SizeType n, const ValueType &value);

        template<typename InputIterator>
        void insert(Iterator pos, InputIterator first, InputIterator last);

        template<typename InputIterator>
        Iterator insert(ConstIterator pos, InputIterator first, InputIterator last);

        void popBack() {
            destroy(_finish);
            --_finish;
        }

        Iterator erase(Iterator pos) {
            copy(pos + 1, _finish, pos);
            destroy(_finish);
            --_finish;
            return pos;
        }

        Iterator erase(ConstIterator pos) {
            Iterator ipos = begin() + (pos - cbegin());
            return erase(ipos);
        }

        Iterator erase(Iterator first, Iterator last) {
            if(first == last) {
                return first;
            }

            T *newFinish = copy(last, _finish, first);
            destroy(newFinish, _finish);
            _finish = newFinish;
            return first;
        }

        Iterator erase(ConstIterator first, ConstIterator last) {
            Iterator iFirst = begin() + (first - cbegin());
            Iterator iLast = begin() + (last - cend());
            return erase(iFirst, iLast);
        }

        void resize(SizeType newSize, const T &value) {
            if(newSize > size()) {
                insert(end(), newSize - size(), value);
            } else {
                erase(begin() + newSize, end());
            }
        }

        void resize(SizeType newSize) {
            resize(newSize, ValueType());
        }

        void clear() {
            erase(begin(), end());
        }

        void assign(SizeType count, const T &value);

        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last);

        Self& operator=(const Self &other);

    protected:
        void _rangeCheck(SizeType n) const {
            if(n >= size()) {
                throw std::out_of_range("out of range vector");
            }
        }

        template<typename Integer>
        void _initializeAux(Integer n, Integer value, TrueType) {
            _start = _allocate(n);
            _endOfStorage = _start + n;
            _finish = uninitializedFillN(_start, n, value);
        }

        template<typename InputIterator>
        void _initializeAux(InputIterator first, InputIterator last,
                          FalseType) {
            _rangeInitialize(first, last);
        }

        template<typename InputIterator>
        void _rangeInitialize(InputIterator first, InputIterator last) {
            _rangeInitializeAux(first, last,
                                typename IteratorTraits<InputIterator>::IteratorCategory());
        }

        template<typename InputIterator>
        void _rangeInitializeAux(InputIterator first, InputIterator last,
                             InputIteratorTag) {
            while(first != last) {
                pushBack(*first);
                ++first;
            }
        }

        template<typename ForwardIterator>
        void _rangeInitializeAux(ForwardIterator first, ForwardIterator last,
                              ForwardIteratorTag) {
            SizeType length = static_cast<SizeType>(tinystl::distance(first, last));
            _start = _allocate(length);
            _finish = _start;
            _endOfStorage = _start + length;
            _finish = uninitializedCopy(first, last, _start);
        }

        void _insertAux(Iterator pos, const T&value);
        void _insertAux(Iterator pos);

        void _fillInsert(Iterator pos, SizeType n, const ValueType &value);

        template<typename InputIterator>
        void _rangeInsert(Iterator pos, InputIterator first, InputIterator last);

        template<typename Integer>
        void _rangeInsertAux(Iterator pos, Integer n, Integer value, TrueType);

        template<typename InputIterator>
        void _rangeInsertAux(Iterator pos, InputIterator first,
                             InputIterator last, FalseType);

        template<typename InputIterator>
        void _rangeInsertIteratorAux(Iterator pos, InputIterator first,
                                     InputIterator last, InputIteratorTag);

        template<typename ForwardIterator>
        void _rangeInsertIteratorAux(Iterator pos, ForwardIterator first,
                                     ForwardIterator last, ForwardIteratorTag);

        void _fillAssign(SizeType count, const T&value);
        template<typename InputIterator>
        void _rangeAssign(InputIterator first, InputIterator last);
        template<typename Integer>
        void _rangeAssignAux(Integer count, Integer value, TrueType);
        template<typename InputIterator>
        void _rangeAssignAux(InputIterator first, InputIterator last,
                             FalseType);
        template<typename InputIterator>
        void _rangeAssignIteratorAux(InputIterator first, InputIterator last,
                                     InputIteratorTag);
        template<typename ForwardIterator>
        void _rangeAssignIteratorAux(ForwardIterator first,
                                     ForwardIterator last,
                                     ForwardIteratorTag);

    };

    template<typename T, typename _Alloc>
    void Vector<T, _Alloc>::_insertAux(Iterator pos, const T &value) {
        if(_finish != _endOfStorage) {
            construct(_finish, *(_finish + 1));
            ++_finish;
            T copyObj = value;
            copyBackward(pos, _finish - 2, _finish - 1);
            *pos = copyObj;
        } else {
            const SizeType oldSize = size();
            const SizeType newLength = oldSize? 2 * oldSize: 1;
            T* newStart = _allocate(newLength);
            T* newFinish = newStart;
            T* newEndOfStorage = newStart + newLength;
            try {
                newFinish = uninitializedCopy(_start, pos, newStart);
                construct(newFinish, value);
                ++newFinish;
                newFinish = uninitializedCopy(pos, _finish, newFinish);
            } catch(...) {
                destroy(newStart, newFinish);
                _deallocate(newStart, newLength);
                throw;
            }
            destroy(_start, _finish);
            _deallocate(_start, capacity());
            _start = newStart;
            _finish = newFinish;
            _endOfStorage = newEndOfStorage;
        }
    }

    template<typename T, typename _Alloc>
    inline void Vector<T, _Alloc>::_insertAux(Iterator pos) {
        _insertAux(pos, ValueType());
    }

    template<typename T, typename _Alloc>
    void Vector<T, _Alloc>::_fillInsert(Iterator pos, SizeType n,
                                          const ValueType &value) {
        if(n == 0) {
            return;
        }

        if(_endOfStorage - _finish >= n) {
            const SizeType afterElementCount = _finish - pos;
            T * const oldFinish = _finish;
            if(afterElementCount > n) {
                _finish = uninitializedCopy(_finish - n, _finish, _finish);
                copyBackward(pos, pos + (afterElementCount - n), oldFinish);
                fillN(pos, n, value);
            } else {
                _finish = uninitializedCopy(pos, _finish,
                                           _finish + (n - afterElementCount));
                fillN(pos, oldFinish, value);
                uninitializedFillN(oldFinish, n - afterElementCount, value);
            }
        } else {
            const SizeType oldSize = size();
            const SizeType newLength = oldSize + max(oldSize, n);
            T * const newStart = _allocate(newLength);
            T * newFinish = newStart;
            T * const newEndOfStorage = newStart + newLength;
            try {
                newFinish = uninitializedCopy(_start, pos, newStart);
                newFinish = uninitializedFillN(newFinish, n, value);
                newFinish = uninitializedCopy(pos, _finish, newFinish);
            } catch(...) {
                destroy(newStart, newFinish);
                _deallocate(newStart, newLength);
                throw;
            }
            destroy(_start, _finish);
            _deallocate(_start, oldSize);
            _start = newStart;
            _finish = newFinish;
            _endOfStorage = newEndOfStorage;
        }
    }

    template<typename T, typename _Alloc>
    inline void Vector<T, _Alloc>::insert(Iterator pos, SizeType n,
                                   const ValueType &value) {
        _fillInsert(pos, n, value);
    }

    template<typename T, typename _Alloc>
    inline typename Vector<T, _Alloc>::Iterator
    Vector<T, _Alloc>::insert(ConstIterator pos, SizeType n, const ValueType &value) {
        Iterator ipos = begin() + (pos - cbegin());
        _fillInsert(ipos, n, value);
        return ipos;
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeInsert(Iterator pos, InputIterator first,
                                                InputIterator last) {
        _rangeInsertAux(pos, first, last, typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename _Alloc>
    template<typename Integer>
    inline void Vector<T, _Alloc>::_rangeInsertAux(Iterator pos,
                                                   Integer n, Integer value,
                                                   TrueType) {
        _fillInsert(pos, static_cast<SizeType>(n), static_cast<T>(value));
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeInsertAux(Iterator pos,
                                                   InputIterator first,
                                                   InputIterator last,
                                                   FalseType) {
        _rangeInsertIteratorAux(pos, first, last,
                                typename IteratorTraits<InputIterator>::IteratorCategory());
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeInsertIteratorAux(Iterator pos,
                                                           InputIterator first,
                                                           InputIterator last,
                                                           InputIteratorTag) {
        while(first != last) {
            insert(pos, *first);
            ++pos;
            ++first;
        }
    }

    template<typename T, typename _Alloc>
    template<typename ForwardIterator>
    inline void Vector<T, _Alloc>::_rangeInsertIteratorAux(Iterator pos,
                                                           ForwardIterator first,
                                                           ForwardIterator last,
                                                           ForwardIteratorTag) {
        if(first == last) {
            return;
        }

        const SizeType n = tinystl::distance(first, last);
        if(_endOfStorage - _finish >= n) {
            const SizeType afterItemCount = _finish - pos;
            T * const oldFinish = _finish;
            if(afterItemCount > n) {
                _finish = uninitializedCopy(_finish - n, _finish, _finish);
                copyBackward(pos, oldFinish - n, oldFinish);
                tinystl::copy(first, last, pos);
            } else {
                _finish = uninitializedCopy(pos, _finish, _finish + (n - afterItemCount));
                uninitializedFill(oldFinish, _finish - afterItemCount, ValueType());
                tinystl::copy(first, last, pos);
            }
        } else {
            const SizeType oldSize = size();
            const SizeType newLength = oldSize + max(oldSize, n);
            T * newStart = _allocate(newLength);
            T * newFinish = newStart;
            T * newEndOfStorage = newStart + newLength;
            try {
                newFinish = uninitializedCopy(_start, pos, newFinish);
                newFinish = uninitializedCopy(first, last, newFinish);
                newFinish = uninitializedCopy(pos, _finish, newFinish);
            } catch(...) {
                destroy(newStart, newFinish);
                _deallocate(newStart, newLength);
                throw;
            }
            destroy(_start, _finish);
            _deallocate(_start, _endOfStorage - _start);
            _start = newStart;
            _finish = newFinish;
            _endOfStorage = newEndOfStorage;
        }
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::insert(Iterator pos, InputIterator first,
                                   InputIterator last) {
        _rangeInsert(pos, first, last);
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline typename Vector<T, _Alloc>::Iterator
    Vector<T, _Alloc>::insert(ConstIterator pos, InputIterator first,
                              InputIterator last) {
        Iterator ipos = begin() + (pos - cbegin());
        insert(ipos, first, last);
    }

    template<typename T, typename _Alloc>
    void Vector<T, _Alloc>::assign(SizeType count, const T &value) {
        _fillAssign(count, value);
    }

    template<typename T, typename _Alloc>
    void Vector<T, _Alloc>::_fillAssign(SizeType count, const T &value) {
        if(count <= _endOfStorage - _start) {
            resize(count, value);
        } else {
            T *newStart = _allocate(count);
            T *newFinish = newStart;
            T *newEndOfStorage = newStart + count;
            try {
                newFinish = uninitializedFillN(newStart, count, value);
            } catch(...) {
                destroy(newStart, newFinish);
                _deallocate(newStart, count);
                throw;
            }
            destroy(_start, _finish);
            _deallocate(_start, _endOfStorage - _start);
            _start = newStart;
            _finish = newFinish;
            _endOfStorage = newEndOfStorage;
        }
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeAssign(InputIterator first,
                                                InputIterator last) {
        _rangeAssignAux(first, last, typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename _Alloc>
    template<typename Integer>
    inline void Vector<T, _Alloc>::_rangeAssignAux(Integer count, Integer value,
                                                   TrueType) {
        _fillAssign(static_cast<SizeType>(count), static_cast<T>(value));
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeAssignAux(InputIterator first,
                                                  InputIterator last,
                                                  FalseType) {
        _rangeAssignIteratorAux(first, last,
                                typename IteratorTraits<InputIterator>::IteratorCategory());
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::_rangeAssignIteratorAux(InputIterator first,
                                                           InputIterator last,
                                                           InputIteratorTag) {
        T *cur = begin();
        while(cur != end() and first != last) {
            *cur++ = *first++;
        }
        if(first == last) {
            erase(cur, end());
        } else {
            insert(end(), first, last);
        }
    }

    template<typename T, typename _Alloc>
    template<typename ForwardIterator>
    inline void Vector<T, _Alloc>::_rangeAssignIteratorAux(ForwardIterator first,
                                                           ForwardIterator last,
                                                           ForwardIteratorTag) {
        const SizeType n = tinystl::distance(first, last);
        if(_endOfStorage - _start >= n) {
            if(n <= size()) {
                T *const newFinish = tinystl::copy(first, last, _start);
                erase(newFinish, _finish);
                _finish = newFinish;
            } else {
                T *newFinish = copyN(first, size(), _start);
                tinystl::advance(first, size());
                _finish = uninitializedCopy(first, last, newFinish);
            }
        } else {
            T *newStart = _allocate(n);
            T *newFinish = newStart;
            T *newEndOfStorage = newStart + n;
            try {
                newFinish = uninitializedCopy(first, last, newFinish);
            } catch(...) {
                destroy(newStart, newFinish);
                _deallocate(newStart, n);
                throw;
            }
            destroy(_start, _finish);
            _deallocate(_start, _endOfStorage - _start);
            _start = newStart;
            _finish = newFinish;
            _endOfStorage = newEndOfStorage;
        }
    }

    template<typename T, typename _Alloc>
    template<typename InputIterator>
    inline void Vector<T, _Alloc>::assign(InputIterator first, InputIterator last) {
        _rangeAssign(first, last);
    }

    template<typename T, typename _Alloc>
    inline bool operator==(const Vector<T, _Alloc> &lhs,
                           const Vector<T, _Alloc> &rhs) {
        return lhs.size() == rhs.size() && equal(lhs.cbegin(), lhs.cend(), rhs.cbegin());
    }

    template<typename T, typename _Alloc>
    inline bool operator!=(const Vector<T, _Alloc> &lhs,
                           const Vector<T, _Alloc> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename _Alloc>
    inline bool operator<(const Vector<T, _Alloc> &lhs,
                          const Vector<T, _Alloc> &rhs) {
        return less(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename _Alloc>
    inline bool operator>(const Vector<T, _Alloc> &lhs,
                          const Vector<T, _Alloc> &rhs) {
        return greater(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename _Alloc>
    inline bool operator<=(const Vector<T, _Alloc> &lhs,
                           const Vector<T, _Alloc> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename _Alloc>
    inline bool operator>=(const Vector<T, _Alloc> &lhs,
                           const Vector<T, _Alloc> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T, typename _Alloc>
    Vector<T, _Alloc>& Vector<T, _Alloc>::operator=(const Vector<T,
                                                    _Alloc> &other) {
        if(this != &other) {
            assign(other.cbegin(), other.cend());
        }
        return *this;
    }

}

#endif

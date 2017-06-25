#ifndef LIST_H
#define LIST_H

#include "alloc.h"
#include "iteratortraits.h"
#include "iteratorbase.h"
#include "iterator.h"
#include "algobase.h"
#include "construct.h"
#include "typetraits.h"

namespace tinystl {

    template<typename T>
    struct ListNode {
        ListNode *prev;
        ListNode *next;
        T data;
    };

    template<typename T, typename Ref, typename PointerType>
    class ListIterator {
    public:
        using IteratorCategory = BidirectionalIteratorTag;
        using ValueType = T;
        using DifferenceType = std::ptrdiff_t;
        using Pointer = PointerType;
        using Reference = Ref;

    private:
        using Self = ListIterator<T, Ref, PointerType>;

    public:
        ListIterator();
        ListIterator(ListNode<T> *node);
        // 亮点：这个构造函数主要完成非const类型到const类型的转化。
        // 当类被实例化为const类型时，参数other是非const类型，可将其转化为const类型
        // 当类被实例化为非const类型时，参数other也是非const类型，该函数是一个拷贝构造函数
        ListIterator(const ListIterator<T, typename RemoveConst<Ref>::ResultType, typename RemoveConst<PointerType>::ResultType> &other);
        // 道理同上
        ListIterator& operator=(const ListIterator<T, typename RemoveConst<Ref>::ResultType, typename RemoveConst<PointerType>::ResultType> &other);

        Self& operator++();
        Self operator++(int);
        Self& operator--();
        Self operator--(int);
        Reference operator*() const;
        Pointer operator->() const;

        ListNode<T> *__node;
    };

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>::ListIterator(): __node(nullptr) {}

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>::ListIterator(ListNode<T> *node): __node(node) {}

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>::ListIterator(const ListIterator<T, typename RemoveConst<Ref>::ResultType, typename RemoveConst<PointerType>::ResultType> &other): __node(other.__node) {
    }

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>&
    ListIterator<T, Ref, PointerType>::operator=(const ListIterator<T, typename RemoveConst<Ref>::ResultType, typename RemoveConst<PointerType>::ResultType> &other) {
        __node = other.__node;
    }

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>&
    ListIterator<T, Ref, PointerType>::operator++() {
        __node = __node->next;
        return *this;
    }

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>
    ListIterator<T, Ref, PointerType>::operator++(int) {
        Self temp = *this;
        __node = __node->next;
        return temp;
    }

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>&
    ListIterator<T, Ref, PointerType>::operator--() {
        __node = __node->prev;
        return *this;
    }

    template<typename T, typename Ref, typename PointerType>
    inline ListIterator<T, Ref, PointerType>
    ListIterator<T, Ref, PointerType>::operator--(int) {
        Self temp = *this;
        __node = __node->prev;
        return temp;
    }

    template<typename T, typename Ref, typename PointerType>
    inline typename ListIterator<T, Ref, PointerType>::Reference
    ListIterator<T, Ref, PointerType>::operator*() const {
        return __node->data;
    }

    template<typename T, typename Ref, typename PointerType>
    inline typename ListIterator<T, Ref, PointerType>::Pointer
    ListIterator<T, Ref, PointerType>::operator->() const {
        return &__node->data;
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator==(const ListIterator<T, LRef, LPointerType> &lhs,
                          const ListIterator<T, RRef, RPointerType> &rhs) {
        return lhs.__node == rhs.__node;
    }

    template<typename T, typename LRef, typename LPointerType,
             typename RRef, typename RPointerType>
    inline bool operator!=(const ListIterator<T, LRef, LPointerType> &lhs,
                           const ListIterator<T, RRef, RPointerType> &rhs) {
        return !(lhs == rhs);
    }

    // ----------------------------------------------------------------------
    // List class
    template<typename T, typename Alloc=Alloc>
    class List {
    public:
        using ValueType = T;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
        using Reference = T&;
        using ConstReference = const T&;
        using Pointer = T*;
        using ConstPointer = const T*;

        using Iterator = ListIterator<T, T&, T*>;
        using ConstIterator = ListIterator<T, const T&, const T*>;
        using ReverseIterator = ReverseIteratorTemplate<Iterator>;
        using ConstReverseIterator = ReverseIteratorTemplate<ConstIterator>;

    protected:
        using _Self = List<T, Alloc>;

    public:
        List();
        List(SizeType count, const T &value);
        List(SizeType count);
        template<typename InputIterator>
        List(InputIterator first, InputIterator last);
        List(const _Self &other);
        ~List();

        _Self& operator=(const _Self &other);

        void assign(SizeType count, const T &value);
        template<typename InputIterator>
        void assign(InputIterator first, InputIterator last);

        Reference front() { return *begin(); };
        ConstReference front() const { return *cbegin(); };
        Reference back() { return *(--end()); };
        ConstReference back() const { return *(--cend()); };

        Iterator begin() { return Iterator(__node->next); };
        ConstIterator begin() const { return ConstIterator(__node->next); };
        ConstIterator cbegin() const { return ConstIterator(__node->next); };
        Iterator end() { return Iterator(__node); };
        ConstIterator end() const { return ConstIterator(__node); };
        ConstIterator cend() const { return ConstIterator(__node); };
        ReverseIterator rbegin() { return ReverseIterator(end()); };
        ConstReverseIterator rbegin() const { return ConstReverseIterator(cend()); };
        ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); };
        ReverseIterator rend() { return ReverseIterator(begin()); };
        ConstReverseIterator rend() const { return ConstReverseIterator(cbegin()); };
        ConstReverseIterator crend() const { return ConstReverseIterator(cbegin()); };

        bool empty() const { return __node->next == __node; };
        SizeType size() const { return tinystl::distance(begin(), end()); };
        SizeType maxSize() const { return static_cast<SizeType>(-1) / sizeof(ValueType); };

        void clear() { erase(cbegin(), cend()); };

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

        void resize(SizeType count);
        void resize(SizeType, const T &value);

        void swap(_Self &other);

        void merge(_Self &other);
        template<typename CompareFun>
        void merge(_Self &other, CompareFun comp);

        void splice(ConstIterator pos, _Self &other);
        void splice(ConstIterator pos, _Self &other, ConstIterator it);
        void splice(ConstIterator pos, _Self &other,
                    ConstIterator first, ConstIterator last);

        void remove(const T &value);
        template<typename UnaryPredicate>
        void removeIf(UnaryPredicate p);

        void reverse();
        void unique();
        template<typename UnaryPredicate>
        void unique(UnaryPredicate p);

        void sort();
        template<typename CompareFun>
        void sort(CompareFun comp);

    protected:
        using Allocator = SimpleAlloc<ListNode<T>, Alloc>;
        ListNode<T>* _createANode();
        void _releaseANode(ListNode<T> *ptr);

        template<typename Integer>
        void _rangeAssignAux(Integer count, Integer value, TrueType);
        template<typename InputIterator>
        void _rangeAssignAux(InputIterator first,
                                    InputIterator last, FalseType);

        Iterator _fillInsert(ConstIterator pos, SizeType count, const T &value);
        template<typename InputIterator>
        Iterator _rangeInsert(ConstIterator pos, InputIterator first,
                              InputIterator last);
        template<typename Integer>
        Iterator _rangeInsertAux(ConstIterator pos, Integer count,
                                 Integer value, TrueType);
        template<typename InputIterator>
        Iterator _rangeInsertAux(ConstIterator pos, InputIterator first,
                                 InputIterator last, FalseType);

        void _transfer(ConstIterator pos, ConstIterator first, ConstIterator last);

    private:
        ListNode<T> *__node;
    };

    template<typename T, typename Alloc>
    inline List<T, Alloc>::List() {
        __node = _createANode();
        __node->prev = __node;
        __node->next = __node;
    }

    template<typename T, typename Alloc>
    inline List<T, Alloc>::List(SizeType count, const T &value): List() {
        insert(cend(), count, value);
    }

    template<typename T, typename Alloc>
    inline List<T, Alloc>::List(SizeType count): List(count, T()) {}

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline List<T, Alloc>::List(InputIterator first, InputIterator last): List() {
        insert(cend(), first, last);
    }

    template<typename T, typename Alloc>
    inline List<T, Alloc>::List(const _Self &other): List(other.cbegin(), other.cend()) {}

    template<typename T, typename Alloc>
    inline List<T, Alloc>::~List() {
        clear();
        _releaseANode(__node);
    }

    template<typename T, typename Alloc>
    inline List<T, Alloc>&
    List<T, Alloc>::operator=(const _Self &other) {
        assign(other.cbegin(), other.cend());
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::assign(SizeType count, const T &value) {
        Iterator first = begin();
        while(first != end() && count) {
            *first = value;
            ++first;
            --count;
        }
        if(count) {
            insert(end(), count, value);
        } else {
            erase(first, end());
        }
    }

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline void List<T, Alloc>::assign(InputIterator first, InputIterator last) {
        _rangeAssignAux(first, last, typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename Alloc>
    template<typename Integer>
    inline void List<T, Alloc>::_rangeAssignAux(Integer count, Integer value,
                                                TrueType) {
        assign(static_cast<SizeType>(count), static_cast<T>(value));
    }

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline void List<T, Alloc>::_rangeAssignAux(InputIterator first,
                                                InputIterator last,
                                                FalseType) {
        Iterator it = begin();
        while(it != end() && first != last) {
            *it = *first;
            ++it;
            ++first;
        }
        if(first == last) {
            erase(it, end());
        } else {
            insert(end(), first, last);
        }
    }

    template<typename T, typename Alloc>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::insert(ConstIterator pos, const T &value) {
        ListNode<T> *newNode = _createANode();
        newNode->data = value;
        newNode->prev = pos.__node->prev;
        pos.__node->prev->next = newNode;
        pos.__node->prev = newNode;
        newNode->next = pos.__node;
        return Iterator(newNode);
    }

    template<typename T, typename Alloc>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::insert(ConstIterator pos, SizeType count, const T &value) {
        return _fillInsert(pos, count, value);
    }

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::insert(ConstIterator pos, InputIterator first,
                           InputIterator last) {
        return _rangeInsert(pos, first, last);
    }

    template<typename T, typename Alloc>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::_fillInsert(ConstIterator cpos, SizeType count,
                                const T &value) {
        Iterator pos(cpos.__node);
        if(count == 0) {
            return pos;
        }
        pos = insert(pos, value);
        Iterator res = pos;
        ++pos;
        --count;
        while(count--) {
            pos = insert(pos, value);
            ++pos;
        }
        return res;
    }

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::_rangeInsert(ConstIterator pos, InputIterator first,
                                 InputIterator last) {
        return _rangeInsertAux(pos, first, last,
                               typename IsInteger<InputIterator>::Integral());
    }

    template<typename T, typename Alloc>
    template<typename Integer>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::_rangeInsertAux(ConstIterator cpos, Integer count,
                                    Integer value, TrueType) {
        return _fillInsert(cpos, count, value);
    }

    template<typename T, typename Alloc>
    template<typename InputIterator>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::_rangeInsertAux(ConstIterator cpos, InputIterator first,
                                    InputIterator last, FalseType) {
        Iterator pos(cpos.__node);
        if(first == last) {
            return pos;
        }
        pos = insert(pos, *first);
        Iterator res = pos;
        ++pos;
        ++first;
        while(first != last) {
            pos = insert(pos, *first);
            ++pos;
            ++first;
        }
        return res;
    }

    template<typename T, typename Alloc>
    inline typename List<T, Alloc>::Iterator List<T, Alloc>::erase(ConstIterator pos) {
        ListNode<T> *cur = pos.__node;
        Iterator res(cur->next);
        cur->prev->next = cur->next;
        cur->next->prev = cur->prev;
        destroy(&cur->data);
        _releaseANode(cur);
        return res;
    }

    template<typename T, typename Alloc>
    inline typename List<T, Alloc>::Iterator
    List<T, Alloc>::erase(ConstIterator first, ConstIterator last) {
        ListNode<T> *prev = first.__node->prev;
        ListNode<T> *next = last.__node;
        prev->next = next;
        next->prev = prev;

        while(first != last) {
            ConstIterator next = first;
            ++next;
            destroy(&first.__node->data);
            _releaseANode(first.__node);
            first = next;
        }
        return Iterator(next);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::pushBack(const T &value) {
        insert(cend(), value);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::popBack() {
        erase(--cend());
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::pushFront(const T &value) {
        insert(cbegin(), value);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::popFront() {
        erase(cbegin());
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::resize(SizeType newSize) {
        resize(newSize, ValueType());
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::resize(SizeType newSize, const T &value) {
        const SizeType oldSize = size();
        if(newSize > oldSize) {
            insert(cend(), newSize - oldSize, value);
        } else {
            ConstIterator it = cbegin();
            tinystl::advance(it, newSize);
            erase(it, cend());
        }
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::swap(_Self &other) {
        tinystl::swap(__node, other.__node);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::merge(_Self &other) {
        merge(other, Less<ValueType>());
    }

    template<typename T, typename Alloc>
    template<typename CompareFun>
    inline void List<T, Alloc>::merge(_Self &other, CompareFun comp) {
        ConstIterator first1 = begin();
        ConstIterator last1 = end();
        ConstIterator first2 = other.begin();
        ConstIterator last2 = other.end();
        while(first1 != last1 && first2 != last2) {
            if(comp(*first1, *first2)) {
                ++first1;
            } else {
                ConstIterator next = first2;
                ++next;
                _transfer(first1, first2, next);
                first2 = next;
            }
        }
        if(first2 != last2) {
            _transfer(first1, first2, last2);
        }
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::_transfer(ConstIterator pos,
                                          ConstIterator first, ConstIterator last) {
        ListNode<T> *node = last.__node->prev;
        first.__node->prev->next = last.__node;
        last.__node->prev = first.__node->prev;

        pos.__node->prev->next = first.__node;
        first.__node->prev = pos.__node->prev;
        pos.__node->prev = node;
        node->next = pos.__node;
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::splice(ConstIterator pos, _Self &other) {
        if(other.empty()) {
            return;
        }
        _transfer(pos, other.cbegin(), other.cend());
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::splice(ConstIterator pos, _Self &other, ConstIterator it) {
        ConstIterator next = it;
        ++next;
        if(it == pos || next == pos) {
            return;
        }
        _transfer(pos, it, next);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::splice(ConstIterator pos, _Self &other,
                                       ConstIterator first, ConstIterator last) {
        if(first != last) {
            _transfer(pos, first, last);
        }
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::remove(const T &value) {
        ConstIterator first = cbegin();
        ConstIterator last = cend();
        while(first != last) {
            if(*first == value) {
                first = erase(first);
            } else {
                ++first;
            }
        }
    }

    template<typename T, typename Alloc>
    template<typename UnaryPredicate>
    inline void List<T, Alloc>::removeIf(UnaryPredicate pre) {
        ConstIterator first = cbegin();
        ConstIterator last = cend();
        while(first != last) {
            if(pre(*first)) {
                first = erase(first);
            } else {
                ++first;
            }
        }
    }

    template<typename T, typename Alloc>
    void List<T, Alloc>::reverse() {
        ListNode<T> *temp = __node;
        do {
            tinystl::swap(temp->next, temp->prev);
            temp = temp->prev;
        } while(temp != __node);
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::unique() {
        ConstIterator first = cbegin();
        ConstIterator last = cend();
        ConstIterator next = first;
        while(++next != last) {
            if(*first == *next) {
                erase(next);
            } else {
                first = next;
            }
            next = first;
        }
    }

    template<typename T, typename Alloc>
    template<typename UnaryPredict>
    inline void List<T, Alloc>::unique(UnaryPredict pre) {
        ConstIterator first = cbegin();
        ConstIterator last = cend();
        ConstIterator next = first;
        while(++next != last) {
            if(pre(*first, *next)) {
                erase(next);
            } else {
                first = next;
            }
            next = first;
        }
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::sort() {
        sort(Less<T>());
    }

    template<typename T, typename Alloc>
    template<typename UnaryPredict>
    inline void List<T, Alloc>::sort(UnaryPredict pre) {
        if(empty() || __node->next->next == __node) {
            return;
        }
        List<T, Alloc> carry;
        List<T, Alloc> counter[64];
        int fill = 0;
        while(!empty()) {
            carry.splice(carry.cbegin(), *this, this->cbegin());
            int i = 0;
            while(i < fill && !counter[i].empty()) {
                counter[i].merge(carry);
                carry.swap(counter[i++]);
            }
            counter[i].swap(carry);
            if(i == fill) {
                ++fill;
            }
        }
        for(int i = 1; i < fill; ++i) {
            counter[i].merge(counter[i - 1]);
        }
        swap(counter[fill - 1]);
    }

    template<typename T, typename Alloc>
    inline ListNode<T>* List<T, Alloc>::_createANode() {
        return Allocator::allocate();
    }

    template<typename T, typename Alloc>
    inline void List<T, Alloc>::_releaseANode(ListNode<T> *ptr) {
        Allocator::deallocate(ptr);
    }

    template<typename T, typename Alloc>
    inline bool operator==(const List<T, Alloc> &lhs, const List<T, Alloc> &rhs) {
        return lhs.size() == rhs.size() && equal(lhs.cbegin(), lhs.cend(),
                                                 rhs.cbegin());
    }

    template<typename T, typename Alloc>
    inline bool operator<(const List<T, Alloc> &lhs, const List<T, Alloc> &rhs) {
        return less(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename Alloc>
    inline bool operator>(const List<T, Alloc> &lhs, const List<T, Alloc> &rhs) {
        return greater(lhs.cbegin(), lhs.cend(), rhs.cbegin(), rhs.cend());
    }

    template<typename T, typename Alloc>
    inline bool operator<=(const List<T, Alloc> &lhs, const List<T, Alloc> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename Alloc>
    inline bool operator>=(const List<T, Alloc> &lhs, const List<T, Alloc> &rhs) {
        return !(lhs < rhs);
    }
}

#endif

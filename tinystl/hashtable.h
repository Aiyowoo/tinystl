#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include "vector.h"
#include "pair.h"
#include "alloc.h"

namespace tinystl {

    template<typename Value>
    struct __HashTableNode {
        using ValueType = Value;

        ValueType data;
        __HashTableNode *next;
    };

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    class HashTable;

    template<typename Value, typename Ref, typename PointerType, typename HashTable>
    class __HashTableIterator {
    public:
        using IteratorCategory = ForwardIteratorTag;
        using ValueType = Value;
        using Reference = Ref;
        using Pointer = PointerType;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
        using Iterator = __HashTableIterator<Value, typename RemoveConst<Ref>::ResultType,
                                             typename RemoveConst<Pointer>::ResultType,
                                             HashTable>;
    protected:
        using _Node = __HashTableNode<Value>;
        using _HashTable = HashTable;
    private:
        using __Self = __HashTableIterator<Value, Ref, Pointer, HashTable>;

    public:
        __HashTableIterator(): __hashtable(nullptr), __node(nullptr) {}
        __HashTableIterator(_Node *node, const _HashTable *hashTable)
            :__node(node), __hashtable(hashTable) {}
        __HashTableIterator(const __HashTableIterator<Value, typename RemoveConst<Ref>::ResultType,
                            typename RemoveConst<Pointer>::ResultType, HashTable> &other)
            : __hashtable(other.__hashtable), __node(other.__node) {}
        __Self& operator=(const __Self&) = default;

        Iterator removeConst() const {
            return Iterator(__node, __hashtable);
        }

        Reference operator*() const { return __node->data; }
        Pointer operator->() const { return &__node->data; }
        __Self& operator++() {
            if(__node == nullptr) {
                return *this;
            }
            _Node *next = __node->next;
            if(next) {
                __node = next;
                return *this;
            }
            SizeType currentBucketNo = __hashtable->_computeBucketNoByValueType(__node->data);
            while(++currentBucketNo < __hashtable->bucketCount() &&
                  __hashtable->__buckets[currentBucketNo] == nullptr);
            if(currentBucketNo < __hashtable->bucketCount()) {
                next = __hashtable->__buckets[currentBucketNo];
            }
            __node = next;
            return *this;
        }
        __Self operator++(int) {
            __Self temp = *this;
            operator++();
            return temp;
        }

        const _HashTable *__hashtable;
        _Node *__node;
    };

    template<typename Value,
             typename LRef, typename LPointer, typename LHashTable,
             typename RRef, typename RPointer, typename RHashTable>
    bool operator==(const __HashTableIterator<Value, LRef, LPointer, LHashTable> &lhs,
                    const __HashTableIterator<Value, RRef, RPointer, RHashTable> &rhs) {
        return lhs.__node == rhs.__node;
    }

    template<typename Value,
             typename LRef, typename LPointer, typename LHashTable,
             typename RRef, typename RPointer, typename RHashTable>
    bool operator!=(const __HashTableIterator<Value, LRef, LPointer, LHashTable> &lhs,
                    const __HashTableIterator<Value, RRef, RPointer, RHashTable> &rhs) {
        return !(lhs == rhs);
    }

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    class HashTable {
    public:
        using ValueType = Value;
        using KeyType = Key;
        using Hash = HashFun;
        using EqualKey = EqualFun;
        using ExtractKey = ExtractFun;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
    protected:
        using _Node = __HashTableNode<ValueType>;
        using Allocator = SimpleAlloc<_Node, _Alloc>;
    private:
        using __Self = HashTable<ValueType, KeyType, Hash, ExtractKey, EqualKey, _Alloc>;
    public:
        using Iterator = __HashTableIterator<ValueType, Reference, Pointer, __Self>;
        using ReverseIterator = ReverseIteratorTemplate<Iterator>;
        using ConstIterator = __HashTableIterator<ValueType, ConstReference, ConstPointer, __Self>;
        using ConstReverseIterator = ReverseIteratorTemplate<ConstIterator>;

        friend Iterator;
        friend ConstIterator;

    public:
        HashTable(): __count(0) {}
        HashTable(SizeType bucketCount, const EqualKey &eql, const ExtractKey &ext, const Hash &hash)
            : __buckets(bucketCount), __equalKey(eql),
              __keyExtractor(ext), __hasher(hash) {}
        HashTable(const __Self &other)
            : __buckets(other.__bucketCount, nullptr), __count(other.__count),
              __hasher(other.__hasher), __keyExtractor(other.__keyExtrator),
            __equalKey(other.__equalKey) {
            _copyFrom(other);
        }
        __Self& operator=(const __Self &other) {
            if(this == &other) {
                return *this;
            }
            clear();
            if(bucketCount() < other.bucketCount()) {
                __buckets.assign(other.bucketCount(), nullptr);
            }
            __count = other.__count;
            __hasher = other.__hasher;
            __keyExtractor = other.__keyExtrator;
            __equalKey(other.__equalKey);
            _copyFrom(other);
        }
        ~HashTable() {
            clear();
        }

        SizeType size() const { return __count; }
        SizeType maxSize() const { return primeList[PRIME_COUNT - 1]; }
        bool empty() const { return size() == 0; }

        void swap(__Self &other) {
            using tinystl::swap;
            swap(__buckets, other.__buckets);
            swap(__count, other.__count);
            swap(__hasher, other.__hasher);
            swap(__keyExtractor, other.__keyExtractor);
            swap(__equalKey, other.__equalKey);
        }

        Iterator begin() {
            return Iterator(_getFirstNode(), this);
        }
        ConstIterator begin() const {
            return ConstIterator(_getFirstNode(), this);
        }
        ConstIterator cbegin() const {
            return begin();
        }
        Iterator end() {
            return Iterator(nullptr, this);
        }
        ConstIterator end() const {
            return ConstIterator(nullptr, this);
        }
        ConstIterator cend() const {
            return end();
        }
        ReverseIterator rbegin() {
            return ReverseIterator(end());
        }
        ConstReverseIterator rbegin() const {
            return ConstReverseIterator(end());
        }
        ConstReverseIterator crbegin() const {
            return rbegin();
        }
        ReverseIterator rend() {
            return ReverseIterator(begin());
        }
        ConstReverseIterator rend() const {
            return ConstReverseItearator(begin());
        }
        ConstReverseIterator crend() const {
            return rend();
        }

        SizeType bucketCount() const {
            return __buckets.size();
        }
        SizeType maxBucketCount() const {
            return primeList[PRIME_COUNT - 1];
        }
        SizeType elementCountInSpecificBucket(SizeType bucketNo) const {
            SizeType count = 0;
            _Node *cur = __buckets[bucketNo];
            while(cur) {
                ++count;
                cur->next;
            }
            return count;
        }

        Pair<Iterator, bool> insertUnique(const ValueType &value) {
            _resizeBuckets(__count + 1);
            return insertUniqueNoResize(value);
        }
        Iterator insertEqual(const ValueType &value) {
            _resizeBuckets(__count + 1);
            return insertEqualNoResize(value);
        }
        Pair<Iterator, bool> insertUniqueNoResize(const ValueType &value) {
            const SizeType bucketNo = _computeBucketNoByValueType(value);
            _Node *ptr = __buckets[bucketNo];
            while(ptr) {
                if(__equalKey(__keyExtractor(ptr->data), __keyExtractor(value))) {
                    return makePair(Iterator(ptr, this), false);
                }
                ptr = ptr->next;
            }
            _Node *newNode = _createANode(value);
            newNode->next = __buckets[bucketNo];
            __buckets[bucketNo] = newNode;
            ++__count;
            return makePair(Iterator(newNode, this), true);
        }
        Iterator insertEqualNoResize(const ValueType &value) {
            const SizeType bucketNo = _computeBucketNoByValueType(value);
            _Node *ptr = __buckets[bucketNo];
            while(ptr) {
                if(__equalKey(__keyExtractor(ptr->data), __keyExtractor(value))) {
                    break;
                }
                ptr = ptr->next;
            }
            _Node *newNode = _createANode(value);
            if(ptr) {
                newNode->next = ptr->next;
                ptr->next = newNode;
            } else {
                newNode->next = __buckets[bucketNo];
                __buckets[bucketNo] = newNode;
            }
            ++__count;
            return Iterator(newNode, this);
        }

        template<typename InputIterator>
        void insertUnique(InputIterator first, InputIterator last) {
            _rangeInsertUnique(first, last);
        }
        template<typename InputIterator>
        void insertEqual(InputIterator first, InputIterator last) {
            _rangeInsertEqual(first, last);
        }

        Reference findOrInsert(const ValueType &value) {
        }
        Iterator find(const KeyType &key) {
            const SizeType bucketNo = _computeBucketNoByKeyType(key);
            _Node *ptr = __buckets[bucketNo];
            while(ptr) {
                if(__equalKey(__keyExtractor(ptr->data), key)) {
                    return Iterator(ptr, this);
                }
                ptr = ptr->next;
            }
            return end();
        }
        ConstIterator find(const KeyType &key) const {
            const SizeType bucketNo = _computeBucketNoByKeyType(key);
            _Node *ptr = __buckets[bucketNo];
            while(ptr) {
                if(__equalKey(__keyExtractor(ptr->data), key)) {
                    return ConstIterator(ptr, this);
                }
                ptr = ptr->next;
            }
            return end();
        }
        SizeType count(const KeyType &key) const {
            Pair<ConstIterator, ConstIterator> range = equalRange(key);
            return tinystl::distance(range.first, range.second);
        }
        Pair<Iterator, Iterator>
        equalRange(const KeyType &key) {
            SizeType bucketNo = _computeBucketNoByKeyType(key);
            for(_Node *first = __buckets[bucketNo]; first != nullptr;
                first = first->next) {
                if(__equalKey(__keyExtractor(first->data), key)) {
                    for(_Node *last = first; last != nullptr; last = last->next) {
                        if(!__equalKey(__keyExtractor(last->data), key)) {
                            return makePair(Iterator(first, this), Iterator(last, this));
                        }
                    }
                    while(++bucketNo < bucketCount() && __buckets[bucketNo] == nullptr);
                    if(bucketNo < bucketCount()) {
                        return makePair(Iterator(first, this),
                                        Iterator(__buckets[bucketNo], this));
                    }
                    return makePair(Iterator(first, this), end());
                }
            }
            return makePair(end(), end());
        }
        Pair<ConstIterator, ConstIterator>
        equalRange(const KeyType &key) const {
            SizeType bucketNo = _computeBucketNoByKeyType(key);
            for(_Node *first = __buckets[bucketNo]; first != nullptr;
                first = first->next) {
                if(__equalKey(__keyExtractor(first->data), key)) {
                    for(_Node *last = first; last != nullptr; last = last->next) {
                        if(!__equalKey(__keyExtractor(last->data), key)) {
                            return makePair(ConstIterator(first, this),
                                            ConstIterator(last, this));
                        }
                    }
                    while(++bucketNo < bucketCount() && __buckets[bucketNo] == nullptr);
                    if(bucketNo < bucketCount()) {
                        return makePair(ConstIterator(first, this),
                                        ConstIterator(__buckets[bucketNo], this));
                    }
                    return makePair(ConstIterator(first, this), end());
                }
            }
            return makePair(end(), end());
        }

        SizeType erase(const KeyType &key) {
            const SizeType bucketNo = _computeBucketNoByKeyType(key);
            _Node *ptr = __buckets[bucketNo];
            SizeType count = 0;
            while(ptr) {
                if(__equalKey(__keyExtractor(ptr->data), key)) {
                    _Node *temp = ptr;
                    ptr = ptr->next;
                    _deleteANode(temp);
                    ++count;
                } else {
                    break;
                }
            }
            __buckets[bucketNo] = ptr;
            while(ptr && ptr->next) {
                if(__equalKey(__keyExtractor(ptr->next->data), key)) {
                    _Node *temp = ptr->next;
                    ptr->next = temp->next;
                    _deleteANode(temp);
                    ++count;
                } else {
                    ptr = ptr->next;
                }
            }
            __count -= count;
            return count;
        }
        void erase(Iterator pos) {
            const SizeType bucketNo = _computeBucketNoByValueType(*pos);
            _Node *ptr = __buckets[bucketNo];
            if(ptr == pos.__node) {
                __buckets[bucketNo] = ptr->next;
                _deleteANode(ptr);
                --__count;
                return;
            }
            while(ptr && ptr->next) {
                if(ptr->next == pos.__node) {
                    ptr->next = ptr->next->next;
                    _deleteANode(pos.__node);
                    --__count;
                    return;
                }
            }
        }
        void erase(ConstIterator pos) {
            erase(pos.removeConst());
        }
        void erase(Iterator first, Iterator last) {
            if(first == last) {
                return;
            }
            SizeType elementCount = distance(first, last);
            SizeType firstBucketNo = _computeBucketNoByValueType(*first);
            SizeType lastBucketNo = last == end()? bucketCount():
                _computeBucketNoByValueType(*last);
            if(firstBucketNo == lastBucketNo) {
                _eraseBucket(firstBucketNo, first.__node, last.__node);
            } else {
                _eraseBucket(firstBucketNo, first.__node, nullptr);
                while(++firstBucketNo != lastBucketNo) {
                    _eraseBucket(firstBucketNo);
                }
                if(lastBucketNo < bucketCount()) {
                    _eraseBucket(lastBucketNo, __buckets[lastBucketNo], last.__node);
                }
            }
            __count -= elementCount;
        }
        void erase(ConstIterator first, ConstIterator last) {
            erase(first.removeConst(), last.removeConst());
        }

        void resize(SizeType elementCountHint) {
            _resizeBuckets(elementCountHint);
        }
        void clear() {
            erase(begin(), end());
        }

    protected:
        SizeType _nextSize(SizeType currentSize) const;
        void _initializeBuckets(SizeType n);
        SizeType _computeBucketNoByKeyType(const KeyType &key) const {
            return __hasher(key) % bucketCount();
        }
        SizeType _computeBucketNoByValueType(const ValueType &value) const {
            return __hasher(__keyExtractor(value)) % bucketCount();
        }

        _Node* _createANode(const ValueType &value) const {
            _Node *ptr = Allocator::allocate();
            construct(&ptr->data, value);
            return ptr;
        }
        void _deleteANode(_Node *ptr) const {
            destroy(&ptr->data);
            Allocator::deallocate(ptr);
        }

        void _copyFromSameBucketCount(const __Self &other) {
            SizeType currentBucketNo = 0;
            while(currentBucketNo < other.bucketCount()) {
                _Node *cur = other.__buckets[currentBucketNo];
                while(cur) {
                    _Node *newNode = _createANode(cur->data);
                    newNode->next = __buckets[currentBucketNo];
                    __buckets[currentBucketNo] = newNode;
                    cur = cur->next;
                }
                ++currentBucketNo;
            }
        }
        void _copyFrom(const __Self &other) {
            SizeType bucketNo = 0;
            while(bucketNo < other.bucketCount()) {
                _Node *cur = other.__buckets[bucketNo];
                while(cur) {
                    const SizeType targetBucketNo = _computeBucketNoByValueType(cur->data);
                    _Node *newNode = _createANode(cur->data);
                    newNode->next = __buckets[targetBucketNo];
                    __buckets[targetBucketNo] = newNode;
                    cur = cur->next;
                }
                ++bucketNo;
            }
        }
        _Node* _getFirstNode() const {
            SizeType currentBucketNo = 0;
            while(currentBucketNo < bucketCount()) {
                if(__buckets[currentBucketNo]) {
                    return __buckets[currentBucketNo];
                }
                ++currentBucketNo;
            }
            return nullptr;
        }
        void _resizeBuckets(const SizeType newElementCount) {
            if(bucketCount() >= newElementCount) {
                return;
            }
            int i = 0;
            while(i < PRIME_COUNT) {
                if(primeList[i] >= newElementCount) {
                    break;
                }
                ++i;
            }
            i = tinystl::min(PRIME_COUNT - 1, i);
            const SizeType newSize = primeList[i];
            Vector<_Node*, _Alloc> newBuckets(newSize, nullptr);
            for(SizeType currentBucketNo = 0; currentBucketNo < bucketCount();
                ++currentBucketNo) {
                _Node *ptr = __buckets[currentBucketNo];
                while(ptr) {
                    _Node *temp = ptr;
                    ptr = ptr->next;
                    const SizeType targetBucketNo = _computeBucketNoByValueType(temp->data);
                    temp->next = newBuckets[targetBucketNo];
                    newBuckets[targetBucketNo] = temp;
                }
            }
            using tinystl::swap;
            swap(__buckets, newBuckets);
        }

        template<typename InputIterator>
        void _rangeInsertUnique(InputIterator first, InputIterator last) {
            using IteratorCategory = typename IteratorTraits<InputIterator>::IteratorCategory;
            _rangeInsertUniqueAux(first, last, IteratorCategory());
        }
        template<typename InputIterator>
        void _rangeInsertUniqueAux(InputIterator first, InputIterator last,
                                   InputIteratorTag) {
            while(first != last) {
                insertUnique(*first);
                ++first;
            }
        }
        template<typename ForwardIterator>
        void _rangeInsertUniqueAux(ForwardIterator first, ForwardIterator last,
                                   ForwardIteratorTag) {
            _resizeBuckets(__count + tinystl::distance(first, last));
            while(first != last) {
                insertUniqueNoResize(*first);
                ++first;
            }
        }
        template<typename InputIterator>
        void _rangeInsertEqual(InputIterator first, InputIterator last) {
            using IteratorCategory = typename IteratorTraits<InputIterator>::IteratorCategory;
            _rangeInsertEqualAux(first, last, IteratorCategory());
        }
        template<typename InputIterator>
        void _rangeInsertEqualAux(InputIterator first, InputIterator last,
                                  InputIteratorTag) {
            while(first != last) {
                insertEqual(*first);
                ++first;
            }
        }
        template<typename ForwardIterator>
        void _rangeInsertEqualAux(ForwardIterator first, ForwardIterator last) {
            _resizeBuckets(__count + tinystl::distance(first, last));
            while(first != last) {
                insertEqualNoResize(*first);
                ++first;
            }
        }

        void _deleteAListNode(_Node *first, _Node *last) {
            while(first != last) {
                _deleteANode(first);
                first = first->next;
            }
        }

        void _eraseBucket(SizeType bucketNo, _Node *first, _Node *last) {
            _Node *ptr = __buckets[bucketNo];
            if(ptr == first) {
                __buckets[bucketNo] = last;
            } else {
                while(ptr && ptr->next != first) {
                    ptr = ptr->next;
                }
                ptr->next = last;
            }
            _deleteAListNode(first, last);
        }
        void _eraseBucket(SizeType bucketNo) {
            _eraseBucket(bucketNo, __buckets[bucketNo], nullptr);
        }

    private:
        Vector<_Node*, _Alloc> __buckets;
        SizeType __count;
        Hash __hasher;
        ExtractKey __keyExtractor;
        EqualKey __equalKey;

        enum { PRIME_COUNT = 28 };
        static const unsigned long primeList[PRIME_COUNT];
    };

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    const unsigned long HashTable<Value, Key, HashFun, ExtractFun,
                                  EqualFun, _Alloc>::primeList[PRIME_COUNT] = {
        53ul,         97ul,         193ul,       389ul,       769ul,
        1543ul,       3079ul,       6151ul,      12289ul,     24593ul,
        49157ul,      98317ul,      196613ul,    393241ul,    786433ul,
        1572869ul,    3145739ul,    6291469ul,   12582917ul,  25165843ul,
        50331653ul,   100663319ul,  201326611ul, 402653189ul, 805306457ul, 
        1610612741ul, 3221225473ul, 4294967291ul
    };

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    inline bool operator==(const HashTable<Value, Key, HashFun, ExtractFun,
                           EqualFun, _Alloc> &lhs,
                           const HashTable<Value, Key, HashFun, ExtractFun,
                           EqualFun, _Alloc> &rhs) {
        return lhs.size() == rhs.size() &&
            equal(lhs.begin(), lhs.end(), rhs.begin());
    }

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    inline bool operator!=(const HashTable<Value, Key, HashFun, ExtractFun,
                           EqualFun, _Alloc> &lhs,
                           const HashTable<Value, Key, HashFun, ExtractFun,
                           EqualFun, _Alloc> &rhs) {
        return !(lhs == rhs);
    }

    template<typename Value, typename Key, typename HashFun,
             typename ExtractFun, typename EqualFun, typename _Alloc>
    inline void swap(HashTable<Value, Key, HashFun, ExtractFun, EqualFun, _Alloc> &lhs,
              HashTable<Value, Key, HashFun, ExtractFun, EqualFun, _Alloc> &rhs) {
        lhs.swap(rhs);
    }

}

#endif

#ifndef RBTREE_H
#define RBTREE_H

#include "construct.h"
#include "pair.h"
#include "alloc.h"
#include "algobase.h"

namespace tinystl {

    using __RBTreeNodeColorType = bool;
    const __RBTreeNodeColorType red = true;
    const __RBTreeNodeColorType black = false;

    struct __RBTreeNodeBase {
        using _BasePtr = __RBTreeNodeBase*;

        static __RBTreeNodeBase* getMinNode(__RBTreeNodeBase *root) {
            if(!root) {
                return nullptr;
            }
            while(root->left) {
                root = root->left;
            }
            return root;
        }

        static __RBTreeNodeBase* getMaxNode(__RBTreeNodeBase *root) {
            if(!root) {
                return nullptr;
            }
            while(root->right) {
                root = root->right;
            }
            return root;
        }

        __RBTreeNodeBase *parent;
        __RBTreeNodeBase *left;
        __RBTreeNodeBase *right;
        __RBTreeNodeColorType color;
    };

    template<typename T>
    struct __RBTreeNode: public __RBTreeNodeBase {
        using _LinkType = __RBTreeNode<T>*;
        T data;
    };

    void __leftRotate(__RBTreeNodeBase* &root, __RBTreeNodeBase *x) {
        // 没有右孩子就没有什么好转的
        if(!x->right) {
            return;
        }
        __RBTreeNodeBase *xRightSon = x->right;
        xRightSon->parent = x->parent;
        if(root == x) {
            root = xRightSon;
        } else if(x == x->parent->left) {
            x->parent->left = xRightSon;
        } else {
            x->parent->right = xRightSon;
        }
        x->right = xRightSon->left;
        if(x->right) {
            x->right->parent = x;
        }
        xRightSon->left = x;
        x->parent = xRightSon;

    }

    void __rightRotate(__RBTreeNodeBase* &root, __RBTreeNodeBase *x) {
        // 没有左孩子不能转
        if(!x->left) {
            return;
        }
        __RBTreeNodeBase *xLeftSon = x->left;
        xLeftSon->parent = x->parent;
        if(root == x) {
            root = xLeftSon;
        } else if(x == x->parent->left) {
            x->parent->left = xLeftSon;
        } else {
            x->parent->right = xLeftSon;
        }
        x->left = xLeftSon->right;
        if(x->left) {
            x->left->parent = x;
        }
        xLeftSon->right = x;
        x->parent = xLeftSon;
    }

    void __rebalanceTreeAfterInsert(__RBTreeNodeBase* &root,
                                    __RBTreeNodeBase *x) {
        // x表示已经插入到以root为根的树中的节点
        x->color = red;
        while(x != root && x->parent->color == red) {
            if(x->parent == x->parent->parent->left) {
                __RBTreeNodeBase *y = x->parent->parent->right;
                if(y && y->color == red) {
                    y->parent->color = red;
                    y->color = black;
                    y->parent->left->color = black;
                    x = y->parent;
                } else {
                    if(x == x->parent->right) {
                        x = x->parent;
                        __leftRotate(root, x);
                    }
                    x->parent->color = black;
                    x->parent->parent->color = red;
                    __rightRotate(root, x->parent->parent);
                }
            } else {
                __RBTreeNodeBase *y = x->parent->parent->left;
                if(y && y->color == red) {
                    y->parent->color = red;
                    y->color = black;
                    y->parent->right->color = black;
                    x = y->parent;
                } else {
                    if(x == x->parent->left) {
                        x = x->parent;
                        __rightRotate(root, x);
                    }
                    x->parent->parent->color = red;
                    x->parent->color = black;
                    __leftRotate(root, x->parent->parent);
                }
            }
        }
        root->color = black;
    }

    void __rebalanceTreeAfterDelete(__RBTreeNodeBase* &root,
                                    __RBTreeNodeBase *x,
                                    __RBTreeNodeBase *xParent) {
        // x表示用来替代已删除那个节点的点
        // x以正确插入
        while(x != root && (x == nullptr || x->color == black)) {
            if(x == xParent->left) {
                // w一定不为nullptr，因为在xParent的一个子树中删除了一个黑色节点才
                // 会调用本函数，所以对应的另一颗子树中一定有一个黑色节点，所以w一定不为空
                __RBTreeNodeBase *w = xParent->right;
                if(w->color == red) {
                    w->color = black;
                    xParent->color = red;
                    __leftRotate(root, xParent);
                    w = xParent->right;
                }
                if((w->left == nullptr || w->left->color == black) &&
                   (w->right == nullptr || w->right->color == black)) {
                    w->color = red;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if(w->right == nullptr || w->right->color == black) {
                        if(w->left) {
                            w->left->color = black;
                        }
                        w->color = red;
                        __rightRotate(root, w);
                        w = xParent->right;
                    }
                    w->color = xParent->color;
                    xParent->color = black;
                    w->right->color = black;
                    __leftRotate(root, xParent);
                    x = root;
                }
            } else {
                __RBTreeNodeBase *w = xParent->left;
                if(w->color == red) {
                    w->color = black;
                    xParent->color = red;
                    __rightRotate(root, xParent);
                    w = xParent->left;
                }
                if((w->left == nullptr || w->left->color == black) &&
                   (w->right == nullptr || w->right->color == black)) {
                    w->color = red;
                    x = xParent;
                    xParent = x->parent;
                } else {
                    if(w->left == nullptr || w->left->color == black) {
                        if(w->right) {
                            w->right->color = black;
                        }
                        w->color = red;
                        __leftRotate(root, w);
                        w = xParent->left;
                    }
                    w->color = xParent->color;
                    xParent->color = black;
                    w->left->color = black;
                    __rightRotate(root, xParent);
                    x = root;
                }
            }
        }
        if(x) {
            x->color = black;
        }
    }

    __RBTreeNodeBase* __deleteANode(__RBTreeNodeBase* &root,
                                    __RBTreeNodeBase *z,
                                    __RBTreeNodeBase* &leftMost,
                                    __RBTreeNodeBase* &rightMost) {
        // 返回指向删除节点的指针
        // 因为该函数并不知道__RBTreeNodeBase所指的具体对象是什么,
        // 不应该承担回收该对象的责任
        __RBTreeNodeBase *y = z;
        __RBTreeNodeBase *x = nullptr;
        __RBTreeNodeBase *xParent = nullptr;
        if(!y->left) {
            x = y->right;
        } else {
            if(!y->right) {
                x = y->left;
            } else {
                y = y->right;
                while(y->left) {
                    y = y->left;
                }
                x = y->right;
            }
        }
        // y != z说明要删除的节点z有两个孩子，应使用z的后继y来顶替z。
        if(y != z) {
            y->left = z->left;
            z->left->parent = y;
            if(y != z->right) {
                xParent = y->parent;
                if(x) {
                    x->parent = xParent;
                }
                xParent->left = x;
                y->right = z->right;
                z->right->parent = y;
            } else {
                xParent = y;
            }
            y->parent = z->parent;
            if(root == z) {
                root = y;
            } else if(z == z->parent->left) {
                z->parent->left = y;
            } else {
                z->parent->right = y;
            }
            tinystl::swap(y->color, z->color);
            y = z;
        } else {
            xParent = y->parent;
            if(x) {
                x->parent = xParent;
            }
            if(root == y) {
                root = x;
            } else if(y == y->parent->left) {
                y->parent->left = x;
            } else {
                y->parent->right = x;
            }
            // 更新leftmost,rightmost;
            if(leftMost == z) {
                if(z->right) {
                    leftMost = __RBTreeNodeBase::getMinNode(x);
                } else {
                    leftMost = z->parent;
                }
            }
            if(rightMost == z) {
                if(z->left) {
                    rightMost = __RBTreeNodeBase::getMaxNode(x);
                } else {
                    rightMost = z->parent;
                }
            }
        }
        if(y->color == black) {
            __rebalanceTreeAfterDelete(root, x, xParent);
        }
        return y;
    }

    // ----------------------------------------------------------------------
    // IteratorBase
    struct __RBTreeIteratorBase {
        using IteratorCategory = BidirectionalIteratorTag;
        using _BasePtr = __RBTreeNodeBase::_BasePtr;

        void _increment() {
            if(_node->right) {
                _node = _node->right;
                while(_node->left) {
                    _node = _node->left;
                }
                return;
            }
            while(_node == _node->parent->right) {
                _node = _node->parent;
            }
            // 这里是为了区分root和header
            // 因为循环退出是，_node可能为header
            if(_node->right != _node->parent) {
                _node = _node->parent;
            }
        }

        void _decrement() {
            if(_node->color == red &&
               _node->parent->parent == _node) {
                _node = _node->right;
                return;
            }
            if(_node->left) {
                _node = _node->left;
                while(_node->right) {
                    _node = _node->right;
                }
                return;
            }
            while(_node == _node->parent->left) {
                _node = _node->parent;
            }
            _node = _node->parent;
        }

        _BasePtr _node;
    };

    // ----------------------------------------------------------------------
    // RBTreeIteratorTemplate

    template<typename T, typename Ref, typename PointerType>
    struct __RBTreeIteratorTemplate: public __RBTreeIteratorBase {
        using ValueType = T;
        using Reference = Ref;
        using DifferenceType = std::ptrdiff_t;
        using Pointer = PointerType;
        using Iterator = __RBTreeIteratorTemplate<T, T&, T*>;
        using ConstIterator = __RBTreeIteratorTemplate<T, const T&, const T*>;
        using _Self = __RBTreeIteratorTemplate<T, Ref, PointerType>;
        using _LinkType = __RBTreeNode<T>*;

        __RBTreeIteratorTemplate() = default;
        __RBTreeIteratorTemplate(_LinkType x) {
            _node = x;
        }
        __RBTreeIteratorTemplate(const Iterator &other) {
            _node = other._node;
        }

        Reference operator*() const {
            return static_cast<_LinkType>(_node)->data;
        }
        Pointer operator->() const {
            return &(operator*());
        }
        _Self& operator++() {
            _increment();
            return *this;
        }
        _Self operator++(int) {
            _Self temp = *this;
            operator++();
            return temp;
        }
        _Self& operator--() {
            _decrement();
            return *this;
        }
        _Self operator--(int) {
            _Self temp = *this;
            operator--();
            return temp;
        }
        Iterator removeConst() const {
            return Iterator(reinterpret_cast<_LinkType>(_node));
        }
    };

    inline bool operator==(const __RBTreeIteratorBase &lhs,
                           const __RBTreeIteratorBase &rhs) {
        return lhs._node == rhs._node;
    }

    inline bool operator!=(const __RBTreeIteratorBase &lhs,
                           const __RBTreeIteratorBase &rhs) {
        return !(lhs == rhs);
    }

    // RBTreeBase
    template<typename T, typename _Alloc>
    struct __RBTreeBase {
        __RBTreeBase(): _header(nullptr) {
            _header = _createANode();
        }
        ~__RBTreeBase() {
            _releaseANode(_header);
        }
    protected:
        using Allocator = SimpleAlloc<__RBTreeNode<T>, _Alloc>;
        __RBTreeNode<T>* _createANode() {
            return Allocator::allocate();
        }
        void _releaseANode(__RBTreeNode<T> *ptr) {
            Allocator::deallocate(ptr);
        }

        __RBTreeNode<T> *_header;
    };

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc=Alloc>
    class RBTree: protected __RBTreeBase<Value, _Alloc> {
    public:
        using KeyType = Key;
        using ValueType = Value;
        using Pointer = ValueType*;
        using ConstPointer = const ValueType*;
        using Reference = ValueType&;
        using ConstReference = const ValueType&;
        using _LinkType = __RBTreeNode<ValueType>*;
        using SizeType = std::size_t;
        using DifferenceType = std::ptrdiff_t;
    protected:
        using _BasePtr = __RBTreeNodeBase*;
        using _RBTreeNode = __RBTreeNode<ValueType>;
        using ColorType = __RBTreeNodeColorType;
    private:
        using __Base = __RBTreeBase<ValueType, _Alloc>;

    protected:
        using __Base::_createANode;
        using __Base::_releaseANode;
        using __Base::_header;

    protected:
        _LinkType _createANode(const ValueType &value) {
            _LinkType ptr = _createANode();
            try {
                construct(&ptr->data, value);
            } catch(...) {
                _releaseANode(ptr);
                throw;
            }
            return ptr;
        }

        _LinkType _cloneANode(_LinkType other) {
            _LinkType ptr = _createANode(other->data);
            ptr->color = other->color;
            ptr->parent = other->parent;
            ptr->left = other->left;
            ptr->right = other->right;
            return ptr;
        }

        _LinkType _destroyANode(_LinkType ptr) {
            destroy(&ptr->data);
            _releaseANode(ptr);
        }

    protected:
        SizeType _nodeCount;
        Compare _key_comparer;

        _LinkType& _root() const { return reinterpret_cast<_LinkType&>(_header->parent); }
        _LinkType& _leftMost() const { return reinterpret_cast<_LinkType&>(_header->left); }
        _LinkType& _rightMost() const { return reinterpret_cast<_LinkType&>(_header->right); }

        static _LinkType& _left(_LinkType x) { return reinterpret_cast<_LinkType&>(x->left); }
        static _LinkType& _right(_LinkType x) { return reinterpret_cast<_LinkType&>(x->right); }
        static _LinkType& _parent(_LinkType x) { return reinterpret_cast<_LinkType&>(x->parent); }
        static Reference _value(_LinkType x) { return x->data; }
        static const Key& _key(_LinkType x) { return KeyOfValue()(x->data); }
        static ColorType& _color(_LinkType x) { return x->color; }

        static _LinkType& _left(_BasePtr x) { return reinterpret_cast<_LinkType&>(x->left); }
        static _LinkType& _right(_BasePtr x) { return reinterpret_cast<_LinkType&>(x->right); }
        static _LinkType& _parent(_BasePtr x) { return reinterpret_cast<_LinkType&>(x->parent); }
        static Reference _value(_BasePtr x) { return _value(reinterpret_cast<_LinkType>(x)); }
        static const Key& _key(_BasePtr x) { return _key(reinterpret_cast<_LinkType>(x)); }
        static ColorType& _color(_BasePtr x) { return x->color; }

        static _LinkType _minimum(_LinkType x) {
            return reinterpret_cast<_LinkType>(__RBTreeNodeBase::getMinNode(x));
        }
        static _LinkType _maximum(_LinkType x) {
            return reinterpret_cast<_LinkType>(__RBTreeNodeBase::getMaxNode(x));
        }

    public:
        using Iterator = __RBTreeIteratorTemplate<ValueType, Reference,
                                                  Pointer>;
        using ConstIterator = __RBTreeIteratorTemplate<ValueType,
                                                       ConstReference,
                                                       ConstPointer>;
        using ReverseIterator = ReverseIteratorTemplate<Iterator>;
        using ConstReverseIterator = ReverseIteratorTemplate<ConstIterator>;

    private:
        using __Self = RBTree<Key, Value, KeyOfValue, Compare, _Alloc>;
        Iterator __insert(_BasePtr x, _BasePtr y, const ValueType &v);
        _LinkType __copy(_LinkType src, _LinkType top);
        void __erase(_LinkType root);

    public:
        RBTree(): _nodeCount(0), _key_comparer() { __emptyInitialize(); }
        RBTree(const Compare &compare): _nodeCount(0), _key_comparer() {
            __emptyInitialize();
        }
        RBTree(const __Self &other): _nodeCount(0),
                                     _key_comparer(other._key_comparer) {
            if(other.empty()) {
                __emptyInitialize();
            } else {
                _color(_header) = red;
                _root() = __copy(other._root(), _header);
                _leftMost() = _minimum(_root());
                _rightMost() = _maximum(_root());
                _nodeCount = other._nodeCount;
            }
        }
        ~RBTree() { clear(); }

        __Self& operator=(const __Self &other) {
            if(this == &other) {
                return *this;
            }
            clear();
            _nodeCount = 0;
            _key_comparer = other._key_comparer;
            if(other.empty()) {
                _root() = nullptr;
                _leftMost() = _header;
                _rightMost() = _header;
            } else {
                _root() = __copy(other._root(), _header);
                _leftMost() = _minimum(_root());
                _rightMost() = _maximum(_root());
                _nodeCount = other._nodeCount;
            }
            return *this;
        }

    private:
        void __emptyInitialize() {
            _root() = nullptr;
            _leftMost() = _header;
            _rightMost() = _header;
            _color(_header) = red;
        }

    public:
        Compare keyCompare() const { return _key_comparer; }
        Iterator begin() { return Iterator(_leftMost()); }
        ConstIterator begin() const { return ConstIterator(_leftMost()); }
        ConstIterator cbegin() const { return ConstIterator(_leftMost()); }
        Iterator end() { return Iterator(_header); }
        ConstIterator end() const { return ConstIterator(_header); }
        ConstIterator cend() const { return ConstIterator(_header); }
        ReverseIterator rbegin() { return ReverseIterator(end()); }
        ConstReverseIterator rbegin() const { return ConstReverseIterator(cend()); }
        ConstReverseIterator crbegin() const { return ConstReverseIterator(cend()); }
        ReverseIterator rend() { return ReverseIterator(begin()); }
        ConstReverseIterator rend() const { return ConstReverseIterator(begin()); }
        ConstReverseIterator crend() const { return ConstReverseIterator(begin()); }
        bool empty() const { return _root() == nullptr; }
        SizeType size() const { return _nodeCount; }
        SizeType maxSize() const { return static_cast<SizeType>(-1); }

        void swap(__Self &other) {
            using tinystl::swap;
            swap(_header, other._header);
            swap(_nodeCount, other._nodeCount);
            swap(_key_comparer, other._key_comparer);
        }

    public:
        Pair<Iterator, bool> insertUnique(const ValueType &value);
        Iterator insertEqual(const ValueType &value);
        Iterator insertUnique(Iterator pos, const ValueType &value);
        Iterator insertEqual(Iterator pos, const ValueType &value);
        template<typename InputIterator>
        void insertUnique(InputIterator first, InputIterator last);
        template<typename InputIterator>
        void insertEqual(InputIterator first, InputIterator last);

        void erase(Iterator pos);
        SizeType erase(const KeyType &key);
        void erase(Iterator first, Iterator last);
        void erase(const KeyType *first, const KeyType *last);
        void clear();

        Iterator find(const KeyType &key);
        ConstIterator find(const KeyType &key) const;
        SizeType count(const KeyType &key) const;
        Iterator lowerBound(const KeyType &key);
        ConstIterator lowerBound(const KeyType &key) const;
        Iterator upperBound(const KeyType &key);
        ConstIterator upperBound(const KeyType &key) const;
        Pair<Iterator, Iterator> equalRange(const KeyType &key);
        Pair<ConstIterator, ConstIterator> equalRange(const KeyType &key) const;

        bool rbVerify() const;
    protected:
        SizeType _blackCount(_BasePtr leaf, _BasePtr root) const;
    };

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::_LinkType
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::__copy(_LinkType src,
                                                            _LinkType dest) {
        // 递归的将src拷到dest下
        _LinkType top = _cloneANode(src);
        _parent(top) = dest;
        try {
            if(_right(top)) {
                _right(top) = __copy(_right(src), top);
            }
            src = _left(src);
            dest = top;
            while(src) {
                _LinkType y = _cloneANode(src);
                _parent(y) = dest;
                _left(dest) = y;
                if(_right(src)) {
                    _right(y) = __copy(_right(src), y);
                }
                src = _left(src);
                dest = y;
            }
        } catch(...) {
            __erase(top);
            throw;
        }
        return top;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::__erase(_LinkType root) {
        while(root) {
            __erase(_right(root));
            _LinkType leftSon = _left(root);
            _destroyANode(root);
            root = leftSon;
        }
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::__insert(_BasePtr x,
                                                              _BasePtr p,
                                                              const ValueType &value) {
        _LinkType newNode = _createANode(value);
        if(p == _header || x ||
           _key_comparer(KeyOfValue()(value), _key(p))) {
            _left(p) = newNode;
            if(p == _header) {
                _root() = newNode;
                _rightMost() = newNode;
            } else if(p == _leftMost()) {
                _leftMost() = newNode;
            }
        } else {
            _right(p) = newNode;
            if(_rightMost() == p) {
                _rightMost() = newNode;
            }
        }
        newNode->parent = p;
        _left(newNode) = nullptr;
        _right(newNode) = nullptr;
        _color(newNode) = red;
        __rebalanceTreeAfterInsert(_header->parent, newNode);
        ++_nodeCount;
        return Iterator(newNode);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    Pair<typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator, bool>
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::insertUnique(const ValueType &value) {
        _LinkType cur = _header;
        _LinkType next = _root();
        if(next == nullptr) {
            return Pair<Iterator, bool>(__insert(next, cur, value), true);
        }

        while(next) {
            cur = next;
            next = _key_comparer(KeyOfValue()(value), _key(next))?
                _left(next): _right(next);
        }
        // 为了判断value是否和某个值相等
        // 如果大于当前值，则不可能有相等的
        // 如果小于当前值，也有可能等于前一个
        Iterator prev = Iterator(cur);
        if(_key_comparer(KeyOfValue()(value), _key(cur))) {
            if(prev == begin()) {
                return Pair<Iterator, bool>(__insert(next, cur, value), true);
            }
            --prev;
        }
        if(_key_comparer(_key(prev._node), KeyOfValue()(value))) {
            return Pair<Iterator, bool>(__insert(next, cur, value), true);
        }
        // 其他情况表示已经存在
        return Pair<Iterator, bool>(prev, false);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::insertUnique(Iterator pos, const ValueType &value) {
        if(pos == begin()) {
            if(size() > 0 && _key_comparer(KeyOfValue()(value), _key(pos._node))) {
                return __insert(pos._node, pos._node, value);
            } else {
                return insertUnique(value).first;
            }
        } else if(pos == end()) {
            if(size() > 0 && _key_comparer(_key(_rightMost()), KeyOfValue(value))) {
                return __insert(nullptr, _rightMost(), value);
            } else {
                return insertUnique(value).first;
            }
        } else {
            Iterator prev = pos;
            --prev;
            if(_key_comparer(_key(prev._node), KeyOfValue()(value)) &&
               _key_comparer(KeyOfValue()(value), _key(pos._node))) {
                if(_right(prev._node) == nullptr) {
                    return __insert(nullptr, prev._node, value);
                } else if(_left(pos._node) == nullptr) {
                    return __insert(pos._node, pos._node, value);
                }
            }
        }
        return insertUnique(value).first;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::insertEqual(const ValueType &value) {
        _LinkType cur = _header;
        _LinkType next = _root();
        while(next) {
            cur = next;
            next = _key_comparer(KeyOfValue()(value), _key(next))?
                _left(cur): _right(cur);
        }
        return __insert(next, cur, value);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::insertEqual(Iterator pos,
                                                                 const ValueType &value) {
        if(pos == begin()) {
            if(size() > 0 && !_key_comparer(_key(pos._node), KeyOfValue()(value))) {
                return __insert(pos._node, pos._node, value);
            } else {
                return insertEqual(value);
            }
        } else if(pos == end()) {
            if(size() > 0 && !_key_comparer(KeyOfValue()(value), _key(_right()))) {
                return __insert(nullptr, pos._node, value);
            } else {
                return insertEqual(value);
            }
        } else {
            Iterator prev = pos;
            --prev;
            if(!_key_comparer(KeyOfValue()(value), _key(prev._node)) &&
               !_key_comparer(_key(pos._node), KeyOfValue()(value))) {
                if(_right(prev._node) == nullptr) {
                    return __insert(nullptr, prev._node, value);
                } else if(_left(pos._node) == nullptr) {
                    return __insert(pos._node, pos._node, value);
                }
            }
        }
        return insertEqual(value);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    template<typename InputIterator>
    inline void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::
    insertUnique(InputIterator first, InputIterator last) {
        while(first != last) {
            insertUnique(*first++);
        }
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    template<typename InputIterator>
    inline void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::
    insertEqual(InputIterator first, InputIterator last) {
        while(first != last) {
            insertEqual(*first++);
        }
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::
    erase(Iterator pos) {
        _BasePtr ptr = __deleteANode(_header->parent, pos._node,
                                     _header->left, _header->right);
        _destroyANode(static_cast<_LinkType>(ptr));
        --_nodeCount;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::SizeType
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::
    erase(const KeyType &key) {
        Pair<Iterator, Iterator> range = equalRange(key);
        SizeType count = tinystl::distance(range.first, range.second);
        erase(range.first, range.second);
        return count;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::
    erase(Iterator first, Iterator second) {
        if(first == begin() and second == end()) {
            clear();
        } else {
            while(first != second) {
                erase(first++);
            }
        }
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline void RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::clear() {
        __erase(_root());
        _root() = nullptr;
        _leftMost() = _header;
        _rightMost() = _header;
        _nodeCount = 0;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::find(const KeyType &key) {
        return static_cast<const __Self* const>(this)->find(key).removeConst();
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::ConstIterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::find(const KeyType &key) const {
        _LinkType target = _header;
        _LinkType cur = _root();
        while(cur) {
            if(!_key_comparer(_key(cur), key)) {
                target = cur;
                cur = _left(cur);
            } else {
                cur = _right(cur);
            }
        }
        Iterator res(target);
        return (res == end() || _key_comparer(key, _key(res._node)))?
            end(): res;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::SizeType
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::count(const KeyType &key) const {
        Pair<ConstIterator, ConstIterator> range = equalRange(key);
        return tinystl::distance(range.first, range.second);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::lowerBound(const KeyType &key) {
        return static_cast<const __Self* const>(this)->lowerBound(key).removeConst();
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::ConstIterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::lowerBound(const KeyType &key) const {
        _LinkType rangeFirst = _header;
        _LinkType cur = _root();
        while(cur) {
            if(!_key_comparer(_key(cur), key)) {
                rangeFirst = cur;
                cur = _left(cur);
            } else {
                cur = _right(cur);
            }
        }
        return ConstIterator(rangeFirst);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::upperBound(const KeyType &key) {
        return static_cast<const __Self* const>(this)->upperBound(key).removeConst();
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::ConstIterator
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::upperBound(const KeyType &key) const {
        _LinkType rangeLast = _header;
        _LinkType cur = _root();
        while(cur) {
            if(_key_comparer(key, _key(cur))) {
                rangeLast = cur;
                cur = _left(cur);
            } else {
                cur = _right(cur);
            }
        }
        return ConstIterator(rangeLast);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline Pair<typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator,
                typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::Iterator>
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::equalRange(const KeyType &key) {
        return makePair(lowerBound(key), upperBound(key));
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline Pair<typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::ConstIterator,
                typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::ConstIterator>
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::equalRange(const KeyType &key) const {
        return makePair(lowerBound(key), upperBound(key));
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline typename RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::SizeType
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::_blackCount(_BasePtr leaf, _BasePtr root) const {
        if(!leaf) {
            return 0;
        }
        SizeType count = 0;
        while(leaf != root) {
            count += _color(leaf) == black? 1: 0;
            leaf = _parent(leaf);
        }
        return count;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool
    RBTree<Key, Value, KeyOfValue, Compare, _Alloc>::rbVerify() const {
        if(_nodeCount == 0) {
            return _root() == nullptr && _leftMost() == _header &&
                _rightMost() == _header;
        }
        SizeType blackCount = _blackCount(_leftMost(), _root());
        for(ConstIterator it = begin(); it != end(); ++it) {
            _LinkType leftSon = _left(it._node);
            _LinkType rightSon = _right(it._node);
            if(it._node->color == red &&
               ((leftSon && _color(leftSon) == red) ||
                (rightSon && _color(rightSon) == red))) {
                return false;
            }

            if(leftSon && _key_comparer(_key(it._node), _key(leftSon))) {
                return false;
            }

            if(rightSon && _key_comparer(_key(rightSon), _key(it._node))) {
                return false;
            }

            if(!leftSon && !rightSon && _blackCount(it._node, _root()) !=blackCount) {
                return false;
            }
        }
        if(_leftMost() != __RBTreeNodeBase::getMinNode(_root())) {
            return false;
        }
        if(_rightMost() != __RBTreeNodeBase::getMaxNode(_root())) {
            return false;
        }
        return true;
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline void swap(RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                     RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        lhs.swap(rhs);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator==(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                           const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return lhs.size() == rhs.size() &&
            equal(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator!=(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                           const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return !(lhs == rhs);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator<(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                          const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return less(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator>(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                          const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return greater(lhs.begin(), lhs.end(), rhs.begin(), rhs.end());
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator<=(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                          const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return !(lhs > rhs);
    }

    template<typename Key, typename Value, typename KeyOfValue,
             typename Compare, typename _Alloc>
    inline bool operator>=(const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &lhs,
                           const RBTree<Key, Value, KeyOfValue, Compare, _Alloc> &rhs) {
        return !(lhs < rhs);
    }

}

#endif

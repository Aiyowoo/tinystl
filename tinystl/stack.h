#ifndef STACK_H
#define STACK_H

#include "iteratorbase.h"
#include "algobase.h"
#include "deque.h"

namespace tinystl {
    template<typename T, typename Container=Deque<T>>
    class Stack {
    public:
        using ContainerType = Container;
        using ValueType = T;
        using SizeType = typename Container::SizeType;
        using Reference = typename Container::Reference;
        using ConstReference = typename Container::ConstReference;
    protected:
        using _Self = Stack<T, Container>;

    public:
        explicit Stack(const Container &cont=Container()): __container(cont) {};
        Stack(const _Self &other) = default;
        Reference top() { return __container.back(); };
        ConstReference top() const { return __container.back(); };
        bool empty() const { return __container.empty(); };
        SizeType size() const { return __container.size(); };
        void push(const T &value) { __container.pushBack(value); };
        void pop() { __container.popBack(); };
        void swap(_Self &other) { using tinystl::swap; swap(__container, other.__container); };

        template<typename T1, typename Container1>
        friend bool operator==(const Stack<T1, Container1> &lhs,
                               const Stack<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator!=(const Stack<T1, Container1> &lhs,
                               const Stack<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator<(const Stack<T1, Container1> &lhs,
                              const Stack<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator>(const Stack<T1, Container1> &lhs,
                              const Stack<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator<=(const Stack<T1, Container1> &lhs,
                               const Stack<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator>=(const Stack<T1, Container1> &lhs,
                               const Stack<T1, Container1> &rhs);

    private:
        Container __container;
    };

    template<typename T, typename Container>
    inline bool operator==(const Stack<T, Container> &lhs,
                           const Stack<T, Container> &rhs) {
        return lhs.__container == rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator!=(const Stack<T, Container> &lhs,
                           const Stack<T, Container> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename Container>
    inline bool operator<(const Stack<T, Container> &lhs,
                          const Stack<T, Container> &rhs) {
        return lhs.__container < rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator>(const Stack<T, Container> &lhs,
                          const Stack<T, Container> &rhs) {
        return lhs.__container > rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator<=(const Stack<T, Container> &lhs,
                           const Stack<T, Container> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename Container>
    inline bool operator>=(const Stack<T, Container> &lhs,
                           const Stack<T, Container> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T, typename Container>
    void swap(Stack<T, Container> &lhs, Stack<T, Container> &rhs) {
        lhs.swap(rhs);
    }
}

#endif

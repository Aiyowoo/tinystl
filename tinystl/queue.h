#ifndef QUEUE_H
#define QUEUE_H

#include "deque.h"

namespace tinystl {

    template<typename T, typename Container=Deque<T>>
    class Queue {
    public:
        using ContainerType = Container;
        using ValueType = typename Container::ValueType;
        using SizeType = typename Container::SizeType;
        using Reference = typename Container::Reference;
        using ConstReference = typename Container::ConstReference;

    protected:
        using _Self = Queue<T, Container>;

    public:
        explicit Queue(const Container &cont=Container()): __container(cont) {};
        Queue(const _Self &) = default;
        Reference front() { return __container.front(); };
        ConstReference front() const { return __container.front(); };
        Reference back() { return __container.back(); };
        ConstReference back() const { return __container.back(); };
        bool empty() const { return __container.empty(); };
        SizeType size() const { return __container.size(); };
        void push(const T &value) { __container.pushBack(value); };
        void pop() { __container.popFront(); };
        void swap(_Self &other) { using tinystl::swap; swap(__container, other.__container); };

        template<typename T1, typename Container1>
        friend bool operator==(const Queue<T1, Container1> &lhs,
                               const Queue<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator!=(const Queue<T1, Container1> &lhs,
                               const Queue<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator<(const Queue<T1, Container1> &lhs,
                              const Queue<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator>(const Queue<T1, Container1> &lhs,
                              const Queue<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator<=(const Queue<T1, Container1> &lhs,
                               const Queue<T1, Container1> &rhs);
        template<typename T1, typename Container1>
        friend bool operator>=(const Queue<T1, Container1> &lhs,
                               const Queue<T1, Container1> &rhs);

    private:
        Container __container;
    };

    template<typename T, typename Container>
    inline bool operator==(const Queue<T, Container> &lhs,
                           const Queue<T, Container> &rhs) {
        return lhs.__container == rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator!=(const Queue<T, Container> &lhs,
                           const Queue<T, Container> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename Container>
    inline bool operator<(const Queue<T, Container> &lhs,
                          const Queue<T, Container> &rhs) {
        return lhs.__container < rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator>(const Queue<T, Container> &lhs,
                           const Queue<T, Container> &rhs) {
        return lhs.__container > rhs.__container;
    }

    template<typename T, typename Container>
    inline bool operator<=(const Queue<T, Container> &lhs,
                           const Queue<T, Container> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename Container>
    inline bool operator>=(const Queue<T, Container> &lhs,
                           const Queue<T, Container> &rhs) {
        return !(lhs < rhs);
    }

    template<typename T, typename Container>
    void swap(Queue<T, Container> &lhs, Queue<T, Container> &rhs) {
        lhs.swap(rhs);
    }

}

#endif

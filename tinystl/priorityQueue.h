#ifndef PRIORITY_QUEUE_H
#define PRIORITY_QUEUE_H

#include "vector.h"
#include "algobase.h"
#include "algorithm.h"

namespace tinystl {

    template<typename T, typename Container=Vector<T>, typename Compare=Less<T>>
    class PriorityQueue {
    public:
        using ContainerType = Container;
        using ValueType = typename Container::ValueType;
        using SizeType = typename Container::SizeType;
        using Reference = typename Container::Reference;
        using ConstReference = typename Container::ConstReference;

        explicit PriorityQueue(const Compare &compare=Compare(),
                               const Container &container=Container())
            : __container(container), __comp(compare) {}

        Reference top() { return __container.front(); }
        ConstReference top() const { return __container.front(); }
        bool empty() const { return __container.empty(); }
        SizeType size() const { return __container.size(); }
        void push(const T &value);
        void pop();
        void swap(PriorityQueue<T, Container, Compare> &other) {
            swap(__container, other.__container);
        }

        template<typename T1, typename Container1, typename Compare1>
        friend bool operator==(const PriorityQueue<T1, Container1, Compare1> &lhs,
                               const PriorityQueue<T1, Container1, Compare1> &rhs);
        template<typename T1, typename Container1, typename Compare1>
        friend bool operator!=(const PriorityQueue<T1, Container1, Compare1> &lhs,
                               const PriorityQueue<T1, Container1, Compare1> &rhs);
        template<typename T1, typename Container1, typename Compare1>
        friend bool operator<(const PriorityQueue<T1, Container1, Compare1> &lhs,
                              const PriorityQueue<T1, Container1, Compare1> &rhs);
        template<typename T1, typename Container1, typename Compare1>
        friend bool operator>(const PriorityQueue<T1, Container1, Compare1> &lhs,
                              const PriorityQueue<T1, Container1, Compare1> &rhs);
        template<typename T1, typename Container1, typename Compare1>
        friend bool operator<=(const PriorityQueue<T1, Container1, Compare1> &lhs,
                               const PriorityQueue<T1, Container1, Compare1> &rhs);
        template<typename T1, typename Container1, typename Compare1>
        friend bool operator>=(const PriorityQueue<T1, Container1, Compare1> &lhs,
                               const PriorityQueue<T1, Container1, Compare1> &rhs);

    private:
        Container __container;
        Compare __comp;
    };

    template<typename T, typename Container, typename Compare>
    inline void PriorityQueue<T, Container, Compare>::push(const T &value) {
        __container.pushBack(value);
        pushHeap(__container.begin(), __container.end(), __comp);
    }

    template<typename T, typename Container, typename Compare>
    inline void PriorityQueue<T, Container, Compare>::pop() {
        popHeap(__container.begin(), __container.end(), __comp);
        __container.popBack();
    }

    template<typename T, typename Container, typename Compare>
    void swap(PriorityQueue<T, Container, Compare> &lhs,
              PriorityQueue<T, Container, Compare> &rhs) {
        lhs.swap(rhs);
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator==(const PriorityQueue<T, Container, Compare> &lhs,
                           const PriorityQueue<T, Container, Compare> &rhs) {
        return lhs.__container == rhs.__container;
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator!=(const PriorityQueue<T, Container, Compare> &lhs,
                           const PriorityQueue<T, Container, Compare> &rhs) {
        return !(lhs == rhs);
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator<(const PriorityQueue<T, Container, Compare> &lhs,
                          const PriorityQueue<T, Container, Compare> &rhs) {
        return lhs.__container < rhs.__container;
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator>(const PriorityQueue<T, Container, Compare> &lhs,
                          const PriorityQueue<T, Container, Compare> &rhs) {
        return lhs.__container > rhs.__container;
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator<=(const PriorityQueue<T, Container, Compare> &lhs,
                          const PriorityQueue<T, Container, Compare> &rhs) {
        return !(lhs > rhs);
    }

    template<typename T, typename Container, typename Compare>
    inline bool operator>=(const PriorityQueue<T, Container, Compare> &lhs,
                           const PriorityQueue<T, Container, Compare> &rhs) {
        return !(lhs < rhs);
    }

}

#endif

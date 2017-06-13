#include <new>
#include <cstddef>
#include <cstdlib>
#include <cstring>
#include <cassert>

#ifndef ALLOC_H
#define ALLOC_H

namespace tinystl {
    template<int inst>
    class MallocAlloc {
    public:
        static void* allocate(std::size_t nBytes) {
            void* result = std::malloc(nBytes);
            if(!result) {
                return outOfMemoryMalloc(nBytes);
            }
            return result;
        }

        static void deallocate(void *ptr, std::size_t) {
            std::free(ptr);
        }

        static void* reallocate(void *ptr, std::size_t oldSize, std::size_t newSize) {
            void *result = std::realloc(ptr, newSize);
            if(!result) {
                return outOfMemoryRealloc(ptr, oldSize, newSize);
            }
            return result;
        }

        static void (* setHandler(void (*fun)()))() {
            auto old = errorHandle;
            errorHandle = fun;
            return old;
        }
    private:
        static void (*errorHandle)();
        static void* outOfMemoryMalloc(std::size_t nBytes);
        static void* outOfMemoryRealloc(void *ptr, std::size_t oldSize, std::size_t newSize);
    };

    template<int inst>
    void (* MallocAlloc<inst>::errorHandle)() = nullptr;

    template<int inst>
    void* MallocAlloc<inst>::outOfMemoryMalloc(std::size_t nBytes) {
        while(true) {
            auto handle = errorHandle;
            if(!handle) {
                throw std::bad_alloc();
            }
            // handle应该由用提供,用来在malloc不能分配空间的情况下,释放空间,以完成此分配
            handle();
            void *result = std::malloc(nBytes);
            if(result) {
                return result;
            }
        }
    }

    template<int inst>
    void* MallocAlloc<inst>::outOfMemoryRealloc(void *ptr, std::size_t oldSize, std::size_t newSize) {
        while(true) {
            auto handle = errorHandle;
            if(!handle) {
                throw std::bad_alloc();
            }
            handle();
            void *result = std::realloc(ptr, newSize);
            if(result) {
                return result;
            }
        }
    }

    using MallocAllocator = MallocAlloc<0>;

    template<int inst>
    class DefaultAlloc {
    public:
        static void* allocate(std::size_t n) {
            if(n == 0) {
                return nullptr;
            }
            if(n > MAX_SIZE) {
                return MallocAllocator::allocate(n);
            }
            std::size_t index = freeListIndex(n);
            if(freeLists[index]) {
                void* result = freeLists[index];
                freeLists[index] = freeLists[index]->next;
                return result;
            }
            return refill(roundUp(n));
        }

        static void deallocate(void *ptr, std::size_t n) {
            if(n > MAX_SIZE) {
                return MallocAllocator::deallocate(ptr, n);
            }
            std::size_t index = freeListIndex(n);
            Obj *obj = static_cast<Obj *>(ptr);
            obj->next = freeLists[index];
            freeLists[index] = obj;
        }

        static void* reallocate(void *ptr, std::size_t oldSize, std::size_t newSize);

    private:
        union Obj {
            Obj *next;
            char data[0];
        };

        static std::size_t roundUp(std::size_t n) {
            return (n + ALIGN - 1) & (~(ALIGN - 1));
        }

        static std::size_t freeListIndex(std::size_t n) {
            return (n + ALIGN - 1) / ALIGN - 1;
        }

        // 返回分配的空间,并将剩余的添加到n相应的freelist中
        static void* refill(std::size_t n);
        // 返回分配的n * nObj大小的空间指针,如果空间不够分配nObj个,则可能减少nObj
        static char* allocChunk(std::size_t n, std::size_t &nObj);

        const static std::size_t ALIGN = 8;
        const static std::size_t MAX_SIZE = 128;
        const static std::size_t FREE_LIST_COUNT = MAX_SIZE / ALIGN;

        static Obj* freeLists[FREE_LIST_COUNT];
        static char* freeStartPtr;
        static char* freeEndPtr;
        static std::size_t totalSize;
    };

    template<int inst>
    typename DefaultAlloc<inst>::Obj* DefaultAlloc<inst>::freeLists[DefaultAlloc<inst>::FREE_LIST_COUNT] = {0};

    template<int inst>
    char* DefaultAlloc<inst>::freeStartPtr = nullptr;

    template<int inst>
    char* DefaultAlloc<inst>::freeEndPtr = nullptr;

    template<int inst>
    std::size_t DefaultAlloc<inst>::totalSize = 0;

    template<int inst>
    void* DefaultAlloc<inst>::reallocate( void *ptr, std::size_t oldSize, std::size_t newSize) {
        if(oldSize > MAX_SIZE && newSize > MAX_SIZE) {
            return MallocAllocator::reallocate(ptr, oldSize, newSize);
        }
        if(roundUp(oldSize) == roundUp(newSize)) {
            return ptr;
        }
        void *result = allocate(newSize);
        std::size_t minN = oldSize < newSize? oldSize: newSize;
        std::memcpy(result, ptr, minN);
        deallocate(ptr, oldSize);
        return result;
    }

    template<int inst>
    char* DefaultAlloc<inst>::allocChunk(std::size_t n, std::size_t &nObj) {
        std::size_t totalWantedSize = n * nObj;
        std::size_t leftSize = freeEndPtr - freeStartPtr;
        if(leftSize >= totalWantedSize) {
            char *result = freeStartPtr;
            freeStartPtr += totalWantedSize;
            return result;
        }

        if(leftSize >= n) {
            nObj = leftSize / n;
            char *result = freeStartPtr;
            freeStartPtr += n * nObj;
            return result;
        }

        // 把还残存但不够分配的空间添加到合适的freeList中
        if(leftSize) {
            std::size_t index = freeListIndex(leftSize);
            Obj *obj = reinterpret_cast<Obj*>(freeStartPtr);
            obj->next = freeLists[index];
            freeLists[index] = obj;
            freeStartPtr += leftSize;
        }
        std::size_t byteSizeToGet = totalWantedSize * 2 + roundUp(totalSize >> 4);
        void *newBuf = std::malloc(byteSizeToGet);
        if(newBuf) {
            char *result = static_cast<char*>(newBuf);
            freeStartPtr = result + totalWantedSize;
            freeEndPtr = result + byteSizeToGet;
            totalSize += byteSizeToGet;
            return result;
        }
        // 如果重新malloc申请失败,则从比n大的freeList中尝试窃取
        for(std::size_t i = freeListIndex(n); i < FREE_LIST_COUNT; ++i) {
            if(!freeLists[i]) {
                continue;
            }
            freeStartPtr = reinterpret_cast<char *>(freeLists[i]);
            freeEndPtr = freeStartPtr + (i + 1) * 8;
            freeLists[i] = freeLists[i]->next;
            return allocChunk(n, nObj);
        }
        // 如果比它大的也没有空间,则让MallocAllocator去处理
        freeStartPtr = static_cast<char*>(MallocAllocator::allocate(byteSizeToGet));
        freeEndPtr = freeStartPtr + byteSizeToGet;
        totalWantedSize += byteSizeToGet;
        return allocChunk(n, nObj);
    }

    template<int inst>
    void* DefaultAlloc<inst>::refill(std::size_t n) {
        std::size_t nObj = 20;
        char *result = allocChunk(n, nObj);
        if(1 == nObj) {
            return result;
        }

        for(int i = 1; i < nObj - 1; ++i) {
            Obj *obj = reinterpret_cast<Obj *>(result + n * i);
            Obj *nextObj = reinterpret_cast<Obj *>(result + n * i + n);
            obj->next = nextObj;
        }
        Obj *obj = reinterpret_cast<Obj *>(result + n * (nObj - 1));
        obj->next = nullptr;

        std::size_t index = freeListIndex(n);
        freeLists[index] = reinterpret_cast<Obj *>(result + n);
        return result;
    }

    using DefaultAllocator = DefaultAlloc<0>;

    template<typename T, typename Alloc=DefaultAllocator>
    class SimpleAlloc {
    public:
        static T* allocate() {
            return static_cast<T *>(Alloc::allocate(sizeof(T)));
        }

        static T* allocate(std::size_t n) {
            return static_cast<T *>(Alloc::allocate(sizeof(T) * n));
        }

        static void deallocate(T *ptr) {
            Alloc::deallocate(ptr, sizeof(T));
        }

        static void deallocate(T *ptr, std::size_t n) {
            Alloc::deallocate(ptr, sizeof(T) * n);
        }
    };

    template<typename Alloc>
    class DebugAlloc {
    public:
        static void *allocate(std::size_t n) {
            char *ptr = static_cast<char *>(Alloc::allocate(n + extraSize));
            *reinterpret_cast<std::size_t *>(ptr) = n;
            return ptr + extraSize;
        }

        static void deallocate(void *ptr, std::size_t n) {
            void *realPtr = static_cast<char *>(ptr) - extraSize;
            assert(*static_cast<std::size_t *>(realPtr) == n);
            Alloc::deallocate(realPtr, n + extraSize);
        }

        static void* reallocate(void *ptr, std::size_t oldSize, std::size_t newSize) {
            void *realPtr = static_cast<char *>(ptr) - extraSize;
            assert(*reinterpret_cast<std::size_t *>(realPtr) == oldSize);
            char *result = static_cast<char *>(Alloc::reallocate(realPtr, oldSize + extraSize, newSize + extraSize));
            *reinterpret_cast<std::size_t *>(result) = newSize;
            return result + extraSize;
        }

    private:
        const static std::size_t extraSize = 8;
    };

    using Alloc = DefaultAllocator;
}

#endif

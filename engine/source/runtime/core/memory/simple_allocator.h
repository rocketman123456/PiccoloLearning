#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdlib>

namespace Piccolo
{
    template<typename T>
    class SimpleAllocator
    {
    public:
        using value_type = T;

        SimpleAllocator() = default;

        template<typename U>
        SimpleAllocator(const SimpleAllocator<U>& other)
        {
            (void)other;
        }

        T* allocate(size_t n)
        {
            auto ptr = static_cast<T*>(malloc(sizeof(T) * n));
            if (ptr)
                return ptr;

            throw std::bad_alloc();
        }

        void deallocate(T* ptr, size_t n)
        {
            (void)n;
            free(ptr);
        }
    };

    template<typename T, typename U>
    bool operator==(const SimpleAllocator<T>& a1, const SimpleAllocator<U>& a2)
    {
        (void)a1;
        (void)a2;
        return true;
    }

    template<typename T, typename U>
    bool operator!=(const SimpleAllocator<T>& a1, const SimpleAllocator<U>& a2)
    {
        (void)a1;
        (void)a2;
        return false;
    }
} // namespace Piccolo
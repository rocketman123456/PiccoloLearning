#pragma once
#include "runtime/core/memory/allocator.h"

#include <cstdio>
#include <cstdlib>

namespace Piccolo
{
    template<typename T, size_t Alignment = 64>
    class CacheAlignedAllocator
    {
    public:
        using value_type      = T;
        using is_always_equal = std::true_type;

        template<typename U>
        struct rebind
        {
            using other = CacheAlignedAllocator<U, Alignment>;
        };

        CacheAlignedAllocator() = default;

        template<typename U>
        CacheAlignedAllocator(const CacheAlignedAllocator<U, Alignment>& other)
        {
            (void)other;
        }

        T* allocate(size_t n)
        {
#ifdef _WIN32
            auto ptr = static_cast<T*>(_aligned_malloc(Alignment, sizeof(T) * n));
#else
            auto ptr = static_cast<T*>(std::aligned_alloc(Alignment, sizeof(T) * n));
#endif
            if (ptr)
                return ptr;

            throw std::bad_alloc();
        }

        void deallocate(T* ptr, size_t n)
        {
            (void)n;
#ifdef _WIN32
            _aligned_free(ptr);
#else
            std::free(ptr);
#endif
        }
    };
} // namespace Piccolo
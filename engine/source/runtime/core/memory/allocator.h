#pragma once
#include <cstddef>

namespace Piccolo
{
    class Allocator
    {
    protected:
        std::size_t m_size;
        void*       m_start_address;

    protected:
        Allocator(const std::size_t size) : m_size(size) {}

    public:
        virtual ~Allocator()                                                        = default;
        virtual void* allocate(const std::size_t size, const std::size_t alignment) = 0;
        virtual void  deallocate(void* ptr, std::size_t n)                               = 0;
        virtual void  reset()                                                       = 0;
    };
} // namespace Piccolo
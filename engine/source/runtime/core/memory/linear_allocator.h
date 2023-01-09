#pragma once
#include "runtime/core/memory/allocator.h"

namespace Piccolo
{
    class LinearAllocator : public Allocator
    {
    private:
        std::size_t m_offset;

    public:
        LinearAllocator(const std::size_t size);
        ~LinearAllocator() override;
        void* allocate(const std::size_t size, const std::size_t alignment) override;
        void  deallocate(void* ptr, std::size_t n) override;
        void  reset() override;
    };
} // namespace Piccolo

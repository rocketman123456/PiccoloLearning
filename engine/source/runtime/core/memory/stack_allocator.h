#pragma once
#include "runtime/core/memory/allocator.h"

namespace Piccolo
{
    class StackAllocator : public Allocator
    {
    private:
        using Header = unsigned char;
        std::size_t m_offset;

    public:
        StackAllocator(const std::size_t size);
        ~StackAllocator() override;
        void* allocate(const std::size_t size, const std::size_t alignment) override;
        void  deallocate(void* ptr, std::size_t n) override;
        void  reset() override;
    };
} // namespace Piccolo

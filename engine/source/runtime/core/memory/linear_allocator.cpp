#include "runtime/core/memory/linear_allocator.h"

#include <memory>

namespace Piccolo
{
    LinearAllocator::LinearAllocator(const std::size_t size) : Allocator(size), m_offset(0) { m_start_address = ::operator new(size); }

    LinearAllocator::~LinearAllocator()
    {
        ::operator delete(m_start_address);
        m_start_address = nullptr;
    }

    void* LinearAllocator::allocate(const std::size_t size, const std::size_t alignment)
    {
        void*       currentAddress = reinterpret_cast<char*>(m_start_address) + m_offset;
        std::size_t space          = m_size - m_offset;
        std::align(alignment, size, currentAddress, space);

        if ((std::size_t)currentAddress + size > (std::size_t)m_start_address + m_size)
            return nullptr;

        m_offset = m_size - space + size;

        return currentAddress;
    }

    void LinearAllocator::deallocate(void* ptr, std::size_t n) {}

    void LinearAllocator::reset() { m_offset = 0; }
} // namespace Piccolo
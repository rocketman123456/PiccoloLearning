#include "runtime/core/memory/stack_allocator.h"

#include <memory>

namespace Piccolo
{
    StackAllocator::StackAllocator(const std::size_t size) : Allocator(size), m_offset(0) { m_start_address = ::operator new(size); }

    StackAllocator::~StackAllocator()
    {
        ::operator delete(m_start_address);
        m_start_address = nullptr;
    }

    void* StackAllocator::allocate(const std::size_t size, const std::size_t alignment)
    {
        void*       currentAddress = reinterpret_cast<char*>(m_start_address) + m_offset;
        void*       nextAddress    = reinterpret_cast<void*>(reinterpret_cast<char*>(currentAddress) + sizeof(Header));
        std::size_t space          = m_size - m_offset - sizeof(Header);
        std::align(alignment, size, nextAddress, space);

        if ((std::size_t)nextAddress + size > (std::size_t)m_start_address + m_size)
            return nullptr;

        std::size_t padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

        Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(nextAddress) - sizeof(Header));
        *header        = (Header)padding;

        m_offset = (std::size_t)nextAddress - (std::size_t)m_start_address + size;

        return nextAddress;
    }

    void StackAllocator::deallocate(void* ptr, std::size_t n)
    {
        const std::size_t currentAddress = (std::size_t)ptr;
        Header*           header         = reinterpret_cast<Header*>(currentAddress - sizeof(Header));

        m_offset = currentAddress - (std::size_t)m_start_address - *header;
    }

    void StackAllocator::reset() { m_offset = 0; }

} // namespace Piccolo

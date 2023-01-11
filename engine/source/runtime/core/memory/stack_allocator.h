#pragma once
#include "runtime/core/memory/allocator.h"

#include <memory>

namespace Piccolo
{
    template<typename T>
    class StackAllocator : public Allocator<T>
    {
        using Header = unsigned char;
        std::size_t m_offset;

    public:
        StackAllocator(const std::size_t size);
        ~StackAllocator() override;

        StackAllocator() = default;

        template<typename U>
        StackAllocator(const StackAllocator<U>& other)
        {
            (void)other;
        }

        T*   allocate(const std::size_t size, const std::size_t alignment = 0) override;
        void deallocate(T* ptr, std::size_t n) override;
        void reset() override;
    };

    template<typename T>
    StackAllocator<T>::StackAllocator(const std::size_t size) : Allocator<T>(size), m_offset(0)
    {
        this->m_start_address = ::operator new(size);
    }

    template<typename T>
    StackAllocator<T>::~StackAllocator()
    {
        ::operator delete(this->m_start_address);
        this->m_start_address = nullptr;
    }

    template<typename T>
    T* StackAllocator<T>::allocate(const std::size_t size_, const std::size_t alignment)
    {
        std::size_t size = sizeof(T) * size_;

        void*       currentAddress = reinterpret_cast<char*>(this->m_start_address) + m_offset;
        void*       nextAddress    = reinterpret_cast<void*>(reinterpret_cast<char*>(currentAddress) + sizeof(Header));
        std::size_t space          = this->m_size - m_offset - sizeof(Header);
        std::align(alignment, size, nextAddress, space);

        if ((std::size_t)nextAddress + size > (std::size_t)this->m_start_address + this->m_size)
            return nullptr;

        std::size_t padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

        Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(nextAddress) - sizeof(Header));
        *header        = (Header)padding;

        m_offset = (std::size_t)nextAddress - (std::size_t)this->m_start_address + size;

        return reinterpret_cast<T*>(nextAddress);
    }

    template<typename T>
    void StackAllocator<T>::deallocate(T* ptr, std::size_t n)
    {
        const std::size_t currentAddress = (std::size_t)ptr;
        Header*           header         = reinterpret_cast<Header*>(currentAddress - sizeof(Header));

        m_offset = currentAddress - (std::size_t)this->m_start_address - *header;
    }

    template<typename T>
    void StackAllocator<T>::reset()
    {
        m_offset = 0;
    }
} // namespace Piccolo

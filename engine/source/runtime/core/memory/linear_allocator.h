#pragma once
#include "runtime/core/memory/allocator.h"

#include <memory>

namespace Piccolo
{
    template<typename T>
    class LinearAllocator : public Allocator<T>
    {
    private:
        std::size_t m_offset;

    public:
        LinearAllocator(const std::size_t size);
        ~LinearAllocator() override;

        LinearAllocator() = default;

        template<typename U>
        LinearAllocator(const LinearAllocator<U>& other)
        {
            (void)other;
        }

        T*   allocate(const std::size_t size, const std::size_t alignment = 0) override;
        void deallocate(T* ptr, std::size_t n) override;
        void reset() override;
    };

    template<typename T>
    LinearAllocator<T>::LinearAllocator(const std::size_t size) : Allocator<T>(size), m_offset(0)
    {
        this->m_start_address = ::operator new(size);
    }

    template<typename T>
    LinearAllocator<T>::~LinearAllocator()
    {
        ::operator delete(this->m_start_address);
        this->m_start_address = nullptr;
    }

    template<typename T>
    T* LinearAllocator<T>::allocate(const std::size_t size_, const std::size_t alignment)
    {
        std::size_t size           = sizeof(T) * size_;
        void*       currentAddress = reinterpret_cast<char*>(this->m_start_address) + m_offset;
        std::size_t space          = this->m_size - m_offset;
        std::align(alignment, size, currentAddress, space);

        if ((std::size_t)currentAddress + size > (std::size_t)this->m_start_address + this->m_size)
            return nullptr;

        m_offset = this->m_size - space + size;

        return reinterpret_cast<T*>(currentAddress);
    }

    template<typename T>
    void LinearAllocator<T>::deallocate(T* ptr, std::size_t n)
    {}

    template<typename T>
    void LinearAllocator<T>::reset()
    {
        m_offset = 0;
    }
} // namespace Piccolo

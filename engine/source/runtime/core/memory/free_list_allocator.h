#pragma once
#include "runtime/core/container/linked_list.h"
#include "runtime/core/memory/allocator.h"

#include <cassert>
#include <memory>

namespace Piccolo
{
    template<typename T>
    class FreeListAllocator : public Allocator<T>
    {
    public:
        enum class SearchMethod
        {
            FIRST,
            BEST
        };

    private:
        struct alignas(std::max_align_t) Header
        {
            std::size_t m_size;
        };
        LinkedList   m_list;
        SearchMethod m_search_method;

    public:
        FreeListAllocator(const std::size_t size, const SearchMethod searchMethod);
        ~FreeListAllocator() override;

		FreeListAllocator() = default;

        template<typename U>
        FreeListAllocator(const FreeListAllocator<U>& other)
        {
            (void)other;
        }

        T*   allocate(const std::size_t size, const std::size_t alignment = 0) override;
        void deallocate(T* ptr, std::size_t n) override;
        void reset() override;

    private:
        void init();
        void coalescence(LinkedList::Node* prev, LinkedList::Node* curr);
    };

    template<typename T>
    FreeListAllocator<T>::FreeListAllocator(const std::size_t size, const SearchMethod searchMethod) : Allocator<T>(size)
    {
        static std::string message =
            "Total size must be atleast " + std::to_string(sizeof(LinkedList::Node) + 1) + " bytes for an allocator with atleast 1 byte of free space";
        assert(size >= sizeof(LinkedList::Node) + 1 && message.c_str());
        m_search_method       = searchMethod;
        this->m_start_address = ::operator new(size);
        init();
    }

    template<typename T>
    FreeListAllocator<T>::~FreeListAllocator()
    {
        ::operator delete(this->m_start_address);
        this->m_start_address = nullptr;
    }

    template<typename T>
    T* FreeListAllocator<T>::allocate(const std::size_t size_, const std::size_t alignment)
    {
        std::size_t size = sizeof(T) * size_;
        std::size_t padding;
        void*       currentAddress = (void*)(sizeof(Header) + size);
        void*       nextAddress    = (void*)(sizeof(Header) + size);
        std::size_t space          = size + 100;
        std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
        padding = (std::size_t)nextAddress - (std::size_t)currentAddress;

        LinkedList::Node* prev;
        LinkedList::Node* best;

        switch (m_search_method)
        {
            case SearchMethod::FIRST:
                m_list.search_first(size + padding, best, prev);
                break;
            case SearchMethod::BEST:
                m_list.search_best(size + padding, best, prev);
                break;
        }

        if (best == nullptr)
        {
            return nullptr;
        }

        if (best->m_value >= size + padding + sizeof(LinkedList::Node*) + 1)
        {
            LinkedList::Node* splittedNode = reinterpret_cast<LinkedList::Node*>(reinterpret_cast<char*>(best) + sizeof(Header) + size + padding);
            splittedNode->m_value          = best->m_value - (size + padding + sizeof(Header));
            splittedNode->m_next           = best->m_next;
            best->m_next                   = splittedNode;
        }
        else
        {
            padding += best->m_value - (size + padding);
        }

        if (prev == nullptr)
        {
            m_list.m_head = best->m_next;
        }
        else
        {
            prev->m_next = best->m_next;
        }

        Header* header = reinterpret_cast<Header*>(best);
        header->m_size = size + padding;

        return reinterpret_cast<T*>(best) + sizeof(Header);
    }

    template<typename T>
    void FreeListAllocator<T>::deallocate(T* ptr, std::size_t n)
    {
        Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

        LinkedList::Node* node = reinterpret_cast<LinkedList::Node*>(header);
        node->m_value          = header->m_size;

        LinkedList::Node* prevIt = nullptr;
        LinkedList::Node* it     = m_list.m_head;
        while (it != nullptr)
        {
            if (node < it)
            {
                node->m_next = it;
                if (prevIt == nullptr)
                {
                    m_list.m_head = node;
                }
                else
                {
                    prevIt->m_next = node;
                }
                break;
            }
            prevIt = it;
            it     = it->m_next;
        }

        coalescence(prevIt, node);
    }

    template<typename T>
    void FreeListAllocator<T>::reset()
    {
        init();
    }

    template<typename T>
    void FreeListAllocator<T>::init()
    {
        LinkedList::Node* head = reinterpret_cast<LinkedList::Node*>(this->m_start_address);
        head->m_value          = this->m_size - sizeof(Header);
        head->m_next           = nullptr;
        m_list.m_head          = head;
    }

    template<typename T>
    void FreeListAllocator<T>::coalescence(LinkedList::Node* prev, LinkedList::Node* curr)
    {
        if (curr->m_next != nullptr && (std::size_t)curr + curr->m_value + sizeof(Header) == (std::size_t)curr->m_next)
        {
            curr->m_value += curr->m_next->m_value + sizeof(Header);
            curr->m_next = curr->m_next->m_next;
        }

        if (prev != nullptr && (std::size_t)prev + prev->m_value + sizeof(Header) == (std::size_t)curr)
        {
            prev->m_value += curr->m_value + sizeof(Header);
            prev->m_next = curr->m_next;
        }
    }
} // namespace Piccolo

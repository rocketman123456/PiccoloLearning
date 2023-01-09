#include "runtime/core/memory/free_list_allocator.h"

#include <cassert>
#include <memory>
#include <string>

namespace Piccolo
{
    void LinkedList::search_first(const std::size_t v, Node*& curr, Node*& prev)
    {
        Node* prevX = nullptr;
        Node* x     = m_head;
        while (x != nullptr)
        {
            if (v <= x->m_value)
                break;
            prevX = x;
            x     = x->m_next;
        }
        prev = prevX;
        curr = x;
    }

    void LinkedList::search_best(const std::size_t v, Node*& curr, Node*& prev)
    {
        Node* prevBest = nullptr;
        Node* best     = nullptr;
        Node* prevX    = nullptr;
        Node* x        = m_head;
        while (x != nullptr)
        {
            if (v == x->m_value)
            {
                prevBest = prevX;
                best     = x;
                break;
            }
            else if (x->m_value > v)
            {
                if (best == nullptr || x->m_value < best->m_value)
                {
                    prevBest = prevX;
                    best     = x;
                }
            }
            prevX = x;
            x     = x->m_next;
        }
        prev = prevBest;
        curr = best;
    }

    FreeListAllocator::FreeListAllocator(const std::size_t size, const SearchMethod searchMethod) : Allocator(size)
    {
        static std::string message =
            "Total size must be atleast " + std::to_string(sizeof(LinkedList::Node) + 1) + " bytes for an allocator with atleast 1 byte of free space";
        assert(size >= sizeof(LinkedList::Node) + 1 && message.c_str());
        m_search_method = searchMethod;
        m_start_address = ::operator new(size);
        init();
    }

    FreeListAllocator::~FreeListAllocator()
    {
        ::operator delete(m_start_address);
        m_start_address = nullptr;
    }

    void* FreeListAllocator::allocate(const std::size_t size, const std::size_t alignment)
    {
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

        return reinterpret_cast<char*>(best) + sizeof(Header);
    }

    void FreeListAllocator::deallocate(void* ptr, std::size_t n)
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

    void FreeListAllocator::reset() { init(); }

    void FreeListAllocator::init()
    {
        LinkedList::Node* head = reinterpret_cast<LinkedList::Node*>(m_start_address);
        head->m_value          = m_size - sizeof(Header);
        head->m_next           = nullptr;
        m_list.m_head          = head;
    }

    void FreeListAllocator::coalescence(LinkedList::Node* prev, LinkedList::Node* curr)
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

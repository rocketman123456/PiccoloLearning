#include "runtime/core/memory/free_tree_allocator.h"

#include <algorithm>
#include <cassert>
#include <memory>
#include <string>

namespace Piccolo
{
    FreeTreeAllocator::FreeTreeAllocator(const std::size_t size) : Allocator(size)
    {
        static std::size_t rootNodePadding = get_root_node_padding();
        static std::string message         = "Total size must be atleast " + std::to_string(sizeof(RBTree::Node) * 2 + rootNodePadding) +
                                     " bytes for an allocator with atleast " + std::to_string(sizeof(RBTree::Node) - sizeof(Header)) + " bytes of free space";
        assert(size >= sizeof(RBTree::Node) * 2 + rootNodePadding && message.c_str());
        m_start_address = ::operator new(size);
        init();
    }

    FreeTreeAllocator::~FreeTreeAllocator()
    {
        ::operator delete(m_start_address);
        m_start_address = nullptr;
    }

    void* FreeTreeAllocator::allocate(const std::size_t size, const std::size_t alignment)
    {
        std::size_t padding        = size + sizeof(Header) < sizeof(RBTree::Node) ? sizeof(RBTree::Node) - sizeof(Header) - size : 0;
        void*       currentAddress = (void*)(sizeof(Header) + size + padding);
        void*       nextAddress    = (void*)(sizeof(Header) + size + padding);
        std::size_t space          = size + padding + 100;
        std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
        padding += (std::size_t)nextAddress - (std::size_t)currentAddress;

        RBTree::Node* node = m_tree.search_best(size + padding);

        if (node == nullptr)
        {
            return nullptr;
        }

        m_tree.remove(node);

        if (node->m_value >= size + padding + sizeof(RBTree::Node))
        {
            RBTree::Node* splittedNode = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(node) + sizeof(Header) + size + padding);
            splittedNode->m_value      = node->m_value - (size + padding + sizeof(Header));
            m_tree.insert(splittedNode);
            std::size_t* nextBlockAddress = reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(splittedNode) + sizeof(Header) + splittedNode->m_value);
            if ((std::size_t)nextBlockAddress <= (std::size_t)m_start_address + m_size - sizeof(std::size_t))
            {
                *nextBlockAddress = sizeof(Header) + splittedNode->m_value;
            }
        }
        else
        {
            padding += node->m_value - (size + padding);
        }

        Header* header = reinterpret_cast<Header*>(node);
        header->m_size = size + padding;

        *reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(header) + sizeof(Header) + header->m_size) = 0;

        return reinterpret_cast<char*>(node) + sizeof(Header);
    }

    void FreeTreeAllocator::deallocate(void* ptr, std::size_t n)
    {
        Header*       header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));
        RBTree::Node* node   = reinterpret_cast<RBTree::Node*>(header);
        node->m_value        = header->m_size;
        coalescence(node);
    }

    void FreeTreeAllocator::reset() { init(); }

    void FreeTreeAllocator::init()
    {
        RBTree::Node* nil = reinterpret_cast<RBTree::Node*>(m_start_address);
        m_tree.init(nil);
        void*       currentAddress = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(m_start_address) + sizeof(RBTree::Node) + sizeof(Header));
        std::size_t space          = m_size - sizeof(Header) - sizeof(RBTree::Node);
        std::align(alignof(std::max_align_t), sizeof(std::max_align_t), currentAddress, space);
        RBTree::Node* root = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(currentAddress) - sizeof(Header));
        root->m_value      = reinterpret_cast<char*>(m_start_address) + m_size - reinterpret_cast<char*>(root) - sizeof(Header);
        m_tree.insert(root);
    }

    void FreeTreeAllocator::coalescence(RBTree::Node* curr)
    {
        RBTree::Node* next = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(curr) + sizeof(Header) + curr->m_value);
        if (((std::size_t)next < (std::size_t)m_start_address + m_size) && (std::size_t)next->get_parent_raw() & 1)
        {
            curr->m_value += next->m_value + sizeof(Header);
            m_tree.remove(next);
        }

        if (curr->m_prev_size != 0)
        {
            RBTree::Node* prev = reinterpret_cast<RBTree::Node*>(reinterpret_cast<char*>(curr) - curr->m_prev_size);
            m_tree.remove(prev);
            prev->m_value += curr->m_value + sizeof(Header);
            m_tree.insert(prev);
            std::size_t* nextBlockAddress = reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(prev) + sizeof(Header) + prev->m_value);
            if ((std::size_t)nextBlockAddress <= (std::size_t)m_start_address + m_size - sizeof(std::size_t))
            {
                *nextBlockAddress = sizeof(Header) + prev->m_value;
            }
        }
        else
        {
            m_tree.insert(curr);
            std::size_t* nextBlockAddress = reinterpret_cast<std::size_t*>(reinterpret_cast<char*>(curr) + sizeof(Header) + curr->m_value);
            if ((std::size_t)nextBlockAddress <= (std::size_t)m_start_address + m_size - sizeof(std::size_t))
            {
                *nextBlockAddress = sizeof(Header) + curr->m_value;
            }
        }
    }

    std::size_t FreeTreeAllocator::get_root_node_padding()
    {
        void*       currentAddress = reinterpret_cast<RBTree::Node*>(sizeof(RBTree::Node) + sizeof(Header));
        void*       nextAddress    = currentAddress;
        std::size_t space          = sizeof(RBTree::Node) * 3 - sizeof(Header) - sizeof(RBTree::Node);
        std::align(alignof(std::max_align_t), sizeof(std::max_align_t), nextAddress, space);
        return (std::size_t)nextAddress - (std::size_t)currentAddress;
    }
} // namespace Piccolo

#pragma once
#include "runtime/core/container/rb_tree.h"
#include "runtime/core/memory/allocator.h"

namespace Piccolo
{
    class FreeTreeAllocator : public Allocator
    {
    private:
        struct Header
        {
            std::size_t m_prev_size;
            std::size_t m_size;
        };
        RBTree m_tree;

    public:
        FreeTreeAllocator(const std::size_t size);
        ~FreeTreeAllocator();
        void* allocate(const std::size_t size, const std::size_t alignment) override;
        void  deallocate(void* ptr, std::size_t n) override;
        void  reset() override;

    private:
        void               init();
        void               coalescence(RBTree::Node* curr);
        static std::size_t get_root_node_padding();
    };
} // namespace Piccolo

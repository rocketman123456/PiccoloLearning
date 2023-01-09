#pragma once
#include "runtime/core/memory/allocator.h"

#include <array>
#include <limits>

namespace Piccolo
{
    void buddy_init(void* space, int block_num); // allocate buddy

    void* buddy_alloc(int n); // allocate page (size of page is 2^n)

    void buddy_free(void* space, int n); // free page (starting address is space, size of page is 2^n)

    void buddy_print(); // print current state of buddy

    constexpr std::size_t Log2(std::size_t x) { return x == 1 ? 0 : 1 + Log2(x / 2); }

    class BuddyAllocator : public Allocator
    {

    private:
        struct Node
        {
            Node* m_Next;
        };
        struct alignas(std::max_align_t) Header
        {
            std::size_t m_Size;
        };
        static constexpr std::size_t                                               s_Log2Header = Log2(sizeof(Header));
        std::array<Node*, std::numeric_limits<std::size_t>::digits - s_Log2Header> m_Buckets    = {};

    public:
        BuddyAllocator(const std::size_t size = 128);
        ~BuddyAllocator() override;
        void* allocate(const std::size_t size, const std::size_t alignment) override;
        void  deallocate(void* ptr, std::size_t n) override;
        void  reset() override;

    private:
        void init();
    };
} // namespace Piccolo

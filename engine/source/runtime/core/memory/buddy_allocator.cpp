#include "runtime/core/memory/buddy_allocator.h"

#include <cmath>
#include <ctgmath>
#include <iostream>
#include <memory>

using namespace std;

void* buddySpace;
int   numOfEntries;
int   startingBlockNum;

void** freeList;

namespace Piccolo
{
    void buddy_init(void* space, int block_num)
    {
        if (space == nullptr || block_num < 2)
            exit(1); // broj blokova mora biti veci od 1 (1 blok odlazi na buddy)

        startingBlockNum = block_num;
        buddySpace       = space;
        space            = ((char*)space + BLOCK_SIZE); // ostatak memorije se koristi za alokaciju
        block_num--;                                    // prvi blok ide za potrebe buddy alokatora

        int i        = 1;
        numOfEntries = log2(block_num) + 1;

        freeList = (void**)buddySpace;
        for (i = 0; i < numOfEntries; i++)
            freeList[i] = nullptr;

        int maxLength       = numOfEntries - 1;
        int maxLengthBlocks = 1 << maxLength;

        while (block_num > 0)
        {
            void* addr          = (char*)space + (block_num - maxLengthBlocks) * BLOCK_SIZE;
            freeList[maxLength] = addr;
            *(void**)addr       = nullptr;
            block_num -= maxLengthBlocks;

            if (block_num > 0)
            {
                i         = 1;
                maxLength = 0;
                while (true)
                {
                    if (i <= block_num && 2 * i > block_num)
                        break;
                    i = i * 2;
                    maxLength++;
                }
                maxLengthBlocks = 1 << maxLength;
            }
        }
    }

    void* buddy_alloc(int n)
    {
        if (n < 0 || n >= numOfEntries)
            return nullptr;

        void* returningSpace = nullptr;

        if (freeList[n] != nullptr)
        {
            returningSpace          = freeList[n];
            freeList[n]             = *(void**)returningSpace;
            *(void**)returningSpace = nullptr;
        }
        else
        {
            for (int i = n + 1; i < numOfEntries; i++)
            {
                if (freeList[i] != nullptr)
                {
                    void* ptr1  = freeList[i];
                    freeList[i] = *(void**)ptr1;
                    void* ptr2  = (char*)ptr1 + BLOCK_SIZE * (1 << (i - 1));

                    *(void**)ptr1   = ptr2;
                    *(void**)ptr2   = freeList[i - 1];
                    freeList[i - 1] = ptr1;

                    returningSpace = buddy_alloc(n);
                    break;
                }
            }
        }

        return returningSpace;
    }

    inline bool isValid(void* space, int n) // check if starting address (space1) is valid for length 2^n
    {
        int length = 1 << n;
        int num    = ((startingBlockNum - 1) % length) + 1;
        int i      = ((char*)space - (char*)buddySpace) / BLOCK_SIZE; // num of first block

        if (i % length == num % length) // if starting block number is valid for length 2^n then true
            return true;

        return false;
    }

    void buddy_free(void* space, int n)
    {
        if (n < 0 || n >= numOfEntries)
            return;

        int bNum = 1 << n;

        if (freeList[n] == nullptr)
        {
            freeList[n]    = space;
            *(void**)space = nullptr;
        }
        else
        {
            void* prev = nullptr;
            void* curr = freeList[n];
            while (curr != nullptr)
            {
                if (curr == (void*)((char*)space + BLOCK_SIZE * bNum)) // right buddy potentially found
                {
                    if (isValid(space, n + 1)) // right buddy found
                    {
                        if (prev == nullptr)
                        {
                            freeList[n] = *(void**)freeList[n];
                        }
                        else
                        {
                            *(void**)prev = *(void**)curr;
                        }

                        buddy_free(space, n + 1);

                        return;
                    }
                }
                else if (space == (void*)((char*)curr + BLOCK_SIZE * bNum)) // left buddy potentially found
                {
                    if (isValid(curr, n + 1)) // left buddy found
                    {
                        if (prev == nullptr)
                        {
                            freeList[n] = *(void**)freeList[n];
                        }
                        else
                        {
                            *(void**)prev = *(void**)curr;
                        }

                        buddy_free(curr, n + 1);

                        return;
                    }
                }

                prev = curr;
                curr = *(void**)curr;
            }

            *(void**)space = freeList[n];
            freeList[n]    = space;
        }
    }

    void buddy_print()
    {
        cout << "Buddy current state (first block,last block):" << endl;
        for (int i = 0; i < numOfEntries; i++)
        {
            int size = 1 << i;
            cout << "entry[" << i << "] (size " << size << ") -> ";
            void* curr = freeList[i];

            while (curr != nullptr)
            {
                int first = ((char*)curr - (char*)buddySpace) / BLOCK_SIZE;
                cout << "(" << first << "," << first + size - 1 << ") -> ";
                curr = *(void**)curr;
            }
            cout << "NULL" << endl;
        }
    }

    BuddyAllocator::BuddyAllocator(const std::size_t size) : Allocator(size)
    {
        m_start_address = ::operator new(size);
        init();
    }

    BuddyAllocator::~BuddyAllocator()
    {
        ::operator delete(m_start_address);
        m_start_address = nullptr;
    }

    void* BuddyAllocator::allocate(const std::size_t size, const std::size_t alignment)
    {
        int bucket = std::max(int(std::ceil(std::log2(size + sizeof(Header))) - 1 - s_Log2Header), 0);

        if (m_Buckets[bucket] != nullptr)
        {
            Node* node        = m_Buckets[bucket];
            m_Buckets[bucket] = node->m_Next;
            Header* header    = reinterpret_cast<Header*>(node);
            header->m_Size    = (std::size_t)std::pow(2, bucket + 1 + s_Log2Header) | 1;
            void* address     = (void*)(reinterpret_cast<char*>(node) + sizeof(Header));
            return address;
        }

        int i;

        for (i = bucket + 1; i < m_Buckets.size(); ++i)
        {
            if (m_Buckets[i] != nullptr)
                break;
        }

        if (i == m_Buckets.size())
            return nullptr;

        Node* temp   = m_Buckets[i];
        m_Buckets[i] = temp->m_Next;

        --i;

        for (; i >= bucket; i--)
        {
            Node* node   = reinterpret_cast<Node*>(reinterpret_cast<char*>(temp) + (std::size_t)std::pow(2, i + 1 + s_Log2Header));
            node->m_Next = m_Buckets[i];
            m_Buckets[i] = node;
        }

        Header* header = reinterpret_cast<Header*>(temp);
        header->m_Size = (std::size_t)std::pow(2, i + 2 + s_Log2Header) | 1;
        void* address  = (void*)(reinterpret_cast<char*>(temp) + sizeof(Header));

        return address;
    }

    void BuddyAllocator::deallocate(void* ptr, std::size_t n)
    {
        Header* header = reinterpret_cast<Header*>(reinterpret_cast<char*>(ptr) - sizeof(Header));

        const std::size_t size   = header->m_Size & ~(std::size_t)1;
        const std::size_t bucket = (std::size_t)std::log2(size) - 1 - s_Log2Header;

        Node* node = reinterpret_cast<Node*>(header);

        std::size_t buddyNumber;
        char*       buddyAddress;

        buddyNumber = (reinterpret_cast<char*>(header) - static_cast<char*>(m_start_address)) / size;

        if (buddyNumber % 2 == 0)
            buddyAddress = reinterpret_cast<char*>(header) + size;
        else
            buddyAddress = reinterpret_cast<char*>(header) - size;

        // Check if buddy is occupied to bale early from searching for it
        if (buddyAddress == (static_cast<char*>(m_start_address) + m_size) || *reinterpret_cast<std::size_t*>(buddyAddress) & 1)
        {
            node->m_Next      = m_Buckets[bucket];
            m_Buckets[bucket] = node;
        }
        else
        {
            Node* prevBuddy = nullptr;
            Node* buddy     = reinterpret_cast<Node*>(buddyAddress);
            Node* context   = m_Buckets[bucket];

            // Search the bucket for the buddy and update linked listed
            // This could be improved from O(N) to O(LogN) with RBTree of addresses of nodes
            while (context != buddy && context != nullptr)
            {
                prevBuddy = context;
                context   = context->m_Next;
            }

            // If buddy was not found in the bucket it was probably split so we can't merge
            if (context == nullptr)
            {
                node->m_Next      = m_Buckets[bucket];
                m_Buckets[bucket] = node;
            }
            else
            {
                if (prevBuddy == nullptr)
                {
                    m_Buckets[bucket] = buddy->m_Next;
                }
                else
                {
                    prevBuddy->m_Next = buddy->m_Next;
                }

                if (buddyNumber % 2 == 0)
                {
                    node->m_Next          = m_Buckets[bucket + 1];
                    m_Buckets[bucket + 1] = node;
                }
                else
                {
                    buddy->m_Next         = m_Buckets[bucket + 1];
                    m_Buckets[bucket + 1] = buddy;
                }
            }
        }
    }

    void BuddyAllocator::reset()
    {
        const std::size_t bucket = (std::size_t)std::ceil(std::log2(m_size)) - 1 - s_Log2Header;
        for (std::size_t i = 0; i < bucket; ++i)
        {
            m_Buckets[i] = nullptr;
        }
        init();
    }

    void BuddyAllocator::init()
    {
        Node* root               = reinterpret_cast<Node*>(m_start_address);
        root->m_Next             = nullptr;
        const std::size_t bucket = (std::size_t)std::ceil(std::log2(m_size)) - 1 - s_Log2Header;
        m_Buckets[bucket]        = root;
    }
} // namespace Piccolo

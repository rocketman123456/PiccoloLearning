#pragma once
#include "runtime/core/memory/allocator.h"

namespace Piccolo
{
    class LinkedList
	{
	public:
		struct Node
		{
			Node* m_next;
			std::size_t m_value;
		};
		Node* m_head;
	public:
		void search_first(const std::size_t v, Node*& curr, Node*& prev);
		void search_best(const std::size_t v, Node*& curr, Node*& prev);
	};

    class FreeListAllocator : public Allocator
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
		LinkedList m_list;
		SearchMethod m_search_method;

	public:
		FreeListAllocator(const std::size_t size, const SearchMethod searchMethod);
		~FreeListAllocator() override;

		void* allocate(const std::size_t size, const std::size_t alignment) override;
		void deallocate(void* ptr, std::size_t n) override;
		void reset() override;
	private:
		void init();
		void coalescence(LinkedList::Node* prev, LinkedList::Node* curr);
	};
}

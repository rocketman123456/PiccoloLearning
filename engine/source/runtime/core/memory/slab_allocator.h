#pragma once
#include "runtime/core/memory/allocator.h"

#include <stdlib.h>

namespace Piccolo
{
    typedef struct kmem_cache_s kmem_cache_t;

    void slab_mem_init(void* space, int block_num);

    kmem_cache_t* slab_mem_cache_create(const char* name, size_t size, void (*ctor)(void*), void (*dtor)(void*)); // Allocate cache

    int slab_mem_cache_shrink(kmem_cache_t* cachep); // Shrink cache

    void* slab_mem_cache_alloc(kmem_cache_t* cachep); // Allocate one object from cache

    void slab_mem_cache_free(kmem_cache_t* cachep, void* objp); // Deallocate one object from cache

    void* slab_malloc(size_t size); // Alloacate one small memory buffer

    void slab_free(const void* objp); // Deallocate one small memory buffer

    void slab_mem_cache_destroy(kmem_cache_t* cachep); // Deallocate cache

    void slab_mem_cache_info(kmem_cache_t* cachep); // Print cache info

    int slab_mem_cache_error(kmem_cache_t* cachep); // Print error message
}
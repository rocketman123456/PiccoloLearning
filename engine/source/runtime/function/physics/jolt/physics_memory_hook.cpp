#include "runtime/function/physics/jolt/physics_memory_hook.h"

#include <Jolt/Jolt.h>
#include <Jolt/Core/Memory.h>

#include <memory>
#include <cassert>

using namespace JPH;

// Global to turn checking on/off
static bool sEnableCustomMemoryHook = false;

// Local to temporarily disable checking
static thread_local int sDisableCustomMemoryHook = 1;

// Local to keep track if we're coming through the custom allocator
static thread_local bool sInCustomAllocator = false;

struct InCustomAllocator
{
	InCustomAllocator()
	{
		assert(!sInCustomAllocator);
		sInCustomAllocator = true;
	}

	~InCustomAllocator()
	{
		assert(sInCustomAllocator);
		sInCustomAllocator = false;
	}
};

// Add a tag to an allocation to track if it is aligned / unaligned
static void *TagAllocation(void *inPointer, size_t inAlignment, char inMode)
{
	uint8 *p = reinterpret_cast<uint8 *>(inPointer);
	*p = inMode;
	return p + inAlignment;
}

// Remove tag from allocation
static void *UntagAllocation(void *inPointer, size_t inAlignment, char inMode)
{
	uint8 *p = reinterpret_cast<uint8 *>(inPointer) - inAlignment;
	assert(*p == inMode);
	*p = 0;
	return p;
}

static void *AllocateHook(size_t inSize)
{
	InCustomAllocator ica;
	return TagAllocation(malloc(inSize + 16), 16, 'U');
}

static void FreeHook(void *inBlock)
{
	InCustomAllocator ica;
	free(UntagAllocation(inBlock, 16, 'U'));
}

static void *AlignedAllocateHook(size_t inSize, size_t inAlignment)
{
	assert(inAlignment <= 64);

	InCustomAllocator ica;
	return TagAllocation(_aligned_malloc(inSize + 64, inAlignment), 64, 'A');
}

static void AlignedFreeHook(void *inBlock)
{
	InCustomAllocator ica;
	_aligned_free(UntagAllocation(inBlock, 64, 'A'));
}

static int MyAllocHook(int nAllocType, void *pvData, size_t nSize, int nBlockUse, long lRequest, const unsigned char * szFileName, int nLine) noexcept
{
	assert(!sEnableCustomMemoryHook || sDisableCustomMemoryHook <= 0 || sInCustomAllocator);
	return true;
}

namespace Piccolo
{
    void RegisterCustomMemoryHook()
    {
#if !defined(JPH_DISABLE_CUSTOM_ALLOCATOR)
        JPH::Allocate = AllocateHook;
        JPH::Free = FreeHook;
        JPH::AlignedAllocate = AlignedAllocateHook;
        JPH::AlignedFree = AlignedFreeHook;
#endif
    }

    void EnableCustomMemoryHook(bool inEnable)
    {
        sEnableCustomMemoryHook = inEnable;
    }

    bool IsCustomMemoryHookEnabled()
    {
        return sEnableCustomMemoryHook;
    }

    DisableCustomMemoryHook::DisableCustomMemoryHook()
    {
        sDisableCustomMemoryHook--;
    }

    DisableCustomMemoryHook::~DisableCustomMemoryHook()
    {
        sDisableCustomMemoryHook++;
    }
}

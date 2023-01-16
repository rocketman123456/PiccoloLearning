#pragma once

namespace Piccolo
{
    /// Register hook that detects allocations that aren't made through the custom allocator
    void RegisterCustomMemoryHook();

    /// Enable the custom memory hook to detect allocations not made through the custom allocator
    void EnableCustomMemoryHook(bool inEnable);

    /// Check if the hook is currently checking allocations
    bool IsCustomMemoryHookEnabled();

    struct DisableCustomMemoryHook
    {
        DisableCustomMemoryHook();
        ~DisableCustomMemoryHook();
    };
}
#pragma once

#include <bitset>
#include <cstdint>

namespace Piccolo
{

    // Source: https://gist.github.com/Lee-R/3839813
    constexpr std::uint32_t fnv1a_32(char const* s, std::size_t count)
    {
        return ((count ? fnv1a_32(s, count - 1) : 2166136261u) ^ s[count]) * 16777619u; // NOLINT (hicpp-signed-bitwise)
    }

    constexpr std::uint32_t operator"" _hash(char const* s, std::size_t count) { return fnv1a_32(s, count); }

    // ECS defines
    using Entity                       = std::uint32_t;
    const Entity MAX_ENTITIES          = 5000;
    using ComponentType                = std::uint8_t;
    const ComponentType MAX_COMPONENTS = 32;
    using Signature                    = std::bitset<MAX_COMPONENTS>;

    // Events
    using EventId = std::uint32_t;
    using ParamId = std::uint32_t;

#define METHOD_LISTENER(EventType, Listener) EventType, std::bind(&Listener, this, std::placeholders::_1)
#define FUNCTION_LISTENER(EventType, Listener) EventType, std::bind(&Listener, std::placeholders::_1)

} // namespace Piccolo
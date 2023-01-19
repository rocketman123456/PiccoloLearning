#pragma once
#include "runtime/function/physics/custom/body.h"

#include <vector>

namespace Piccolo
{
    struct CollisionPair
    {
        int a;
        int b;

        bool operator==(const CollisionPair& rhs) const { return (((a == rhs.a) && (b == rhs.b)) || ((a == rhs.b) && (b == rhs.a))); }
        bool operator!=(const CollisionPair& rhs) const { return !(*this == rhs); }
    };

    void broad_phase(const Body* bodies, const int num, std::vector<CollisionPair>& finalPairs, const float dt_sec);
}
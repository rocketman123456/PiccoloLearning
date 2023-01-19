#pragma once
#include "runtime/function/physics/custom/contact.h"

namespace Piccolo
{
    bool intersect(Body* bodyA, Body* bodyB, const float dt, ContactInfo& contact);
}

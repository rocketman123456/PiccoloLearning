#pragma once
#include "runtime/function/physics/custom/body.h"

namespace Piccolo
{
    struct ContactInfo
    {
        Vector3 point_a_worldspace;
        Vector3 point_b_worldspace;
        Vector3 point_a_localspace;
        Vector3 point_b_localspace;

        Vector3 normal;              // In World Space coordinates
        float   separation_distance; // positive when non-penetrating, negative when penetrating
        float   time_of_impact;

        Body* body_a;
        Body* body_b;
    };

    void resolve_contact(ContactInfo& contact);
} // namespace Piccolo
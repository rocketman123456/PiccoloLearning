#include "runtime/function/physics/custom/intersections.h"
#include "runtime/function/physics/custom/shape/sphere_shape.h"

namespace Piccolo
{
    bool ray_sphere(const Vector3& rayStart, const Vector3& rayDir, const Vector3& sphereCenter, const float sphereRadius, float& t1, float& t2)
    {
        const Vector3 m = sphereCenter - rayStart;
        const float   a = rayDir.dotProduct(rayDir);
        const float   b = m.dotProduct(rayDir);
        const float   c = m.dotProduct(m) - sphereRadius * sphereRadius;

        const float delta = b * b - a * c;
        const float invA  = 1.0f / a;

        if (delta < 0)
        {
            // no real solutions exist
            return false;
        }

        const float deltaRoot = sqrtf(delta);
        t1                    = invA * (b - deltaRoot);
        t2                    = invA * (b + deltaRoot);

        return true;
    }

    bool sphere_sphere_dynamic(const ShapeSphere* shapeA,
                               const ShapeSphere* shapeB,
                               const Vector3&     posA,
                               const Vector3&     posB,
                               const Vector3&     velA,
                               const Vector3&     velB,
                               const float        dt,
                               Vector3&           ptOnA,
                               Vector3&           ptOnB,
                               float&             toi)
    {
        const Vector3 relativeVelocity = velA - velB;

        const Vector3 startPtA = posA;
        const Vector3 endPtA   = posA + relativeVelocity * dt;
        const Vector3 rayDir   = endPtA - startPtA;

        float t0 = 0;
        float t1 = 0;
        if (rayDir.squaredLength() < 0.001f * 0.001f)
        {
            // Ray is too short, just check if already intersecting
            Vector3 ab     = posB - posA;
            float   radius = shapeA->m_radius + shapeB->m_radius + 0.001f;
            if (ab.squaredLength() > radius * radius)
            {
                return false;
            }
        }
        else if (!ray_sphere(posA, rayDir, posB, shapeA->m_radius + shapeB->m_radius, t0, t1))
        {
            return false;
        }

        // Change from [0,1] range to [0,dt] range
        t0 *= dt;
        t1 *= dt;

        // If the collision is only in the past, then there's not future collision this frame
        if (t1 < 0.0f)
        {
            return false;
        }

        // Get the earliest positive time of impact
        toi = (t0 < 0.0f) ? 0.0f : t0;

        // If the earliest collision is too far in the future, then there's no collision this frame
        if (toi > dt)
        {
            return false;
        }

        // Get the points on the respective points of collision and return true
        Vector3 newPosA = posA + velA * toi;
        Vector3 newPosB = posB + velB * toi;
        Vector3 ab      = newPosB - newPosA;
        ab.normalise();

        ptOnA = newPosA + ab * shapeA->m_radius;
        ptOnB = newPosB - ab * shapeB->m_radius;
        return true;
    }

    bool intersect(Body* bodyA, Body* bodyB, const float dt, ContactInfo& contact)
    {
        contact.body_a = bodyA;
        contact.body_b = bodyB;

        if (bodyA->m_shape->getType() == RigidBodyShapeType::sphere && bodyB->m_shape->getType() == RigidBodyShapeType::sphere)
        {
            const ShapeSphere* sphereA = (const ShapeSphere*)bodyA->m_shape;
            const ShapeSphere* sphereB = (const ShapeSphere*)bodyB->m_shape;

            Vector3 posA = bodyA->m_position;
            Vector3 posB = bodyB->m_position;

            Vector3 velA = bodyA->m_linearVelocity;
            Vector3 velB = bodyB->m_linearVelocity;

            if (sphere_sphere_dynamic(
                    sphereA, sphereB, posA, posB, velA, velB, dt, contact.point_a_worldspace, contact.point_b_worldspace, contact.time_of_impact))
            {
                // Step bodies forward to get local space collision points
                bodyA->update(contact.time_of_impact);
                bodyB->update(contact.time_of_impact);

                // Convert world space contacts to local space
                contact.point_a_localspace = bodyA->worldSpaceToBodySpace(contact.point_a_worldspace);
                contact.point_b_localspace = bodyB->worldSpaceToBodySpace(contact.point_b_worldspace);

                contact.normal = bodyA->m_position - bodyB->m_position;
                contact.normal.normalise();

                // Unwind time step
                bodyA->update(-contact.time_of_impact);
                bodyB->update(-contact.time_of_impact);

                // Calculate the separation distance
                Vector3 ab                  = bodyB->m_position - bodyA->m_position;
                float   r                   = ab.length() - (sphereA->m_radius + sphereB->m_radius);
                contact.separation_distance = r;
                return true;
            }
        }
        return false;
    }
} // namespace Piccolo
#include "runtime/function/physics/custom/broadphase.h"

namespace Piccolo
{
    struct PsuedoBody
    {
        int   id;
        float value;
        bool  ismin;
    };

    int compare_sap(const void* a, const void* b)
    {
        const PsuedoBody* ea = (const PsuedoBody*)a;
        const PsuedoBody* eb = (const PsuedoBody*)b;

        if (ea->value < eb->value)
        {
            return -1;
        }
        return 1;
    }

    void sort_bodies_bounds(const Body* bodies, const int num, PsuedoBody* sortedArray, const float dt_sec)
    {
        Vector3 axis = Vector3(1, 1, 1);
        axis.normalise();

        for (int i = 0; i < num; i++)
        {
            const Body&    body   = bodies[i];
            AxisAlignedBox bounds = body.m_shape->getBounds(body.m_position, body.m_orientation);

            // Expand the bounds by the linear velocity
            bounds.merge(bounds.m_min_corner + body.m_linearVelocity * dt_sec);
            bounds.merge(bounds.m_max_corner + body.m_linearVelocity * dt_sec);

            const float epsilon = 0.01f;
            bounds.merge(bounds.m_min_corner + Vector3(-1, -1, -1) * epsilon);
            bounds.merge(bounds.m_max_corner + Vector3(1, 1, 1) * epsilon);

            sortedArray[i * 2 + 0].id    = i;
            sortedArray[i * 2 + 0].value = axis.dotProduct(bounds.m_min_corner);
            sortedArray[i * 2 + 0].ismin = true;

            sortedArray[i * 2 + 1].id    = i;
            sortedArray[i * 2 + 1].value = axis.dotProduct(bounds.m_max_corner);
            sortedArray[i * 2 + 1].ismin = false;
        }

        qsort(sortedArray, num * 2, sizeof(PsuedoBody), compare_sap);
    }

    void build_pairs(std::vector<CollisionPair>& collisionPairs, const PsuedoBody* sortedBodies, const int num)
    {
        collisionPairs.clear();

        // Now that the bodies are sorted, build the collision pairs
        for (int i = 0; i < num * 2; i++)
        {
            const PsuedoBody& a = sortedBodies[i];
            if (!a.ismin)
            {
                continue;
            }

            CollisionPair pair;
            pair.a = a.id;

            for (int j = i + 1; j < num * 2; j++)
            {
                const PsuedoBody& b = sortedBodies[j];
                // if we've hit the end of the a element, then we're done creating pairs with a
                if (b.id == a.id)
                {
                    break;
                }

                if (!b.ismin)
                {
                    continue;
                }

                pair.b = b.id;
                collisionPairs.push_back(pair);
            }
        }
    }

    void sweep_and_prune_1d(const Body* bodies, const int num, std::vector<CollisionPair>& finalPairs, const float dt_sec)
    {
        PsuedoBody* sortedBodies = (PsuedoBody*)alloca(sizeof(PsuedoBody) * num * 2);

        sort_bodies_bounds(bodies, num, sortedBodies, dt_sec);
        build_pairs(finalPairs, sortedBodies, num);
    }

    void broad_phase(const Body* bodies, const int num, std::vector<CollisionPair>& finalPairs, const float dt_sec)
    {
        finalPairs.clear();

        sweep_and_prune_1d(bodies, num, finalPairs, dt_sec);
    }
} // namespace Piccolo

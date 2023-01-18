#include "runtime/function/physics/custom/contact_info.h"

namespace Piccolo
{
    void resolve_contact(ContactInfo& contact)
    {
        Body* bodyA = contact.body_a;
        Body* bodyB = contact.body_b;

        const Vector3 ptOnA = bodyA->bodySpaceToWorldSpace(contact.point_a_localspace);
        const Vector3 ptOnB = bodyB->bodySpaceToWorldSpace(contact.point_b_localspace);

        const float elasticityA = bodyA->m_elasticity;
        const float elasticityB = bodyB->m_elasticity;
        const float elasticity  = elasticityA * elasticityB;

        const float invMassA = bodyA->m_invMass;
        const float invMassB = bodyB->m_invMass;

        const Matrix3x3 invWorldInertiaA = bodyA->getInverseInertiaTensorWorldSpace();
        const Matrix3x3 invWorldInertiaB = bodyB->getInverseInertiaTensorWorldSpace();

        const Vector3 n = contact.normal;

        const Vector3 ra = ptOnA - bodyA->getCenterOfMassWorldSpace();
        const Vector3 rb = ptOnB - bodyB->getCenterOfMassWorldSpace();

        const Vector3 angularJA     = (invWorldInertiaA * ra.crossProduct(n)).crossProduct(ra);
        const Vector3 angularJB     = (invWorldInertiaB * rb.crossProduct(n)).crossProduct(rb);
        const float   angularFactor = (angularJA + angularJB).dotProduct(n);

        // Get the world space velocity of the motion and rotation
        const Vector3 velA = bodyA->m_linearVelocity + bodyA->m_angularVelocity.crossProduct(ra);
        const Vector3 velB = bodyB->m_linearVelocity + bodyB->m_angularVelocity.crossProduct(rb);

        // Calculate the collision impulse
        const Vector3 vab            = velA - velB;
        const float   ImpulseJ       = (1.0f + elasticity) * vab.dotProduct(n) / (invMassA + invMassB + angularFactor);
        const Vector3 vectorImpulseJ = n * ImpulseJ;

        bodyA->applyImpulse(ptOnA, vectorImpulseJ * -1.0f);
        bodyB->applyImpulse(ptOnB, vectorImpulseJ * 1.0f);

        //
        // Calculate the impulse caused by friction
        //

        const float frictionA = bodyA->m_friction;
        const float frictionB = bodyB->m_friction;
        const float friction  = frictionA * frictionB;

        // Find the normal direction of the velocity with respect to the normal of the collision
        const Vector3 velNorm = n * n.dotProduct(vab);

        // Find the tangent direction of the velocity with respect to the normal of the collision
        const Vector3 velTang = vab - velNorm;

        // Get the tangential velocities relative to the other body
        Vector3 relativeVelTang = velTang;
        relativeVelTang.normalise();

        const Vector3 inertiaA   = (invWorldInertiaA * ra.crossProduct(relativeVelTang)).crossProduct(ra);
        const Vector3 inertiaB   = (invWorldInertiaB * rb.crossProduct(relativeVelTang)).crossProduct(rb);
        const float   invInertia = (inertiaA + inertiaB).dotProduct(relativeVelTang);

        // Calculate the tangential impulse for friction
        const float   reducedMass     = 1.0f / (bodyA->m_invMass + bodyB->m_invMass + invInertia);
        const Vector3 impulseFriction = velTang * reducedMass * friction;

        // Apply kinetic friction
        bodyA->applyImpulse(ptOnA, impulseFriction * -1.0f);
        bodyB->applyImpulse(ptOnB, impulseFriction * 1.0f);

        //
        // Let's also move our colliding objects to just outside of each other (projection method)
        //
        if (0.0f == contact.time_of_impact)
        {
            const Vector3 ds = ptOnB - ptOnA;

            const float tA = invMassA / (invMassA + invMassB);
            const float tB = invMassB / (invMassA + invMassB);

            bodyA->m_position += ds * tA;
            bodyB->m_position -= ds * tB;
        }
    }
} // namespace Piccolo
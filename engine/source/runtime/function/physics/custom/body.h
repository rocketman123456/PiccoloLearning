#pragma once
#include "runtime/function/physics/custom/shape/shape.h"

namespace Piccolo
{
    class Body
    {
    public:
        Body();

        Vector3    m_position;
        Quaternion m_orientation;
        Vector3    m_linearVelocity;
        Vector3    m_angularVelocity;

        float  m_invMass;
        float  m_elasticity;
        float  m_friction;
        Shape* m_shape;

        Vector3 getCenterOfMassWorldSpace() const;
        Vector3 getCenterOfMassModelSpace() const;

        Vector3 worldSpaceToBodySpace(const Vector3& pt) const;
        Vector3 bodySpaceToWorldSpace(const Vector3& pt) const;

        Matrix3x3 getInverseInertiaTensorBodySpace() const;
        Matrix3x3 getInverseInertiaTensorWorldSpace() const;

        void applyImpulse(const Vector3& impulsePoint, const Vector3& impulse);
        void applyImpulseLinear(const Vector3& impulse);
        void applyImpulseAngular(const Vector3& impulse);

        void update(const float dt_sec);
    };
} // namespace Piccolo
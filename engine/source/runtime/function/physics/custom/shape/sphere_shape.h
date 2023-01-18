#pragma once
#include "runtime/function/physics/custom/shape/shape.h"

namespace Piccolo
{
    class ShapeSphere : public Shape
    {
    public:
        explicit ShapeSphere(const float radius) : m_radius(radius) { m_center_of_mass = Vector3::ZERO; }
        virtual ~ShapeSphere() = default;

        virtual Matrix3x3 inertiaTensor() const;

        virtual AxisAlignedBox getBounds(const Vector3& pos, const Quaternion& orient) const;
        virtual AxisAlignedBox getBounds() const;

        virtual Vector3 getCenterOfMass() const { return m_center_of_mass; }

        virtual RigidBodyShapeType getType() const { return RigidBodyShapeType::sphere; }

        virtual Vector3 support(const Vector3& dir, const Vector3& pos, const Quaternion& orient, const float bias) const;

        virtual float fastestLinearSpeed(const Vector3& angularVelocity, const Vector3& dir) const { return 0.0f; }

    public:
        float m_radius;
    };
} // namespace Piccolo
#pragma once
#include "resource/res_type/components/rigid_body.h"
#include "resource/res_type/data/geometry.h"
#include "runtime/core/math/axis_aligned.h"
#include "runtime/core/math/quaternion.h"

namespace Piccolo
{
    class Shape
    {
    public:
        virtual ~Shape() = default;

        virtual Matrix3x3 inertiaTensor() const = 0;

        virtual AxisAlignedBox getBounds(const Vector3& pos, const Quaternion& orient) const = 0;
        virtual AxisAlignedBox getBounds() const                                             = 0;

        virtual Vector3 getCenterOfMass() const { return m_center_of_mass; }

        virtual RigidBodyShapeType getType() const = 0;

        virtual Vector3 support(const Vector3& dir, const Vector3& pos, const Quaternion& orient, const float bias) const = 0;

        virtual float fastestLinearSpeed(const Vector3& angularVelocity, const Vector3& dir) const { return 0.0f; }

    protected:
        Vector3 m_center_of_mass;
    };
} // namespace Piccolo

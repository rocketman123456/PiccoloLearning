#include "runtime/function/physics/custom/shape/sphere_shape.h"

namespace Piccolo
{
    Vector3 ShapeSphere::support(const Vector3& dir, const Vector3& pos, const Quaternion& orient, const float bias) const
    {
        Vector3 supportPt;

        // TODO: Add code

        return supportPt;
    }

    Matrix3x3 ShapeSphere::inertiaTensor() const
    {
        Matrix3x3 tensor = Matrix3x3::ZERO;
        tensor.m_rows[0][0] = 2.0f * m_radius * m_radius / 5.0f;
        tensor.m_rows[1][1] = 2.0f * m_radius * m_radius / 5.0f;
        tensor.m_rows[2][2] = 2.0f * m_radius * m_radius / 5.0f;
        return tensor;
    }

    AxisAlignedBox ShapeSphere::getBounds(const Vector3& pos, const Quaternion& orient) const
    {
        AxisAlignedBox tmp;
        tmp.update(pos, Vector3(m_radius, m_radius, m_radius));
        return tmp;
    }

    AxisAlignedBox ShapeSphere::getBounds() const
    {
        AxisAlignedBox tmp;
        tmp.update(Vector3::ZERO, Vector3(m_radius, m_radius, m_radius));
        return tmp;
    }
} // namespace Piccolo
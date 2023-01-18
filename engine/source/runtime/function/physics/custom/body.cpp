#include "runtime/function/physics/custom/body.h"

namespace Piccolo
{
    Body::Body() : m_position(Vector3::ZERO), m_orientation(0.0f, 0.0f, 0.0f, 1.0f), m_shape(NULL) { m_linearVelocity.zero(); }

    Vector3 Body::getCenterOfMassWorldSpace() const
    {
        const Vector3 centerOfMass = m_shape->getCenterOfMass();
        const Vector3 pos          = m_position + m_orientation.rotatePoint(centerOfMass);
        return pos;
    }

    Vector3 Body::getCenterOfMassModelSpace() const
    {
        const Vector3 centerOfMass = m_shape->getCenterOfMass();
        return centerOfMass;
    }

    Vector3 Body::worldSpaceToBodySpace(const Vector3& worldPt) const
    {
        Vector3    tmp           = worldPt - getCenterOfMassWorldSpace();
        Quaternion inverseOrient = m_orientation.inverse();
        Vector3    bodySpace     = inverseOrient.rotatePoint(tmp);
        return bodySpace;
    }

    Vector3 Body::bodySpaceToWorldSpace(const Vector3& worldPt) const
    {
        Vector3 worldSpace = getCenterOfMassWorldSpace() + m_orientation.rotatePoint(worldPt);
        return worldSpace;
    }

    Matrix3x3 Body::getInverseInertiaTensorBodySpace() const
    {
        Matrix3x3 inertiaTensor    = m_shape->inertiaTensor();
        Matrix3x3 invInertiaTensor = inertiaTensor.inverse() * m_invMass;
        return invInertiaTensor;
    }

    Matrix3x3 Body::getInverseInertiaTensorWorldSpace() const
    {
        Matrix3x3 inertiaTensor    = m_shape->inertiaTensor();
        Matrix3x3 invInertiaTensor = inertiaTensor.inverse() * m_invMass;
        Matrix3x3 orient           = m_orientation.toRotationMatrix3();
        invInertiaTensor           = orient * invInertiaTensor * orient.transpose();
        return invInertiaTensor;
    }

    void Body::applyImpulse(const Vector3& impulsePoint, const Vector3& impulse)
    {
        if (0.0f == m_invMass)
        {
            return;
        }

        // impulsePoint is the world space location of the application of the impulse
        // impulse is the world space direction and magnitude of the impulse
        applyImpulseLinear(impulse);

        Vector3 position = getCenterOfMassWorldSpace(); // applying impulses must produce torques through the center of mass
        Vector3 r        = impulsePoint - position;
        Vector3 dL       = r.crossProduct(impulse); // this is in world space
        applyImpulseAngular(dL);
    }

    void Body::applyImpulseLinear(const Vector3& impulse)
    {
        if (0.0f == m_invMass)
        {
            return;
        }

        // p = mv
        // dp = m dv = J
        // => dv = J / m
        m_linearVelocity += impulse * m_invMass;
    }

    void Body::applyImpulseAngular(const Vector3& impulse)
    {
        if (0.0f == m_invMass)
        {
            return;
        }

        // L = I w = r x p
        // dL = I dw = r x J
        // => dw = I^-1 * ( r x J )
        m_angularVelocity += getInverseInertiaTensorWorldSpace() * impulse;

        const float maxAngularSpeed = 30.0f; // 30 rad/s is fast enough for us. But feel free to adjust.
        if (m_angularVelocity.squaredLength() > maxAngularSpeed * maxAngularSpeed)
        {
            m_angularVelocity.normalise();
            m_angularVelocity *= maxAngularSpeed;
        }
    }

    void Body::update(const float dt_sec)
    {
        m_position += m_linearVelocity * dt_sec;

        // okay, we have an angular velocity around the center of mass, this needs to be
        // converted somehow to relative to model position.  This way we can properly update
        // the orientation of the model.
        Vector3 positionCM = getCenterOfMassWorldSpace();
        Vector3 cmToPos    = m_position - positionCM;

        // Total Torque is equal to external applied torques + internal torque (precession)
        // T = T_external + omega x I * omega
        // T_external = 0 because it was applied in the collision response function
        // T = Ia = w x I * w
        // a = I^-1 ( w x I * w )
        Matrix3x3 orientation   = m_orientation.toRotationMatrix3();
        Matrix3x3 inertiaTensor = orientation * m_shape->inertiaTensor() * orientation.transpose();
        Vector3   alpha         = inertiaTensor.inverse() * (m_angularVelocity.crossProduct(inertiaTensor * m_angularVelocity));
        m_angularVelocity += alpha * dt_sec;

        // Update orientation
        Vector3    dAngle = m_angularVelocity * dt_sec;
        Quaternion dq     = Quaternion(Radian(dAngle.length()), dAngle);
        m_orientation     = dq * m_orientation;
        m_orientation.normalise();

        // Now get the new model position
        m_position = positionCM + dq.rotatePoint(cmToPos);
    }
} // namespace Piccolo
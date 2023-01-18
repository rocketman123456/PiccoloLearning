#pragma once
#include "runtime/core/math/vector3.h"

#include <memory>
#include <vector>

namespace Piccolo
{
    class PhysicsScene;

    class PhysicsManager
    {
    public:
        void initialize();
        void clear();

        std::weak_ptr<PhysicsScene> createPhysicsScene(const Vector3& gravity);
        void                        deletePhysicsScene(std::weak_ptr<PhysicsScene> physics_scene);

    protected:
        std::vector<std::shared_ptr<PhysicsScene>> m_scenes;
    };
} // namespace Piccolo

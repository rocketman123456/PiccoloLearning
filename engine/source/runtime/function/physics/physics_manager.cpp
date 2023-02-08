#include "runtime/function/physics/physics_manager.h"
#include "runtime/function/physics/jolt/utils.h"
#include "runtime/function/physics/physics_scene.h"

#include "runtime/resource/config_manager/config_manager.h"

#include <Jolt/Core/Memory.h>

namespace Piccolo
{
    void PhysicsManager::initialize() { JPH::RegisterDefaultAllocator(); }

    void PhysicsManager::clear() { m_scenes.clear(); }

    std::weak_ptr<PhysicsScene> PhysicsManager::createPhysicsScene(const Vector3& gravity)
    {
        std::shared_ptr<PhysicsScene> physics_scene = std::make_shared<PhysicsScene>(gravity);

        m_scenes.push_back(physics_scene);

        return physics_scene;
    }

    void PhysicsManager::deletePhysicsScene(std::weak_ptr<PhysicsScene> physics_scene)
    {
        std::shared_ptr<PhysicsScene> deleted_scene = physics_scene.lock();

        auto iter = std::find(m_scenes.begin(), m_scenes.end(), deleted_scene);
        if (iter != m_scenes.end())
        {
            m_scenes.erase(iter);
        }
    }
} // namespace Piccolo
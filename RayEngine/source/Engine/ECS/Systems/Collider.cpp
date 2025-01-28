#include "Collider.h"

#include "Transform.h"
#include "Instance/Manager.h"
#include "Physics/CollisionShape.h"
#include "Physics/Manager.h"
#include "Rendering/Debug/Draw.h"

void ECS::SysCollider::Init(EntityID InEntity, Collider& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return; 
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollider::Deinit(EntityID InEntity, Collider& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return; 
    Physics::Manager::Get().Remove(InEntity); 
}

void ECS::SysCollider::Update(EntityID InID, Collider& InComponent)
{
    auto& t = Get<Transform>(InID);
    auto& data = InComponent.ShapeData.Get();
    switch (static_cast<Physics::Shape>(InComponent.Shape.Get())) {
    case Physics::Shape::BOX:
        Rendering::DebugBox(
            t.GetPosition(),
            t.GetRotation(),
            Vec3F(data.x, data.y, data.z) * 2.0f);
        break;
    case Physics::Shape::CAPSULE:
        Rendering::DebugCapsule(
            t.GetPosition(),
            t.GetRotation(),
            data.x,
            data.y);
        break;
    case Physics::Shape::SPHERE:
        Rendering::DebugSphere(
            t.GetPosition(),
            data.x);
        break;
    }
}

bool ECS::SysCollider::ShouldUpdate() const
{
    // Show debug visualization in editor
    return Engine::Instance::Get().IsEditor();
}

void ECS::SysCollider::UpdateShape(EntityID InEntity) const
{
    auto& man = Physics::Manager::Get();
    man.Remove(InEntity);
    man.Add(InEntity); 
}

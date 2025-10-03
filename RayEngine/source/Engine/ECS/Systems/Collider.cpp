#include "Collider.h"

#include "Transform.h"
#include "Editor/Debug/Draw.h"
#include "Instance/Manager.h"
#include "Physics/CollisionShape.h"
#include "Physics/Manager.h"

void ECS::SysCollider::Init(const EntityID InEntity, Collider& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return; 
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollider::Deinit(const EntityID InEntity, Collider& InComponent)
{
    if (Engine::Instance::Get().IsEditor())
        return; 
    Physics::Manager::Get().Remove(InEntity); 
}

void ECS::SysCollider::Update(const EntityID InID, Collider& InComponent)
{
    const auto& t = Get<Transform>(InID);
    const auto& data = InComponent.ShapeData.Get();
    switch (static_cast<Physics::Shape>(InComponent.Shape.Get())) {
        case Physics::Shape::BOX:
        Debug::Box(
            t.GetPosition(),
            t.GetRotation(),
            Vec3F(data.x, data.y, data.z) * 2.0f);
        break;
    case Physics::Shape::CAPSULE:
        Debug::Capsule(
            t.GetPosition(),
            t.GetRotation(),
            data.x,
            data.y);
        break;
    case Physics::Shape::SPHERE:
        Debug::Sphere(
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

void ECS::SysCollider::UpdateShape(const EntityID InEntity)
{
    auto& man = Physics::Manager::Get();
    man.Remove(InEntity);
    man.Add(InEntity); 
}

#include "Collider.h"

#include "Transform.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Debug/DebugDraw.h"

void ECS::SysCollider::Init(EntityID InEntity, Collider& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return; 
    Physics::Manager::Get().Add(InEntity); 
}

void ECS::SysCollider::Deinit(EntityID InEntity, Collider& InComponent)
{
    if (Engine::InstanceBase::Get().IsEditor())
        return; 
    Physics::Manager::Get().Remove(InEntity); 
}

void ECS::SysCollider::Update(EntityID InID, Collider& InComponent, double InDelta)
{
    auto& t = Get<Transform>(InID);
    switch (static_cast<CollisionShape>(InComponent.Shape.Get())) {
    case CollisionShape::BOX:
        Rendering::DebugBox(
            t.GetPosition(),
            InComponent.ShapeData.Get().xyz);
        break;
    case CollisionShape::CAPSULE:
        Rendering::DebugCapsule(
            t.GetPosition(),
            t.GetRotation(),
            InComponent.ShapeData.Get().x,
            InComponent.ShapeData.Get().y);
        break;
    case CollisionShape::SPHERE:
        Rendering::DebugSphere(
            t.GetPosition(),
            InComponent.ShapeData.Get().x);
        break;
    }
}

bool ECS::SysCollider::ShouldUpdate() const
{
    // Show debug visualization in editor
    return Engine::InstanceBase::Get().IsEditor();
}

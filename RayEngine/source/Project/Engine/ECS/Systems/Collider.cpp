#include "Collider.h"

#include "Transform.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Physics/CollisionShape.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Debug/Draw.h"

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

void ECS::SysCollider::Update(EntityID InID, Collider& InComponent, double InDelta)
{
    auto& t = Get<Transform>(InID);
    switch (static_cast<Physics::Shape>(InComponent.Shape.Get())) {
    case Physics::Shape::BOX:
        Rendering::DebugBox(
            t.GetPosition(),
            InComponent.ShapeData.Get().xyz * 2.0f);
        break;
    case Physics::Shape::CAPSULE:
        Rendering::DebugCapsule(
            t.GetPosition(),
            t.GetRotation(),
            InComponent.ShapeData.Get().x,
            InComponent.ShapeData.Get().y);
        break;
    case Physics::Shape::SPHERE:
        Rendering::DebugSphere(
            t.GetPosition(),
            InComponent.ShapeData.Get().x);
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

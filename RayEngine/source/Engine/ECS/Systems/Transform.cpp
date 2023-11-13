#include "Transform.h"

using namespace ECS;

void ECS::Transform::SetPosition(const Vec3F& InPos)
{
    Position = InPos;
    // Set position
    // Transform stuff
    // Update children
}

void ECS::Transform::SetRotation(const QuatF& InRot)
{
    Rotation = InRot;
    // Set rotation
    // Transform stuff
    // Update children
}

void ECS::Transform::SetScale(const Vec3F& InScale)
{
    Scale = InScale; 
    // Set scale
    // Transform stuff
    // Update children
}

void SysTransform::SetupHierarchy(const EntityID InParent, const EntityID InChild)
{
    const auto parent = TryGet<Transform>(InParent);
    const auto child = TryGet<Transform>(InChild);
    CHECK_RETURN(!parent || !child);
    parent->Children.insert(InChild);
    child->Parent = InParent;
}

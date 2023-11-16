#include "Player.h"

#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Transform.h"

void ECS::SysPlayer::Init(EntityID InID, Player& InComponent)
{
    // Find camera and collider
    const auto& trans = Get<Transform>(InID);
    for (const EntityID child : trans.GetChildren())
    {
        if (TryGet<Collider>(child))
            InComponent.Collider = child;
        if (TryGet<Camera>(child))
            InComponent.Camera = child; 
    }
}

void ECS::SysPlayer::OnBeginContact(const Physics::Contact& InContact)
{
}

void ECS::SysPlayer::OnEndContact(const Physics::Contact& InContact)
{
}

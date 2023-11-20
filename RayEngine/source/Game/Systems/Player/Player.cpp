#include "Player.h"

#include "Input.h"
#include "Movement.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Transform.h"

void ECS::Player::Init()
{
    // Find camera and collider
    const auto& trans = Get<Transform>(GetID());
    for (const EntityID child : trans.GetChildren())
    {
        if (TryGet<Collider>(child))
            ColliderID = child;
        if (TryGet<Camera>(child))
            CameraID = child; 
    }
    
    if (const auto movement = TryGet<Movement>(GetID()))
        movement->PlayerID = GetID();
    if (const auto input = TryGet<Input>(GetID()))
        input->PlayerID = GetID();

}

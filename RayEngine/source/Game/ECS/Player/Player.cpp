#include "Player.h"

#include "Input.h"
#include "PlayerCamera.h"
#include "Movement/Movement.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Menu/Manager.h"
#include "Game/Menus/MenuLevelEnd.h"
#include "Game/ECS/LevelEnd.h"
#include "Game/Menus/MenuDeath.h"

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
        movement->SetPlayer(GetID());
    if (const auto input = TryGet<Input>(GetID()))
        input->SetPlayer(GetID());
    if (const auto playerCamera = TryGet<PlayerCamera>(GetID()))
        playerCamera->SetPlayer(GetID());
}

void ECS::Player::Update(double InDelta)
{
    const auto& t = Get<Transform>(GetID());
    if (t.GetPosition().y < -30.0f)
        Die();
}

void ECS::Player::OnBeginContact(const Physics::Contact& InContact)
{
    const auto parent = Get<Transform>(InContact.Target).GetParent(); 
    if (parent != InvalidID)
        if (GetSystem<SysLevelEnd>().Contains(parent))
            Menu::Manager::Get().Push<MenuLevelEnd>();
}

void ECS::Player::Die()
{
    LOG("Player died!");
    Menu::Manager::Get().Push<MenuDeath>();
}

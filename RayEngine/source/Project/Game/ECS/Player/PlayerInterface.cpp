#include "PlayerInterface.h"

#include "Player.h"
#include "PlayerInput.h"
#include "PlayerCamera.h"
#include "Movement/Movement.h"
#include "Weapon/Weapon.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Game/GameState.h"

ECS::Player& ECS::PlayerInterface::GetPlayer() const
{
    auto* ptr = Manager::Get().GetComponent<Player>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid player");
    return *ptr; 
}

ECS::EntityID ECS::PlayerInterface::GetPlayerID() const
{
    const ECS::EntityID id = GameState::Get().GetPlayerID();
    CHECK_ASSERT(id == InvalidID, "Invalid player ID");
    return id; 
}

ECS::Transform& ECS::PlayerInterface::GetPlayerTransform() const
{
    auto* ptr = Manager::Get().GetComponent<Transform>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid player trans");
    return *ptr; 
}

ECS::Transform& ECS::PlayerInterface::GetCameraTransform() const
{
    auto* ptr = Manager::Get().GetComponent<Transform>(GetPlayer().GetCameraID());
    CHECK_ASSERT(!ptr, "Invalid camera trans");
    return *ptr; 
}

ECS::Transform& ECS::PlayerInterface::GetColliderTransform() const
{
    auto* ptr = Manager::Get().GetComponent<Transform>(GetPlayer().GetColliderID());
    CHECK_ASSERT(!ptr, "Invalid collider trans");
    return *ptr; 
}

ECS::Movement& ECS::PlayerInterface::GetMovement() const
{
    auto* ptr = Manager::Get().GetComponent<Movement>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid movement");
    return *ptr;
}

ECS::PlayerInput& ECS::PlayerInterface::GetInput() const
{
    auto* ptr = Manager::Get().GetComponent<PlayerInput>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid input");
    return *ptr;
}

ECS::Collider& ECS::PlayerInterface::GetCollider() const
{
    auto* ptr = Manager::Get().GetComponent<Collider>(GetPlayer().GetColliderID());
    CHECK_ASSERT(!ptr, "Invalid collider");
    return *ptr;
}

ECS::Rigidbody& ECS::PlayerInterface::GetRB() const
{
    auto* ptr = Manager::Get().GetComponent<Rigidbody>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid rb");
    return *ptr;
}

ECS::PlayerCamera& ECS::PlayerInterface::GetPlayerCamera() const
{
    auto* ptr = Manager::Get().GetComponent<PlayerCamera>(GetPlayerID());
    CHECK_ASSERT(!ptr, "Invalid camera");
    return *ptr;
}

ECS::Camera& ECS::PlayerInterface::GetCamera() const
{
    auto* ptr = Manager::Get().GetComponent<Camera>(GetPlayer().GetCameraID());
    CHECK_ASSERT(!ptr, "Invalid camera");
    return *ptr;
}

Weapon* ECS::PlayerInterface::GetSword() const
{
    const EntityID id = GetPlayer().GetSwordID();
    CHECK_RETURN(id == InvalidID, nullptr); 
    return Manager::Get().GetComponent<Weapon>(id);
}

double ECS::PlayerInterface::GetTime()
{
    return Utility::Time::Get().Total();
}

#include "PlayerInterface.h"

#include "Player.h"
#include "Input.h"
#include "PlayerCamera.h"
#include "Movement/Movement.h"
#include "Sword/Sword.h"
#include "Engine/ECS/Manager.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Collider.h"

ECS::Player& ECS::PlayerInterface::GetPlayer() const
{
    auto* ptr = Manager::Get().GetComponent<Player>(PlayerID);
    CHECK_ASSERT(!ptr, "Invalid player");
    return *ptr; 
}

ECS::Transform& ECS::PlayerInterface::GetPlayerTransform() const
{
    auto* ptr = Manager::Get().GetComponent<Transform>(PlayerID);
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
    auto* ptr = Manager::Get().GetComponent<Movement>(PlayerID);
    CHECK_ASSERT(!ptr, "Invalid movement");
    return *ptr;
}

ECS::Input& ECS::PlayerInterface::GetInput() const
{
    auto* ptr = Manager::Get().GetComponent<Input>(PlayerID);
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
    auto* ptr = Manager::Get().GetComponent<Rigidbody>(PlayerID);
    CHECK_ASSERT(!ptr, "Invalid rb");
    return *ptr;
}

ECS::PlayerCamera& ECS::PlayerInterface::GetPlayerCamera() const
{
    auto* ptr = Manager::Get().GetComponent<PlayerCamera>(PlayerID);
    CHECK_ASSERT(!ptr, "Invalid camera");
    return *ptr;
}

ECS::Camera& ECS::PlayerInterface::GetCamera() const
{
    auto* ptr = Manager::Get().GetComponent<Camera>(GetPlayer().GetCameraID());
    CHECK_ASSERT(!ptr, "Invalid camera");
    return *ptr;
}

Sword* ECS::PlayerInterface::GetSword() const
{
    const EntityID id = GetPlayer().GetSwordID();
    CHECK_RETURN(id == InvalidID, nullptr); 
    return Manager::Get().GetComponent<Sword>(id);
}

double ECS::PlayerInterface::GetTime()
{
    return Utility::Time::Get().Total();
}

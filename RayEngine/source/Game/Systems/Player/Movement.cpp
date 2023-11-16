#include "Movement.h"

#include "Player.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Utility/Math/AngleConversion.h"

void ECS::SysMovement::ConsumeRotInput(Movement& InMovement)
{
    // Set body rotation
    auto& trans = Get<Transform>(InMovement.GetID());
    Vec3F bodyRot = trans.GetRotation().Euler();
    bodyRot.y += InMovement.RotInput.y; 
    trans.SetRotation(bodyRot);

    // Set camera rotation
    const auto& player = Get<Player>(InMovement.GetID());
    auto& camTrans = Get<Transform>(player.Camera);
    float camRot = camTrans.GetRotation().Euler().x; 
    camRot += InMovement.RotInput.x;
    camRot = CLAMP(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        camRot);
    camTrans.SetRotation(
        Vec3F(camRot, 0.0f, 0.0f),
        Transform::Space::LOCAL); 

    // Consume input
    InMovement.RotInput = Vec2F::Zero();
}

void ECS::SysMovement::ConsomeMoveInput(Movement& InMovement)
{
    const auto& rb = Get<Rigidbody>(InMovement.GetID());
    if (InMovement.MoveInput.length > 0.01f)
    {
        const Vec2F force = Vec2F(InMovement.MoveInput.normalized) * InMovement.MovementForce.Get();
        rb.AddForce(Vec3F(force.x, 0.0f, force.y));
        
        // Consume input
        InMovement.MoveInput = Vec2F::Zero(); 
    }
}

void ECS::SysMovement::ConsumeJumpInput(Movement& InMovement)
{
    const auto& rb = Get<Rigidbody>(InMovement.GetID());
    if (InMovement.JumpInput)
    {
        rb.AddImpulse(Vec3F::Up() * InMovement.JumpForce.Get());

        // Consume input
        InMovement.JumpInput = false; 
    }
}

void ECS::SysMovement::Update(EntityID InID, Movement& InComponent, double InDelta)
{
    ConsumeRotInput(InComponent);
    ConsomeMoveInput(InComponent);
    ConsumeJumpInput(InComponent);
}

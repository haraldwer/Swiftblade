#include "Movement.h"

#include "MovementStateMachine.h"
#include "Game/Systems/Player/Input.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Contact.h"
#include "Engine/Physics/Query.h"
#include "Engine/Rendering/Debug/DebugDraw.h"
#include "Game/Systems/Player/Player.h"
#include "Utility/Math/AngleConversion.h"
#include "Utility/StateMachine/StateMachine.h"

void ECS::Movement::Init()
{
}

void ECS::Movement::Deinit()
{
    if (StateMachine)
    {
        StateMachine->Deinit();
        StateMachine = nullptr;
    }
}

void ECS::Movement::Update(double InDelta)
{
    if (!StateMachine)
    {
        StateMachine = new MovementStateMachine();
        StateMachine->Init(GetPlayer().GetID());
    }
    if (StateMachine)
        StateMachine->Update(InDelta); 
    
    ConsumeRotInput();
    ConsomeMoveInput();
    ConsumeJumpInput();
    ApplySlowdown(InDelta); 
    ApplyVelocityClamp(InDelta);
    GroundSnap(); 
}

void ECS::Movement::OnBeginContact(const Physics::Contact& InContact)
{
    CHECK_RETURN(InContact.IsTrigger);
    CHECK_RETURN(OnGround);
    for (const auto& point : InContact.Points)
    {
        const float dot = point.Normal.Dot(Vec3F::Up());
        if (dot > GroundDot)
        {
            OnGround = true;
            GroundLocation = point.Position;
        }
    }
}

void ECS::Movement::ConsumeRotInput() const
{
    auto& trans = GetPlayerTransform();
    auto& camTrans = GetCameraTransform();
    auto& input = GetInput();
    
    // Set body rotation
    Vec3F bodyRot = trans.GetRotation().Euler();
    bodyRot.y += input.RotInput.y; 
    trans.SetRotation(bodyRot);

    // Set camera rotation
    float camRot = camTrans.GetRotation().Euler().x; 
    camRot += input.RotInput.x;
    camRot = CLAMP(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        camRot);
    camTrans.SetRotation(
        Vec3F(camRot, 0.0f, 0.0f),
        Transform::Space::LOCAL); 

    input.RotInput = Vec2F::Zero();
}

void ECS::Movement::ConsomeMoveInput()
{
    const auto& rb = GetRB(); 
    auto& input = GetInput();
    
    if (input.MoveInput.length > 0.01f)
    {
        LastInputVector = input.MoveInput.GetNormalized();
        const float force = MovementForce * (OnGround ? 1.0f : AirMovementMultiplier);
        const Vec2F vec = Vec2F(LastInputVector) * force;
        rb.AddForce(Vec3F(vec.x, 0.0f, vec.y));
        input.MoveInput = Vec2F::Zero(); 
    }
    else
    {
        LastInputVector = Vec2F::Zero();
    }
}

void ECS::Movement::ConsumeJumpInput()
{
    const auto& rb = GetRB(); 
    auto& input = GetInput();
    
    if (input.JumpInput && OnGround)
    {
        const Vec3F vel = rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f);
        rb.SetVelocity(vel + Vec3F::Up() * JumpVelocity.Get());
        OnGround = false; 
        input.JumpInput = false; 
    }
}

void ECS::Movement::ApplySlowdown(double InDelta)
{
    CHECK_RETURN(LastInputVector.Length > 0.001f);
    const float slowdown = OnGround ? GroundSlowdown : AirSlowdown;
    const float friction = std::pow(slowdown, InDelta);

    auto& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
    const Vec3F flatVel = vel * Vec3F(1.0f, 0.0f, 1.0f) * friction;
    const Vec3F newVel = Vec3F(flatVel.x, vel.y, flatVel.z);
    rb.SetVelocity(newVel);
}

void ECS::Movement::ApplyVelocityClamp(double InDelta) const
{
    auto& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();

    const Vec3F flatVel = vel * Vec3F(1.0f, 0.0f, 1.0f);
    const float maxSpeed = OnGround ?
        MaxGroundSpeed : MaxAirSpeed;
    const float currentSpeed = flatVel.Length();
    
    // Horizontal friction slowdown
    const float slowdown = OnGround ? GroundClampSlowdown : AirClampSlowdown;
    const float friction = std::pow(slowdown, InDelta);
    const float newSpeed = currentSpeed < maxSpeed ? currentSpeed : currentSpeed * friction;
    const Vec3F clampedFlatVel = flatVel.GetNormalized() * newSpeed;
    
    // Vertical raw clamp
    float clapedVertVel = CLAMP(vel.y, -MaxVerticalSpeed, MaxVerticalSpeed); 
    rb.SetVelocity(Vec3F(clampedFlatVel.x, clapedVertVel, clampedFlatVel.z));
}

void ECS::Movement::GroundSnap()
{
    CHECK_RETURN(!OnGround);

    constexpr bool debugDraw = false; 
    
    auto& transform = GetPlayerTransform();
    auto& collTrans = GetColliderTransform();
    const auto& collider = GetCollider();
    
    Physics::SweepParams params;
    params.Start = transform.GetPosition();
    params.End = params.Start - Vec3F::Up() * GroundDist.Get();
    params.IgnoredEntities = { GetID() };
    params.Shape = static_cast<Physics::Shape>(collider.Shape.Get());
    params.ShapeData = collider.ShapeData;
    params.Pose = collTrans.Local(); 

    if (debugDraw)
    {
        Rendering::DebugCapsule(params.Start, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y, RED); 
        Rendering::DebugCapsule(params.End, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y, BLUE);
    }
    
    // Sweep 
    const Physics::QueryResult result = Physics::Query::Sweep(params);
    CHECK_RETURN(!result.IsHit);
    CHECK_RETURN(result.Hits.empty());
    auto hit = result.ClosestHit();

    
    // Test ground dot
    const float dot = hit.Normal.Dot(Vec3F::Up());
    CHECK_RETURN(dot < GroundDot);

    // Set location
    GroundLocation = hit.Position;
    const Vec3F newPos = params.Start - Vec3F::Up() * hit.Distance; 
    transform.SetPosition(newPos);

    if (debugDraw)
        Rendering::DebugCapsule(newPos, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y, GREEN); 

    // Flatten velocity
    auto& rb = GetRB();
    Vec3F vel = rb.GetVelocity(); 
    rb.SetVelocity(vel * Vec3F(1.0f, 0.0f, 1.0f));
}

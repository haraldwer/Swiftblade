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
        const Vec2F force = Vec2F(input.MoveInput.normalized) * MovementForce.Get();
        rb.AddForce(Vec3F(force.x, 0.0f, force.y));
        input.MoveInput = Vec2F::Zero(); 
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

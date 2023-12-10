#include "Movement.h"

#include "MovementStateMachine.h"
#include "Game/ECS/Player/Input.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Contact.h"
#include "Engine/Physics/Query.h"
#include "Engine/Rendering/Debug/DebugDraw.h"
#include "Game/ECS/Player/Player.h"
#include "Game/ECS/Player/PlayerCamera.h"
#include "Game/ECS/Player/Sword/Sword.h"
#include "Utility/Math/AngleConversion.h"
#include "Utility/Math/Geometry/Plane.h"
#include "Utility/StateMachine/StateMachine.h"

void ECS::Movement::Init()
{
    StateMachine = new MovementStateMachine();
    StateMachine->Init();
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
    if (StateMachine)
        StateMachine->Update(InDelta);

    const bool prevGround = OnGround;
    GetMovement().GroundSnap();

    if (OnGround != prevGround)
        {LOG(OnGround ? "On ground" : "In air");}
}

bool ECS::Movement::Edit(const String& InName)
{
    bool edited = UniqueComponent::Edit(InName);  
    //if (StateMachine)
    //    if (StateMachine->Edit(InName))
    //        edited = true;  
    return edited; 
}

void ECS::Movement::Look(const Vec2F& InInput, const LookParams& InParams) const
{
    auto& trans = GetPlayerTransform();
    auto& camTrans = GetCameraTransform();
    
    // Set body rotation
    Vec3F bodyRot = trans.GetRotation().Euler();
    bodyRot.y += InInput.y * InParams.SensitivityMultiplier.y; 
    trans.SetRotation(bodyRot);

    // Set camera rotation
    float camRot = camTrans.GetRotation().Euler().x; 
    camRot += InInput.x * InParams.SensitivityMultiplier.x;
    camRot = CLAMP(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        camRot);
    camTrans.SetRotation(
        Vec3F(camRot, 0.0f, 0.0f),
        Transform::Space::LOCAL); 
}

bool ECS::Movement::Move(const Vec2F& InInput, const MoveParams& InParams) const
{
    const auto& rb = GetRB(); 
    CHECK_RETURN(InInput.length() <= InParams.InputDeadzone, false);
    const float force = InParams.MovementForce;
    const Vec2F vec = InInput.GetNormalized() * force;
    Vec3F forceVec = Vec3F(vec.x, 0.0f, vec.y); 

    // Project vec to PlaneNormal if set
    if (InParams.PlaneNormal.Length() > 0.1f)
        forceVec = Utility::Math::Plane(InParams.PlaneNormal).Project(forceVec);
    
    rb.AddForce(forceVec);
    return true;
}

void ECS::Movement::Jump(const JumpParams& InParams)
{
    const auto& rb = GetRB();
    const Vec3F vel = rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f);
    rb.SetVelocity(vel + Vec3F::Up() * InParams.JumpVelocity);
    JumpTimestamp = GetTime(); 
    OnGround = false;
    LOG("Jump"); 
}

void ECS::Movement::Slowdown(double InDelta, const SlowdownParams& InParams) const
{
    const float slowdown = InParams.Slowdown;
    const float friction = static_cast<float>(std::pow(slowdown, InDelta));

    auto& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
    const Vec3F flatVel = vel * Vec3F(1.0f, 0.0f, 1.0f) * friction;
    const Vec3F newVel = Vec3F(flatVel.x, vel.y, flatVel.z);
    rb.SetVelocity(newVel);
}

void ECS::Movement::VelocityClamp(double InDelta, const VelocityClampParams& InParams) const
{
    auto& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();

    const Vec3F flatVel = vel * Vec3F(1.0f, 0.0f, 1.0f);
    const float currentSpeed = flatVel.Length();
    
    // Horizontal friction slowdown
    const float friction = static_cast<float>(std::pow(InParams.ClampSlowdown, InDelta));
    const float newSpeed = currentSpeed < InParams.MaxSpeed ? currentSpeed : currentSpeed * friction;
    const Vec3F clampedFlatVel = flatVel.GetNormalized() * newSpeed;
    
    // Vertical raw clamp
    float clampedVertVel = CLAMP(vel.y, -InParams.MaxVerticalSpeed, InParams.MaxVerticalSpeed); 
    rb.SetVelocity(Vec3F(clampedFlatVel.x, clampedVertVel, clampedFlatVel.z));
}

void ECS::Movement::SetCrouch(bool InCrouch, const CrouchParams& InParams)
{
    CHECK_RETURN(InCrouch == Crouching);
    Crouching = InCrouch;

    // Retain velocity
    auto& rb = GetRB();
    Vec3F vel = rb.GetVelocity();
    
    // Change size of collider
    Collider& collider = GetCollider();
    Vec4F data = collider.ShapeData.Get();
    const float prevHeight = data.y;
    data.y = InCrouch ? data.y * InParams.HeightMul : 1.0f; 
    const float heightDiff = data.y - prevHeight;
    collider.ShapeData = data; 
    GetSystem<SysCollider>().UpdateShape(collider.GetID());

    rb.SetVelocity(vel);
    
    // Move down / up a bit
    Transform& trans = GetPlayerTransform();
    trans.SetPosition(trans.GetPosition() + Vec3F::Up() * heightDiff);

    PlayerCamera& cam = GetPlayerCamera();
    cam.AddCrouchOffset(heightDiff);

    if (!InCrouch)
        LOG("Uncrouch"); 
}

void ECS::Movement::GroundSnap()
{    
    CHECK_RETURN(TimeSinceJump() < GroundJumpDelay);
    
    OnGround = false;
    
    constexpr bool debugDraw = false; 
    
    auto& transform = GetPlayerTransform();
    auto& collTrans = GetColliderTransform();
    const auto& collider = GetCollider();
    
    Physics::SweepParams params;
    params.Start = transform.GetPosition();
    params.End = params.Start - Vec3F::Up() * GroundDist;
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
    for (auto hit : result.DistanceSorted())
    {
        // Test ground dot
        const float dot = hit.Normal.Dot(Vec3F::Up());
        CHECK_CONTINUE(dot < GroundDot);

        // Set location
        OnGround = true; 
        const Vec3F newPos = params.Start - Vec3F::Up() * hit.Distance; 
        transform.SetPosition(newPos);

        if (debugDraw)
            Rendering::DebugCapsule(newPos, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y, GREEN); 

        // Flatten velocity
        auto& rb = GetRB();
        Vec3F vel = rb.GetVelocity(); 
        rb.SetVelocity(vel * Vec3F(1.0f, 0.0f, 1.0f));
        return; 
    }
}

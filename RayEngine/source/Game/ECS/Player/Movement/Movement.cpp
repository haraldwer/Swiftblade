#include "Movement.h"

#include "ECS/Player/PlayerCamera.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Editor/Debug/Draw.h"
#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Query.h"
#include "MovementStateMachine.h"
#include "Utility/Math/Geometry/Plane.h"

void ECS::Movement::Init()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
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

void ECS::Movement::Update()
{
    if (Engine::Instance::Get().IsEditor())
        return;
    
    GroundSnap();

    if (StateMachine)
        StateMachine->Update();
}

void ECS::Movement::OnBeginContact(const Physics::Contact& InContact)
{
    CHECK_RETURN(InContact.IsTrigger);
    CHECK_RETURN(OnGround);
    CHECK_RETURN(TimeSinceJump() < GroundJumpDelay);
    for (auto& point : InContact.Points)
    {
        if (CheckGroundHit(point.Normal))
        {
            OnGround = true;
            GroundTimestamp = GetTime(); 
            break;
        }
    }
}

void ECS::Movement::Look(const Vec2F& InInput, const LookParams& InParams) const
{
    auto& trans = GetPlayerTransform();
    auto& camTrans = GetCameraTransform();
    
    // Set body rotation
    Vec3F bodyRot = trans.GetRotation().Euler();
    bodyRot.y += InInput.y * InParams.SensitivityMultiplier.Get().y; 
    trans.SetRotation(QuatF::FromEuler(bodyRot));

    // Set camera rotation
    float camRot = camTrans.GetRotation().Euler().x; 
    camRot += InInput.x * InParams.SensitivityMultiplier.Get().x;
    camRot = Utility::Math::Clamp(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        camRot);
    camTrans.SetRotation(
        QuatF::FromEuler(Vec3F(camRot, 0.0f, 0.0f)),
        Transform::Space::LOCAL); 
}

bool ECS::Movement::Move(const Vec2F& InInput, const MoveParams& InParams) const
{
    const auto& rb = GetRB(); 
    CHECK_RETURN(InInput.Length() <= InParams.InputDeadzone, false);
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
    const Vec3F newVel = vel +
        Vec3F::Up() * InParams.UpVelocity.Get() +
        InParams.Direction * InParams.DirectionalForce.Get(); 
    rb.SetVelocity(newVel);
    if (OnGround)
    {
        OnGround = false;
        GroundTimestamp = GetTime(); 
    }
    LOG("Jump"); 
}

void ECS::Movement::Slowdown(double InDelta, const SlowdownParams& InParams) const
{
    const float slowdown = InParams.Friction;
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
    const float friction = static_cast<float>(std::pow(InParams.ClampSlowdown.Get(), InDelta));
    const float newSpeed = currentSpeed < InParams.MaxSpeed ? currentSpeed : currentSpeed * friction;
    const Vec3F clampedFlatVel = flatVel.GetNormalized() * newSpeed;
    
    // Vertical raw clamp
    float clampedVertVel = Utility::Math::Clamp(vel.y, -InParams.MaxVerticalSpeed.Get(), InParams.MaxVerticalSpeed.Get()); 
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

double ECS::Movement::TimeSinceJump() const
{
    CHECK_ASSERT(!StateMachine, "Invalid ptr");
    const auto jumpState = StateMachine->GetState<MovementStateJump>();
    CHECK_ASSERT(!jumpState, "Missing jump state");
    return jumpState->GetTimeSinceEnter();
}

double ECS::Movement::TimeSinceLeftGround() const
{
    return GetTime() - GroundTimestamp; 
}

void ECS::Movement::GroundSnap()
{
    CHECK_RETURN(!OnGround);
    CHECK_RETURN(TimeSinceJump() < GroundJumpDelay);

    OnGround = false;
    
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
        Engine::DebugCapsule(params.Start, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y); 
        Engine::DebugCapsule(params.End, params.Pose.GetRotation(), params.ShapeData.x, params.ShapeData.y);
    }

    // Sweep
    const Physics::QueryResult result = Physics::Query::Sweep(params);
    
    if (result.IsHit)
    {
        for (auto hit : result.DistanceSorted())
        {
            if (CheckGroundHit(hit.Normal))
            {
                // Set location
                OnGround = true;
                GroundTimestamp = GetTime(); 
    
                const Vec3F newPos = transform.GetPosition() - Vec3F::Up() * hit.Distance; 
                transform.SetPosition(newPos);
    
                // Flatten velocity
                auto& rb = GetRB();
                Vec3F vel = rb.GetVelocity(); 
                rb.SetVelocity(vel * Vec3F(1.0f, 0.0f, 1.0f));
            
                if (debugDraw)
                    Engine::DebugCapsule(
                        transform.GetPosition(),
                        params.Pose.GetRotation(),
                        params.ShapeData.x,
                        params.ShapeData.y);
                break;
            } 
        }
    }
}

bool ECS::Movement::CheckGroundHit(const Vec3F& InNormal) const
{
    // Test ground dot
    const float dot = Vec3F::Dot(InNormal, Vec3F::Up());
    CHECK_RETURN(dot < GroundDot, false);
    return true; 
}

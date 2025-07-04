﻿#include "MovementStateWall.h"

#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "MovementStateAir.h"
#include "MovementStateIdle.h"

Type MovementStateWall::Check()
{
    const auto& movement = GetMovement();
    const bool recentJump = movement.TimeSinceJump() < EnterAirDelay;
    if (movement.IsInAir() && !recentJump && CheckInput() && CheckWall())
        return GetType();
    return Type::None(); 
}

Type MovementStateWall::Update()
{
    // Is still close to wall?
    if (!CheckWall())
        return Type::Get<MovementStateAir>();

    if (!CheckInput())
        return Type::Get<MovementStateAir>();

    const auto& movement = GetMovement();
    const auto& input = GetInput();
    
    if (!movement.IsInAir())
        return Type::Get<MovementStateIdle>();

    const float dt = static_cast<float>(Utility::Time::Get().Delta());
    
    // Interp wall normal
    if (targetWallNormal.LengthSqr() > 0.1f)
    {
        currentWallNormal = Utility::Math::Lerp(currentWallNormal, targetWallNormal, NormalInterpSpeed * static_cast<float>(dt));
        currentWallNormal = currentWallNormal.GetNormalized();
    }

    // Set params
    auto move = Move.Get();
    move.planeNormal = currentWallNormal;
    
    movement.Look(input.rotInput, Look);
    if (!movement.Move(input.moveInput, move))
        movement.Slowdown(dt, Slowdown); 
    movement.VelocityClamp(dt, VelocityClamp);
    
    return Type::None(); 
}

void MovementStateWall::Enter()
{
    LOG("Enter wall");
    MovementState::Enter();
    auto& rb = GetRB();
    rb.GravityScale = 0.0f;
    rb.SetVelocity(rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f));
    currentWallNormal = targetWallNormal;
}

void MovementStateWall::Exit()
{
    LOG("Exit wall");
    MovementState::Exit();
    GetRB().GravityScale = 1.0f;
    targetWallNormal = Vec3F::Zero();
}

bool MovementStateWall::CheckWall()
{
    ECS::Collider& collider = GetCollider();
    const ECS::Transform& trans = GetPlayerTransform();
    const Mat4F world = trans.World();
    
    Physics::SweepParams params;
    params.start = world.GetPosition();
    params.end = params.start;
    params.pose = GetColliderTransform().Local();
    params.shape = static_cast<Physics::Shape>(collider.Shape.Get());
    params.shapeData = collider.ShapeData.Get() * 0.8f;
    params.ignoredEntities = { GetPlayerID() };

    Physics::SweepParams rightParams = params;
    const Vec3F rightDir = world.Right() * SweepLength.Get();
    rightParams.end += rightDir;
    Physics::SweepParams leftParams = params; 
    leftParams.end -= rightDir; 
    
    //Rendering::DebugCapsule(rightParams.End, rightParams.Pose.GetRotation(), rightParams.ShapeData.x, rightParams.ShapeData.y, RED);
    //Rendering::DebugCapsule(leftParams.End, leftParams.Pose.GetRotation(), leftParams.ShapeData.x, leftParams.ShapeData.y, RED);

    const Physics::QueryResult right = Physics::Query::Sweep(rightParams);
    const Physics::QueryResult left = Physics::Query::Sweep(leftParams);
    
    onWall = false;
    const bool isCurrent = IsCurrentState();
    Vec3F normal = isCurrent ? targetWallNormal : Vec3F::Zero();

    // Cache input
    const Vec2F inputVec = GetInput().moveInput.GetNormalized();
    const Vec3F inputDir = Vec3F(inputVec.x, 0.0f, inputVec.y);
    
    auto processHit = [&](const Physics::QueryResult::Hit& InHit)
    {
        // Check vertical dot
        const float vertDot = Vec3F::Dot(InHit.normal, Vec3F::Up());
        CHECK_RETURN(abs(vertDot) > MaxVerticalDot.Get());
        
        //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + InHit.Normal + world.Forward(), GREEN);
        
        // Check input dot
        const float inputDot = inputDir.LengthSqr() > 0.1f ? Vec3F::Dot(InHit.normal, inputDir * -1.0f) : 1.0f;
        CHECK_RETURN(inputDot < MinWallInputDot.Get());

        // Prioritize hits close to WallNormal
        const float dot = Vec3F::Dot(InHit.normal, targetWallNormal);
        const float clamped = Utility::Math::Clamp(dot, 0.0f, 1.0f); 
        // If wallNormal is set
        const float normalMul = isCurrent ? clamped : 1.0f;
        
        // Maybe ignore hit
        CHECK_RETURN(normalMul < 0.01f); 

        // Is hit!
        onWall = true; 
        // Add to result
        normal += InHit.normal * normalMul;
    };

    if (right.isHit)
        processHit(right.ClosestHit());
    if (left.isHit)
        processHit(left.ClosestHit());
    targetWallNormal = normal.GetNormalized();

    //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + TargetWallNormal * 0.5f + world.Forward());
    //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + CurrentWallNormal * 0.5f + world.Forward(), BLUE);

    return onWall; 
}

bool MovementStateWall::CheckInput() const
{
    return GetInput().moveInput.Length() > InputThreshold ||
        (GetRB().GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f)).Length() > VelocityThreshold;
}

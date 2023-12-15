#include "MovementStateWall.h"

#include "MovementStateIdle.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "Game/ECS/Player/Input.h"
#include "Game/ECS/Player/Movement/Movement.h"

Type MovementStateWall::Check()
{
    auto& movement = GetMovement(); 
    bool recentlyOnGround = movement.TimeSinceLeftGround() < EnterAirDelay;
    if (CheckWall() && CheckInput() && movement.IsInAir() && !recentlyOnGround)
        return Type::Get<MovementStateWall>();
    return Type::None(); 
}

Type MovementStateWall::Update(double InDT)
{
    // Is still close to wall?
    if (!CheckWall() || !CheckInput() || !GetMovement().IsInAir())
        return Type::Get<MovementStateIdle>();
    
    const auto& movement = GetMovement();
    const auto& input = GetInput();

    // Interp wall normal
    if (TargetWallNormal.LengthSqr() > 0.1f)
        CurrentWallNormal = LERP(CurrentWallNormal, TargetWallNormal, NormalInterpSpeed * static_cast<float>(InDT));

    // Set params
    ECS::Movement::MoveParams move;
    move.PlaneNormal = CurrentWallNormal;
    
    movement.Look(input.RotInput);
    if (!movement.Move(input.MoveInput, move))
        movement.Slowdown(InDT); 
    movement.VelocityClamp(InDT);
    
    return Type::None(); 
}

void MovementStateWall::Enter()
{
    LOG("Enter wall");
    MovementState::Enter();
    auto& rb = GetRB();
    rb.GravityScale = 0.0f;
    rb.SetVelocity(rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f));
    CurrentWallNormal = TargetWallNormal;
}

void MovementStateWall::Exit()
{
    LOG("Exit wall");
    MovementState::Exit(); 
    GetRB().GravityScale = 1.0f;
    TargetWallNormal = Vec3F::Zero(); 
}

bool MovementStateWall::CheckWall()
{
    ECS::Collider& collider = GetCollider();
    const ECS::Transform& trans = GetPlayerTransform();
    const Mat4F world = trans.World();
    
    Physics::SweepParams params;
    params.Start = world.GetPosition();
    params.End = params.Start;
    params.Pose = GetColliderTransform().Local();
    params.Shape = static_cast<Physics::Shape>(collider.Shape.Get());
    params.ShapeData = collider.ShapeData.Get() * 0.8f;
    params.IgnoredEntities = { GetPlayerID() };

    Physics::SweepParams rightParams = params;
    const Vec3F rightDir = world.Right() * SweepLength.Get();
    rightParams.End += rightDir;
    rightParams.Start += rightDir * 0.1f; 
    Physics::SweepParams leftParams = params; 
    leftParams.End -= rightDir; 
    leftParams.Start -= rightDir * 0.1f; 
    
    //Rendering::DebugCapsule(rightParams.End, rightParams.Pose.GetRotation(), rightParams.ShapeData.x, rightParams.ShapeData.y, RED);
    //Rendering::DebugCapsule(leftParams.End, leftParams.Pose.GetRotation(), leftParams.ShapeData.x, leftParams.ShapeData.y, RED);

    const Physics::QueryResult right = Physics::Query::Sweep(rightParams);
    const Physics::QueryResult left = Physics::Query::Sweep(leftParams);
    
    OnWall = false;
    TargetWallNormal = Vec3F();

    // Cache input
    const Vec2F inputVec = GetInput().MoveInput.GetNormalized();
    const Vec3F inputDir = Vec3F(inputVec.x, 0.0f, inputVec.y);
    
    auto processHit = [&](const Physics::QueryResult::Hit& InHit)
    {
        // Check vertical dot
        const float vertDot = InHit.Normal.Dot(Vec3F::Up());
        CHECK_RETURN(abs(vertDot) > MaxVerticalDot.Get());
        
        //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + InHit.Normal + world.Forward(), GREEN);
        
        // Check input dot
        const float inputDot = InHit.Normal.Dot(inputDir * -1.0f);
        CHECK_RETURN(inputDot < MinWallInputDot.Get());

        // Prioritize hits close to WallNormal
        const float dot = InHit.Normal.Dot(TargetWallNormal);
        const float clamped = CLAMP(dot, 0.0f, 1.0f); 
        // If wallNormal is set
        const float normalMul = TargetWallNormal.LengthSqr() > 0.1f ? clamped : 1.0f;
        
        // Maybe ignore hit
        CHECK_RETURN(normalMul < 0.01f); 

        // Is hit!
        OnWall = true; 
        // Add to result
        TargetWallNormal += InHit.Normal * normalMul;
    };
    
    for (auto& hit : right.DistanceSorted())
        processHit(hit);
    for (auto& hit : left.DistanceSorted())
        processHit(hit);
    TargetWallNormal = TargetWallNormal.GetNormalized();

    //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + TargetWallNormal * 0.5f + world.Forward());
    //Rendering::DebugLine(world.GetPosition() + world.Forward(), world.GetPosition() + CurrentWallNormal * 0.5f + world.Forward(), BLUE);

    return OnWall; 
}

bool MovementStateWall::CheckInput()
{
    return GetInput().MoveInput.Length() > InputThreshold; 
}

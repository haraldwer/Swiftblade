#include "MovementStateWall.h"

#include "MovementStateIdle.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "Game/Systems/Player/Input.h"
#include "Game/Systems/Player/Movement/Movement.h"

Type MovementStateWall::Check()
{
    if (CheckWall() && CheckInput() && GetMovement().IsInAir())
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
    auto& rb = GetRB();
    rb.GravityScale = 0.0f;
    rb.SetVelocity(rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f));
    CurrentWallNormal = TargetWallNormal;
    LOG("Enter wall")
}

void MovementStateWall::Exit()
{
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
    params.Shape = static_cast<Physics::Shape>(collider.Shape.Get());
    params.ShapeData = collider.ShapeData;
    params.IgnoredEntities = { GetPlayerID() };

    Physics::SweepParams rightParams = params;
    rightParams.End += world.Right() * SweepLength.Get();
    Physics::SweepParams leftParams = params;
    leftParams.End -= world.Right() * SweepLength.Get();

    const Physics::QueryResult right = Physics::Query::Sweep(rightParams);
    const Physics::QueryResult left = Physics::Query::Sweep(leftParams);

    OnWall = false;
    TargetWallNormal = Vec3F();
    auto processHit = [&](const Physics::QueryResult::Hit& InHit)
    {
        // Check vertical dot
        const float vertDot = InHit.Normal.Dot(Vec3F::Up());
        CHECK_RETURN(abs(vertDot) > MaxVerticalDot.Get());

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
    
    for (auto& hit : right.Hits)
        processHit(hit);
    for (auto& hit : left.Hits)
        processHit(hit);
    TargetWallNormal = TargetWallNormal.GetNormalized();
    return OnWall; 
}

bool MovementStateWall::CheckInput()
{
    return GetInput().MoveInput.Length() > InputThreshold; 
}

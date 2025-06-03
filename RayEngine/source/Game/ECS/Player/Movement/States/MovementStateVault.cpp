#include "MovementStateVault.h"

#include "ECS/Player/Movement/Movement.h"
#include "ECS/Player/PlayerInput.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Query.h"
#include "MovementStateAir.h"
#include "MovementStateIdle.h"
#include "MovementStateWall.h"

Type MovementStateVault::Check()
{
    if (GetMovement().IsCrouching())
        return Type::None();

    if (GetCurrentState() == Type::Get<MovementStateWall>())
        return Type::None(); 

    // Do not update cached result if already in state
    if (IsCurrentState())
        return Type::None();
    
    const Vec2F input = GetInput().moveInput;
    if (input.Length() < Deadzone)
        return Type::None();
    sweepDirection = Vec3F(input.x, 0.0f, input.y).GetNormalized();

    const Physics::QueryResult result = Sweep();
    if (!result.isHit)
        return Type::None();
    
    // Cache result
    const Physics::QueryResult::Hit hit = result.ClosestHit();
    ledgeLocation = hit.position;

    return Type::Get<MovementStateVault>();
}

Type MovementStateVault::Update()
{
    // Move towards Y
    // Input has to be above a threshold and in the right direction

    if (const auto air = GetState<MovementStateAir>())
        air->Update();

    const ECS::Rigidbody& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
    if (vel.y < UpSpeed)
        rb.SetVelocity(Vec3F(vel.x, UpSpeed.Get(), vel.z));

    //Rendering::DebugSphere(LedgeLocation.y, 0.1f, BLUE);
    
    const Vec2F input = GetInput().moveInput;
    if (input.Length() < Deadzone)
        return Type::Get<MovementStateIdle>();

    const Vec3F dir = Vec3F(input.x, 0.0f, input.y).GetNormalized();
    const float dot = Vec3F::Dot(dir, sweepDirection);
    if (dot < InputDot)
        return Type::Get<MovementStateIdle>();

    const float targetY = ledgeLocation.y + HeightOffset;
    if (GetPlayerTransform().GetPosition().y > targetY)
        return Type::Get<MovementStateIdle>();
    
    return Type::None();
}

void MovementStateVault::Exit()
{
    MovementState::Exit();

    // Limit exit Y velocity
    const ECS::Rigidbody& rb = GetRB();
    const Vec3F vel = rb.GetVelocity();
    rb.SetVelocity(Vec3F(vel.x, 0.0f, vel.z));
}

Physics::QueryResult MovementStateVault::Sweep() const
{
    const ECS::Transform& trans = GetPlayerTransform();
    const Mat4F world = trans.World();

    Physics::SweepParams baseParams;
    baseParams.shape = Physics::Shape::SPHERE;
    baseParams.shapeData.x = SweepRadius;
    baseParams.ignoredEntities = { GetPlayerID() };

    // First do forward sweep
    Physics::SweepParams forwardParams = baseParams;
    forwardParams.start = world.GetPosition() + Vec3F::Up() * SweepHeight.Get();
    forwardParams.end = forwardParams.start + sweepDirection * ForwardDist.Get(); 
    //Rendering::DebugLine(forwardParams.Start, forwardParams.End, GREEN);
    //Rendering::DebugSphere(forwardParams.End, forwardParams.ShapeData.x, GREEN);
    const Physics::QueryResult forwardResult = Physics::Query::Sweep(forwardParams);
    if (forwardResult.isHit) // Exit if hit
        return {};

    // Then downwards
    Physics::SweepParams downParams = baseParams;
    downParams.start = forwardParams.end;
    downParams.end = downParams.start - Vec3F::Up() * SweepHeight.Get() * 2.0f;
    //Rendering::DebugLine(downParams.Start, downParams.End);
    //Rendering::DebugSphere(downParams.Start, downParams.ShapeData.x);
    auto result = Physics::Query::Sweep(downParams);
    //if (result.IsHit)
    //    Rendering::DebugSphere(result.ClosestHit().Position, downParams.ShapeData.x, BLUE);

    return result;
}

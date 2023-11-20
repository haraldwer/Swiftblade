#include "Movement.h"

#include "Input.h"
#include "Engine/ECS/Systems/Collider.h"
#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Contact.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Debug/DebugDraw.h"
#include "Utility/Math/AngleConversion.h"

void ECS::Movement::Update(double InDelta)
{
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
        rb.SetVelocity(Vec3F::Up() * JumpVelocity.Get());
        OnGround = false; 
        input.JumpInput = false; 
    }
}

void ECS::Movement::GroundSnap()
{
    CHECK_RETURN(!OnGround);

    auto& transform = GetPlayerTransform();
    const auto& collider = GetCollider();
    
    const Vec4F shapeData = collider.ShapeData;
    const Vec3F pos = transform.GetPosition();
    const Vec3F start = pos - Vec3F::Up() * shapeData.y;
    const Vec3F end = start - Vec3F::Up() * GroundDist.Get();
    
    // TODO: Sweep
    const Physics::TraceResult result = Physics::Manager::Get().Trace(start, end);
    
    CHECK_RETURN(!result.IsHit);
    CHECK_RETURN(result.Hits.empty());

    const auto& hit = result.Hits[0];
    const float dot = hit.Normal.Dot(Vec3F::Up());
    CHECK_RETURN(dot < GroundDot);
    
    const Vec3F loc = hit.Position; 
    GroundLocation = loc;
    const Vec3F newPos = loc + Vec3F::Up() * shapeData.y;
    LOG("Diff " + std::to_string((newPos - transform.GetPosition()).Length)); 
    transform.SetPosition(newPos);
    auto& rb = GetRB();
    rb.SetVelocity(rb.GetVelocity() * Vec3F(1.0f, 0.0f, 1.0f)); 
        
    Rendering::DebugSphere(hit.Position, 0.1f); 
    Rendering::DebugSphere(newPos, 0.1f);
}

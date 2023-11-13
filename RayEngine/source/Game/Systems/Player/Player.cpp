#include "Player.h"

#include "Engine/ECS/Systems/Rigidbody.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Utility/Math/AngleConversion.h"

void ECS::SysPlayer::Deinit(EntityID InID, Player& InComponent)
{
    if (IsCursorHidden())
        ShowCursor();
}

void ECS::SysPlayer::Update(EntityID InID, Player& InComponent, double InDelta)
{
    if (!IsCursorHidden())
        HideCursor();
    
    auto& trans = Get<Transform>(InID);
    auto& rb = Get<Rigidbody>(InID);

    // Camera rotation
    const auto mouseDelta = GetMouseDelta();
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    Vec3F rotDelta = Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.005f;
    Vec3F newRot = trans.GetRotation().Euler() + rotDelta;
    newRot.x = CLAMP(
            Utility::Math::DegreesToRadians(-90.0f),
            Utility::Math::DegreesToRadians(90.0f),
            newRot.x);
    trans.SetRotation(newRot); 

    // Get directions
    const Mat4F rotMat = Mat4F(trans.GetRotation());
    const Vec3F up = Vec3F::Up();
    const Vec3F right = (rotMat.Right() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized() * -1.0f;
    const Vec3F forward = (rotMat.Forward() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();

    // Calculate force
    const Vec3F force =
        right * (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) +
        forward * (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S)));

    if (force.length > 0.01f)
        rb.AddForce(Vec3F(force.normalized) * InComponent.MovementForce.Get());

    if (IsKeyPressed(KEY_SPACE))
        rb.AddImpulse(up * InComponent.JumpForce.Get());
}

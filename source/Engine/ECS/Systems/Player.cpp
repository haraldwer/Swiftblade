#include "Player.h"

#include "Rigidbody.h"
#include "Transform.h"

void ECS::SysPlayer::Update(EntityID InID, Player& InComponent, double InDelta)
{
    auto& trans = Get<Transform>(InID);
    auto& rb = Get<Rigidbody>(InID);
    auto& rbSys = GetSystem<SysRigidbody>();

    // Camera rotation
    const auto mouseDelta = GetMouseDelta();
    Vec3F rotDelta = Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.01f;
    trans.Rotation = QuatF(trans.Rotation.Get().Euler() + rotDelta);

    // Get directions
    const Mat4F rotMat = Mat4F(trans.Rotation);
    const Vec3F up = Vec3F::Up();
    const Vec3F right = rotMat.Right() * -1.0f;
    const Vec3F forward = rotMat.Forward();

    // Calculate force
    const Vec3F force =
        right * (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) +
        forward * (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S)));

    if (force.length > 0.01f)
        rbSys.AddForce(InID, Vec3F(force.normalized) * InComponent.MovementForce.Get());

    if (IsKeyPressed(KEY_SPACE))
        rbSys.AddImpulse(InID, up * InComponent.JumpForce.Get());
}
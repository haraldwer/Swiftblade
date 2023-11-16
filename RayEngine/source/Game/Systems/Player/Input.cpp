#include "Input.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Movement.h"

void ECS::SysInput::Deinit(EntityID InID, Input& InComponent)
{
    if (IsCursorHidden())
        ShowCursor();
}

void ECS::SysInput::Update(EntityID InID, Input& InComponent, double InDelta)
{
    if (!IsCursorHidden())
        HideCursor();
    
    auto& trans = Get<Transform>(InID);
    auto& movement = Get<Movement>(InID);

    // Camera rotation
    const auto mouseDelta = GetMouseDelta();
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    Vec2F rotDelta = Vec2F(mouseDelta.y, mouseDelta.x * -1.0f) * InComponent.Sensitivity.Get();
    movement.RotInput += rotDelta;  

    // Get directions
    const Mat4F rotMat = Mat4F(trans.GetRotation());
    const Vec3F right = (rotMat.Right() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized() * -1.0f;
    const Vec3F forward = (rotMat.Forward() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();

    // Movement input
    const Vec3F input =
        right * (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) +
        forward * (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S)));
    movement.MoveInput += input.xz;

    if (IsKeyPressed(KEY_SPACE))
        movement.JumpInput = true; 
}

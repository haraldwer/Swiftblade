#include "PlayerInput.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Input/Manager.h"
#include "Movement/Movement.h"

void ECS::PlayerInput::Deinit()
{
    if (IsCursorHidden())
        ShowCursor();
}

void ECS::PlayerInput::Update(double InDelta)
{
    if (Blocked)
        return;
    
    if (!IsWindowFocused())
        return; 
    
    if (!IsCursorHidden())
        HideCursor();
    
    auto& trans = Get<Transform>(GetID());

    // Camera rotation
    const auto mouseDelta = GetMouseDelta();
    SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
    Vec2F rotDelta = Vec2F(mouseDelta.y, mouseDelta.x * -1.0f) * Sensitivity.Get();
    RotInput = rotDelta;  

    // Get directions
    const Mat4F rotMat = Mat4F(trans.GetRotation());
    const Vec3F right = (rotMat.Right() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized() * -1.0f;
    const Vec3F forward = (rotMat.Forward() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();


    auto& man = Input::Manager::Get();
    
    // Movement input
    const Vec3F input =
        right * (static_cast<float>(man.Action("Right").Down()) - static_cast<float>(man.Action("Left").Down())) +
        forward * (static_cast<float>(man.Action("Forward").Down()) - static_cast<float>(man.Action("Back").Down()));
    MoveInput = input.xz;

    JumpInput = man.Action("Jump").Pressed();
    CrouchInput = man.Action("Crouch").Down();
    DashInput = man.Action("Dash").Down();
}


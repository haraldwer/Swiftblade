#include "PlayerInput.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Input/Manager.h"
#include "Movement/Movement.h"

void ECS::PlayerInput::Deinit()
{
}

void ECS::PlayerInput::Init()
{
    
}

void ECS::PlayerInput::Update(double InDelta)
{
    auto& trans = Get<Transform>(GetID());

    // Camera rotation
    const Vec2F mouseDelta = {
        Input::Action::Get("LookHorizontal").Axis(),
        Input::Action::Get("LookVertical").Axis()
    };
    
    Vec2F rotDelta = Vec2F(mouseDelta.y, mouseDelta.x * -1.0f) * Sensitivity.Get();
    RotInput = rotDelta;  

    // Get directions
    const Mat4F rotMat = Mat4F(trans.GetRotation());
    const Vec3F right = (rotMat.Right() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized() * -1.0f;
    const Vec3F forward = (rotMat.Forward() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();
    
    // Movement input
    const Vec3F input =
        right * (static_cast<float>(Input::Action::Get("Right").Down()) - static_cast<float>(Input::Action::Get("Left").Down())) +
        forward * (static_cast<float>(Input::Action::Get("Forward").Down()) - static_cast<float>(Input::Action::Get("Backward").Down()));
    MoveInput = input.xz;

    JumpInput = Input::Action::Get("Jump").Pressed();
    CrouchInput = Input::Action::Get("Crouch").Down();
    DashInput = Input::Action::Get("Dash").Down();
}


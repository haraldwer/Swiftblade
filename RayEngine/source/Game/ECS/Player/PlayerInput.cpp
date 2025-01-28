#include "PlayerInput.h"

#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Input/Manager.h"
#include "Movement/Movement.h"

void ECS::PlayerInput::Update()
{
    auto& trans = Get<Transform>(GetID());

    Input::Manager& man = Input::Manager::Get(); 
    
    // Camera rotation
    const Vec2F mouseDelta = {
        man.GetAction("LookHorizontal").Axis(),
        man.GetAction("LookVertical").Axis()
    };
    
    Vec2F rotDelta = Vec2F(mouseDelta.y, mouseDelta.x * -1.0f) * Sensitivity.Get();
    RotInput = rotDelta;  

    // Get directions
    const Mat4F rotMat = Mat4F(trans.GetRotation());
    const Vec3F right = (rotMat.Right() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized() * -1.0f;
    const Vec3F forward = (rotMat.Forward() * Vec3F(1.0f, 0.0f, 1.0f)).GetNormalized();
    
    // Movement input
    const Vec3F input =
        right * (static_cast<float>(man.GetAction("Right").Down()) - static_cast<float>(man.GetAction("Left").Down())) +
        forward * (static_cast<float>(man.GetAction("Forward").Down()) - static_cast<float>(man.GetAction("Backward").Down()));
    MoveInput = { input.x, input.z };

    JumpInput = man.GetAction("Jump").Pressed();
    JumpInputHeld = man.GetAction("Jump").Down();
    CrouchInput = man.GetAction("Crouch").Down();
    DashInput = man.GetAction("Dash").Down();
}


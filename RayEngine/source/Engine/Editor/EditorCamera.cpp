#include "EditorCamera.h"

#include "BlueprintEditor.h"
#include "Utility/Math/AngleConversion.h"
#include "raylib.h"

void EditorCamera::Update()
{
    if (Input::Action::Get("EditorCamera").Pressed())
    {
        if (bAlwaysEnabled)
            bRequireHold = !bRequireHold; 
        else
            Toggle();
    }

    const bool previousEnableInput = bInputEnabled;
    const Input::Action& rm = Input::Action::Get("RM"); 
    bInputEnabled = bUseEditCamera && (!bRequireHold || rm.Down());  
    if (bInputEnabled != previousEnableInput)
    {
        if (bInputEnabled)
        {
            Input::Manager::Get().Push("EditorCamera");
        }
        else
        {
            Input::Manager::Get().Pop("EditorCamera");
        }
    }

    if (bUseEditCamera)
        UpdateMovement(); 
}

void EditorCamera::Deinit()
{
    if (bInputEnabled)
        Input::Manager::Get().Pop("EditorCamera");
    bInputEnabled = false;
    if (bUseEditCamera)
        Input::Manager::Get().Pop("Default");
    bUseEditCamera = false;
}

void EditorCamera::SetTarget(Vec3F InPosition, float InDistance)
{
    Vec3F dir = Vec3F(1.0f, -1.0f, 1.0f).GetNormalized();
    TargetState.Position = InPosition - dir * InDistance;
    TargetState.Rotation = QuatF::FromDirection(dir).Euler();
    CurrentState = TargetState;
}

void EditorCamera::Toggle()
{
    bUseEditCamera = !bUseEditCamera;
    if (bUseEditCamera)
    {
        auto& cam = Engine::Instance::Get().GetRenderScene().GetCamera();
        if (cam.FOV > 1.0f)
        {
            TargetState.Position = cam.Position;
            TargetState.Rotation = cam.Rotation.Euler();
            TargetState.FOV = cam.FOV;
        }
        CurrentState = TargetState;
        Input::Manager::Get().Push("Default");
    }
    else
    {
        Input::Manager::Get().Pop("Default");
    }
}

void EditorCamera::UpdateMovement()
{
    float dt = static_cast<float>(Utility::Time::Get().Unscaled());
    
    auto& man = Input::Manager::Get();
    const bool ctrl = man.GetAction("Control", "EditorCamera").Down(); 
    
    // Calculate rotation
    const Vec2F mouseDelta = {
        man.GetAction("LookHorizontal", "EditorCamera").Axis(),
        man.GetAction("LookVertical", "EditorCamera").Axis()
    };
    TargetState.Rotation +=
        Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.0025f *
        static_cast<float>(!ctrl);
    TargetState.Rotation.x = Utility::Math::Clamp(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        TargetState.Rotation.x);  

    const Mat4F rotMat = Mat4F::FromEuler(TargetState.Rotation);
    const Vec3F up = Vec3F::Up();
    const Vec3F right = rotMat.Right() * -1.0f;
    const Vec3F forward = rotMat.Forward();

    const float ScrollDelta = GetMouseWheelMove() * 0.1f;
    TargetState.MovementSpeed = Utility::Math::Clamp(TargetState.MovementSpeed + ScrollDelta * (50.0f + TargetState.MovementSpeed * 0.5f), 1.0f, 300.0f);
    
    // Add position
    const Vec3F posDelta =
        (up * (
            static_cast<float>(man.GetAction("Up", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Down", "EditorCamera").Down())) +
        right * (
            static_cast<float>(man.GetAction("Right", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Left", "EditorCamera").Down())) +
        forward * (
            static_cast<float>(man.GetAction("Forward", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("Backward", "EditorCamera").Down()))) * 
        static_cast<float>(!ctrl);
    TargetState.Position += Vec3F(posDelta.GetNormalized()) * static_cast<float>(dt) * TargetState.MovementSpeed;
    
    // FOV
    TargetState.FOV += (
            static_cast<float>(man.GetAction("IncreaseFOV", "EditorCamera").Down()) -
            static_cast<float>(man.GetAction("DecreaseFOV", "EditorCamera").Down())) *
        static_cast<float>(!ctrl) * 
        static_cast<float>(dt) * 30.0f;

    // TODO: Interp account for dt
    CurrentState.Rotation = TargetState.Rotation;
    CurrentState.Position = Utility::Math::Lerp(CurrentState.Position, TargetState.Position, 10.0f * static_cast<float>(dt));
    CurrentState.FOV = Utility::Math::Lerp(CurrentState.FOV, TargetState.FOV, 10.0f * static_cast<float>(dt));
    CurrentState.MovementSpeed = Utility::Math::Lerp(CurrentState.MovementSpeed, TargetState.MovementSpeed, 10.0f * static_cast<float>(dt));
    
    // Set camera
    Engine::Instance::Get().GetRenderScene().SetCamera({
       CurrentState.Position,
        QuatF::FromEuler(CurrentState.Rotation),
        CurrentState.FOV,
        50.0f,
        0.01f
   });
}

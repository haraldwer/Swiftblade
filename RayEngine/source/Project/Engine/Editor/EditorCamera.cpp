#include "EditorCamera.h"

#include "BlueprintEditor.h"
#include "Utility/Math/AngleConversion.h"

void EditorCamera::Update(double InDelta)
{
    auto& man = Input::Manager::Get();
    const bool ctrl = man.Action("Control", "EditorCamera").Down(); 
    
    // Calculate rotation
    const Vec2F mouseDelta = {
        Input::Action::Get("LookHorizontal").Axis(),
        Input::Action::Get("LookVertical").Axis()
    };
    TargetState.Rotation +=
        Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.005f *
        static_cast<float>(!ctrl);
    TargetState.Rotation.x = CLAMP(
        Utility::Math::DegreesToRadians(-90.0f),
        Utility::Math::DegreesToRadians(90.0f),
        TargetState.Rotation.x);  

    const Mat4F rotMat = Mat4F::FromEuler(TargetState.Rotation);
    const Vec3F up = Vec3F::Up();
    const Vec3F right = rotMat.Right() * -1.0f;
    const Vec3F forward = rotMat.Forward();

    const float ScrollDelta = GetMouseWheelMove() * 0.1f;
    TargetState.MovementSpeed = CLAMP(TargetState.MovementSpeed + ScrollDelta * (50.0f + TargetState.MovementSpeed * 0.5f), 1.0f, 300.0f);
    
    // Add position
    const Vec3F posDelta =
        (up * (
            static_cast<float>(man.Action("Up", "EditorCamera").Down()) -
            static_cast<float>(man.Action("Down", "EditorCamera").Down())) +
        right * (
            static_cast<float>(man.Action("Right", "EditorCamera").Down()) -
            static_cast<float>(man.Action("Left", "EditorCamera").Down())) +
        forward * (
            static_cast<float>(man.Action("Forward", "EditorCamera").Down()) -
            static_cast<float>(man.Action("Backward", "EditorCamera").Down()))) * 
        static_cast<float>(!ctrl);
    TargetState.Position += Vec3F(posDelta.normalized) * static_cast<float>(InDelta) * TargetState.MovementSpeed;
    
    // FOV
    TargetState.FOV += (
            static_cast<float>(man.Action("IncreaseFOV", "EditorCamera").Down()) -
            static_cast<float>(man.Action("DecreaseFOV", "EditorCamera").Down())) *
        static_cast<float>(!ctrl) * 
        static_cast<float>(InDelta) * 30.0f;

    // TODO: Interp account for dt
    CurrentState.Rotation = TargetState.Rotation;
    CurrentState.Position = LERP(CurrentState.Position, TargetState.Position, 10.0f * (float)InDelta);
    CurrentState.FOV = LERP(CurrentState.FOV, TargetState.FOV, 10.0f * (float)InDelta);
    CurrentState.MovementSpeed = LERP(CurrentState.MovementSpeed, TargetState.MovementSpeed, 10.0f * (float)InDelta);
    
    // Set camera
    BlueprintEditor::Get().GetRenderScene().SetCamera({
        CurrentState.Position, QuatF::FromEuler(CurrentState.Rotation), CurrentState.FOV
    });
}

void EditorCamera::Enter(const CameraInstance& InCamera)
{
    TargetState.Position = InCamera.Position;
    TargetState.Rotation = InCamera.Rotation.Euler();
    TargetState.FOV = InCamera.FOV;
    CurrentState = TargetState;

    Input::Manager::Get().Push("Default");
    Input::Manager::Get().Push("EditorCamera");
}

void EditorCamera::Exit()
{
    Input::Manager::Get().Pop("EditorCamera");
    Input::Manager::Get().Pop("Default");
}

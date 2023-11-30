#include "EditorCamera.h"

#include "BlueprintEditor.h"
#include "Utility/Math/AngleConversion.h"

void EditorCamera::Update(double InDelta)
{
    // Calculate rotation
    if (IsControlling())
    {
        if (!IsCursorHidden())
        {
            auto pos = GetMousePosition();
            CursorPos = { pos.x, pos.y };
            HideCursor();
        }
        
        const auto mouseDelta = GetMouseDelta();
        SetMousePosition(GetScreenWidth() / 2, GetScreenHeight() / 2);
        TargetState.Rotation +=
            Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.005f *
            static_cast<float>(!IsKeyDown(KEY_LEFT_CONTROL));
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
            (up * (static_cast<float>(IsKeyDown(KEY_E)) - static_cast<float>(IsKeyDown(KEY_Q))) +
            right * (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) +
            forward * (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S)))) * 
            static_cast<float>(!IsKeyDown(KEY_LEFT_CONTROL));
        TargetState.Position += Vec3F(posDelta.normalized) * static_cast<float>(InDelta) * TargetState.MovementSpeed;
        
        // FOV
        TargetState.FOV +=
            (static_cast<float>(IsKeyDown(KEY_Z)) - static_cast<float>(IsKeyDown(KEY_X))) *
            static_cast<float>(!IsKeyDown(KEY_LEFT_CONTROL)) * 
            static_cast<float>(InDelta) * 30.0f;
    }
    else
    {
        if (IsCursorHidden())
        {
            SetMousePosition(
                static_cast<int>(CursorPos.x),
                static_cast<int>(CursorPos.y));
            ShowCursor();
        }
    }

    // TODO: Interp account for dt
    CurrentState.Rotation = TargetState.Rotation;
    CurrentState.Position = LERP(CurrentState.Position, TargetState.Position, 10.0f * (float)InDelta);
    CurrentState.FOV = LERP(CurrentState.FOV, TargetState.FOV, 10.0f * (float)InDelta);
    CurrentState.MovementSpeed = LERP(CurrentState.MovementSpeed, TargetState.MovementSpeed, 10.0f * (float)InDelta);
    
    // Set camera
    BlueprintEditor::Get().GetRenderScene().SetCamera({
        CurrentState.Position, QuatF(CurrentState.Rotation), CurrentState.FOV
    });
}

void EditorCamera::Deinit() const
{
    if (IsCursorHidden())
    {
        SetMousePosition(
            static_cast<int>(CursorPos.x),
            static_cast<int>(CursorPos.y));
        ShowCursor();
    }
}

bool EditorCamera::IsControlling() const
{
    return (IsMouseButtonDown(MOUSE_BUTTON_RIGHT) || !RequireHold) && IsWindowFocused();
}

bool EditorCamera::IsFullyControlling() const
{
    return !RequireHold && IsWindowFocused();
}

void EditorCamera::SetReference(const CameraInstance& InCamera)
{
    TargetState.Position = InCamera.Position;
    TargetState.Rotation = InCamera.Rotation.Euler();
    TargetState.FOV = InCamera.FOV;
    CurrentState = TargetState; 
}

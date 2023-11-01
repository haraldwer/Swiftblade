#include "EditorCamera.h"

#include "BlueprintEditor.h"

void EditorCamera::Update(double InDelta)
{
    // Calculate rotation
    if (IsMouseButtonDown(MOUSE_BUTTON_LEFT))
    {
        const auto mouseDelta = GetMouseDelta();
        TargetState.Rotation += Vec3F(mouseDelta.y, mouseDelta.x * -1.0f,  0.0f) * 0.01f;

        const Mat4F rotMat = Mat4F::FromEuler(TargetState.Rotation);
        const Vec3F up = Vec3F::Up();
        const Vec3F right = rotMat.Right() * -1.0f;
        const Vec3F forward = rotMat.Forward();

        // Add position
        const Vec3F posDelta =
            up * (static_cast<float>(IsKeyDown(KEY_E)) - static_cast<float>(IsKeyDown(KEY_Q))) +
            right * (static_cast<float>(IsKeyDown(KEY_D)) - static_cast<float>(IsKeyDown(KEY_A))) +
            forward * (static_cast<float>(IsKeyDown(KEY_W)) - static_cast<float>(IsKeyDown(KEY_S)));
        TargetState.Position += Vec3F(posDelta.normalized) * static_cast<float>(InDelta) * 100.0f;

        // FOV
        TargetState.FOV += (static_cast<float>(IsKeyDown(KEY_Z)) - static_cast<float>(IsKeyDown(KEY_X))) *
            static_cast<float>(InDelta) * 30.0f;
    }

    // TODO: Interp account for dt
    CurrentState.Rotation = TargetState.Rotation;
    CurrentState.Position = LERP(CurrentState.Position, TargetState.Position, 10.0f * (float)InDelta);
    CurrentState.FOV = LERP(CurrentState.FOV, TargetState.FOV, 10.0f * (float)InDelta);
    
    // Set camera
    BlueprintEditor::Get().GetRenderScene().SetCamera({
        CurrentState.Position, QuatF(CurrentState.Rotation), CurrentState.FOV
    });
}

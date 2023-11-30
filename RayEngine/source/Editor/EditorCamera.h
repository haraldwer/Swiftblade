#pragma once
#include "Engine/Rendering/Scene.h"

class EditorCamera
{
public:

    EditorCamera() = default;

    void Update(double InDelta);
    void Deinit() const;
    
    void SetRequireHold(bool InHold) { RequireHold = InHold; }
    void ToggleRequireHold() { RequireHold = !RequireHold; }
    bool IsControlling() const;
    bool IsFullyControlling() const;
    void SetReference(const CameraInstance& InCamera);
    
    Vec3F GetPosition() const { return TargetState.Position; }

private:
    
    struct State
    {
        float FOV = 90.0f;
        float MovementSpeed = 50.0f; 
        Vec3F Position = Vec3F(0.0f, 10.0f, -20.0f);
        Vec3F Rotation;
    };

    State TargetState;
    State CurrentState;
    bool RequireHold = true;
    Vec2F CursorPos; 
    
};

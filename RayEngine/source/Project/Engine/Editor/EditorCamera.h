#pragma once
#include "..\Rendering\RenderScene.h"

class EditorCamera
{
public:

    EditorCamera() = default;

    void Update(double InDelta);
    void Deinit();
    
    Vec3F GetPosition() const { return TargetState.Position; }
    bool IsControlling() const { return bUseEditCamera && bInputEnabled; }
    void SetAlwaysEnabled(bool InEnabled) { bAlwaysEnabled = InEnabled; }
    void Toggle();
    
private:

    void UpdateMovement(double InDelta); 
    
    struct State
    {
        float FOV = 100.0f;
        float MovementSpeed = 50.0f; 
        Vec3F Position = Vec3F(0.0f, 10.0f, -20.0f);
        Vec3F Rotation;
    };

    State TargetState;
    State CurrentState;
    Vec2F CursorPos;

    bool bUseEditCamera = false;
    bool bInputEnabled = false;
    bool bRequireHold = true;
    bool bAlwaysEnabled = false;
    
};

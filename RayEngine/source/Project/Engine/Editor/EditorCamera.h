#pragma once
#include "Engine/Rendering/Scene.h"

class EditorCamera
{
public:

    EditorCamera() = default;

    void Update(double InDelta);
    void Deinit();

    void Enter(const CameraInstance& InCamera);
    void Enter();
    void Exit();
    
    Vec3F GetPosition() const { return TargetState.Position; }

private:
    
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
    
};

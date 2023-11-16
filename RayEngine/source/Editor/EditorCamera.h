#pragma once

class EditorCamera
{
public:

    EditorCamera() = default;
    
    void Update(double InDelta);
    void SetRequireHold(bool InHold) { HoldRight = InHold; }
    void ToggleRequireHold() { HoldRight = !HoldRight; }
    bool IsControlling() const;

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
    bool HoldRight = true;
    Vec2F CursorPos; 
    
};

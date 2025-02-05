#pragma once

class EditorCamera
{
public:

    EditorCamera() = default;

    void Update();
    void Deinit();

    void SetTarget(const Vec3F& InPosition, float InDistance = 10.0f);
    Vec3F GetPosition() const { return TargetState.Position; }
    bool IsControlling() const { return bUseEditCamera && bInputEnabled; }
    void SetAlwaysEnabled(bool InEnabled) { bAlwaysEnabled = InEnabled; }
    void Toggle();
    
private:

    void UpdateMovement(); 
    
    struct State
    {
        float FOV = 100.0f;
        float MovementSpeed = 50.0f; 
        Vec3F Position = Vec3F(254.0f, 254.0f, -20.0f);
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

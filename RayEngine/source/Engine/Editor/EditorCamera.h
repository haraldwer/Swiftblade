#pragma once

class EditorCamera
{
public:

    EditorCamera() = default;

    void Update();
    void Deinit();

    void SetTarget(const Vec3F& InPosition, float InDistance = 10.0f);
    Vec3F GetPosition() const { return targetState.position; }
    bool IsControlling() const { return bUseEditCamera && bInputEnabled; }
    void SetAlwaysEnabled(const bool InEnabled) { bAlwaysEnabled = InEnabled; }
    void Toggle();
    
private:

    void UpdateMovement(); 
    
    struct State
    {
        float fov = 100.0f;
        float movementSpeed = 50.0f; 
        Vec3F position = Vec3F(254.0f, 254.0f, -20.0f);
        Vec3F rotation = {};
    };

    State targetState = {};
    State currentState = {};
    Vec2F cursorPos = {};

    bool bUseEditCamera = false;
    bool bInputEnabled = false;
    bool bRequireHold = true;
    bool bAlwaysEnabled = false;
    
};

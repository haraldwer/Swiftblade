#pragma once

class EditorCamera
{
public:

    void Update(double InDelta);

private:

    struct State
    {
        float FOV = 90.0f;
        float MovementSpeed = 100.0f; 
        Vec3F Position = Vec3F(0.0f, 10.0f, -20.0f);
        Vec3F Rotation;
    };

    State TargetState;
    State CurrentState;
    
    
};

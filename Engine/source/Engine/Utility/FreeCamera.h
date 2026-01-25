#pragma once

namespace Engine
{
    class FreeCamera
    {
    public:

        FreeCamera() = default;

        void Update();
        void Deinit();

        void SetState(const Vec3F &InPos, const Vec3F &InRot);
        void SetTarget(const Vec3F& InPosition, float InDistance = 7.0f);
        Vec3F GetPosition() const { return currentState.position; }
        Vec3F GetRotation() const { return currentState.rotation; };
        
        bool IsControlling() const { return bUseEditCamera && bInputEnabled; }
        void SetAlwaysEnabled(const bool InEnabled) { bAlwaysEnabled = InEnabled; }
        void Toggle();
        
        Vec3F GetMouseDirection() const;


        Vec3F ScreenToWorld(const Vec2F &InScreen) const;
        Vec3F ClipToWorld(Vec2F InClip) const;

    private:

        void UpdateMovement(); 
        
        struct State
        {
            float fov = 100.0f;
            float movementSpeed = 50.0f; 
            float near = 0.01f; 
            float far = 100.0f; 
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
}

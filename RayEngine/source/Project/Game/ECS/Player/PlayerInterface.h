#pragma once

#include "Engine/ECS/Entity.h"

class Animator;
class Weapon;

namespace ECS
{
    struct Transform;
    struct CameraComponent;
    struct Rigidbody;
    struct Collider;
    
    class PlayerInput;
    class Movement;
    class Player;
    class SysPlayer;
    class Manager;
    class PlayerCamera;
    
    class PlayerInterface
    {
    public:
        virtual ~PlayerInterface() = default;

    protected:
        
        // Base getters
        Player& GetPlayer() const;
        EntityID GetPlayerID() const;

        // Specific getters
        Transform& GetPlayerTransform() const;
        Transform& GetCameraTransform() const;
        Transform& GetColliderTransform() const;
        Movement& GetMovement() const;
        Animator& GetAnimator() const;
        PlayerInput& GetInput() const;
        Collider& GetCollider() const;
        Rigidbody& GetRB() const;
        PlayerCamera& GetPlayerCamera() const;
        CameraComponent& GetCamera() const;
        Weapon* GetSword() const;

        static double GetTime();
        
    };
}

#pragma once

#include "Engine/ECS/Entity.h"

namespace ECS
{
    struct Transform;
    struct Camera;
    struct Rigidbody;
    struct Collider;
    
    class PlayerInput;
    class Movement;
    class Player;
    class SysPlayer;
    class Manager;
    class PlayerCamera;
    class Weapon;
    class Animator;
    class AnimationPoser;
    
    class PlayerInterface
    {
    public:
        virtual ~PlayerInterface() = default;

    protected:
        
        // Base getters
        Player& GetPlayer() const;
        static EntityID GetPlayerID();

        // Specific getters
        Transform& GetPlayerTransform() const;
        Transform& GetLeftTransform() const;
        Transform& GetRightTransform() const;
        Transform& GetCameraTransform() const;
        Transform& GetColliderTransform() const;
        Movement& GetMovement() const;
        Animator& GetAnimator() const;
        AnimationPoser* GetWeaponPoser() const;
        AnimationPoser& GetHandPoser() const;
        PlayerInput& GetInput() const;
        Collider& GetCollider() const;
        Rigidbody& GetRB() const;
        PlayerCamera& GetPlayerCamera() const;
        Camera& GetCamera() const;
        Weapon* GetWeapon() const;

        static double GetTime();
        
    };
}

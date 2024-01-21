#pragma once

#include "Engine/ECS/Entity.h"

class Weapon;

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
    
    class PlayerInterface
    {
    protected:
        
        // Base getters
        Player& GetPlayer() const;
        EntityID GetPlayerID() const;

        // Specific getters
        Transform& GetPlayerTransform() const;
        Transform& GetCameraTransform() const;
        Transform& GetColliderTransform() const;
        Movement& GetMovement() const;
        PlayerInput& GetInput() const;
        Collider& GetCollider() const;
        Rigidbody& GetRB() const;
        PlayerCamera& GetPlayerCamera() const;
        Camera& GetCamera() const;
        Weapon* GetSword() const;

        static double GetTime();
        
    };
}

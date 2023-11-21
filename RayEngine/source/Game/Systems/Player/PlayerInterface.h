#pragma once

#include "Engine/ECS/Entity.h"

namespace ECS
{
    struct Transform;
    struct Camera;
    struct Rigidbody;
    struct Collider;
    
    class Input;
    class Movement;
    class Player;
    class SysPlayer;
    class Manager;
    
    class PlayerInterface
    {
    public:

        void SetPlayer(const EntityID InPlayer) { PlayerID = InPlayer; }
        
    protected:
        
        // Base getters
        Player& GetPlayer() const;

        // Specific getters
        Transform& GetPlayerTransform() const;
        Transform& GetCameraTransform() const;
        Transform& GetColliderTransform() const;
        Movement& GetMovement() const;
        Input& GetInput() const;
        Collider& GetCollider() const;
        Rigidbody& GetRB() const;
        Camera& GetCamera() const;

    private:
        
        EntityID PlayerID = InvalidID;
        
    };
}

#pragma once

#include "Engine/ECS/Entity.h"

class Sword;

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
    class PlayerCamera;
    
    class PlayerInterface
    {
    public:

        void SetPlayer(const EntityID InPlayer) { PlayerID = InPlayer; }
        
    protected:
        
        // Base getters
        Player& GetPlayer() const;
        EntityID GetPlayerID() const { return PlayerID; }

        // Specific getters
        Transform& GetPlayerTransform() const;
        Transform& GetCameraTransform() const;
        Transform& GetColliderTransform() const;
        Movement& GetMovement() const;
        Input& GetInput() const;
        Collider& GetCollider() const;
        Rigidbody& GetRB() const;
        PlayerCamera& GetPlayerCamera() const;
        Camera& GetCamera() const;
        Sword* GetSword() const;

        static double GetTime(); 
        
    private:
        
        EntityID PlayerID = InvalidID;
        
    };
}

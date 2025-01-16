#pragma once

#include "Engine/ECS/UniqueComponent.h"
#include "Utility/Time/Timer.h"

namespace ECS
{
    class PlayerInterface;

    class Player : public UniqueComponent<Player>
    {
    public:
        void Init() override;
        void Update() override;
        void OnBeginContact(const Physics::Contact& InContact) override;
        int GetPriority() const override { return 20; }

        EntityID GetColliderID() const { return ColliderID; }
        EntityID GetCameraID() const { return CameraID; }
        EntityID GetWeaponID() const { return WeaponID; }
        EntityID GetLeftID() const { return LeftID; }
        EntityID GetRightID() const { return RightID; }

        void PickupWeapon(EntityID InWeaponID);
        void Die();
        
        void TriggerSectionEnd();
        void TriggerGameEnd();
        void ActivateCheckpoint();

    private:
        EntityID ColliderID = InvalidID;
        EntityID CameraID = InvalidID;
        EntityID WeaponID = InvalidID;
        EntityID LeftID = InvalidID;
        EntityID RightID = InvalidID;

        Utility::Timer PlayTimer; 
    };
}

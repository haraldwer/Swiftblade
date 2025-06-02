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

        EntityID GetColliderID() const { return colliderID; }
        EntityID GetCameraID() const { return cameraID; }
        EntityID GetWeaponID() const { return weaponID; }
        EntityID GetLeftID() const { return leftID; }
        EntityID GetRightID() const { return rightID; }

        void PickupWeapon(EntityID InWeaponID);
        void Die() const;
        
        void TriggerSectionEnd() const;
        void TriggerGameEnd() const;
        void ActivateCheckpoint() const;

    private:
        EntityID colliderID = INVALID_ID;
        EntityID cameraID = INVALID_ID;
        EntityID weaponID = INVALID_ID;
        EntityID leftID = INVALID_ID;
        EntityID rightID = INVALID_ID;

        Utility::Timer playTimer; 
    };
}

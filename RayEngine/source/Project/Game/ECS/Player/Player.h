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
        void Update(double InDelta) override;
        void OnBeginContact(const Physics::Contact& InContact) override;
        int GetPriority() const override { return 20; }

        EntityID GetColliderID() const { return ColliderID; }
        EntityID GetCameraID() const { return CameraID; }
        EntityID GetSwordID() const { return SwordID; }

        void PickupSword(EntityID InSwordID);
        void Die();
        
        void TriggerSectionEnd();
        void TriggerGameEnd();
        void ActivateCheckpoint();

    private:
        EntityID ColliderID = InvalidID;
        EntityID CameraID = InvalidID;
        EntityID SwordID = InvalidID;

        Utility::Timer PlayTimer; 
    };
}

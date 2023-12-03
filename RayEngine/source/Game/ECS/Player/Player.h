#pragma once

#include "Engine/ECS/UniqueComponent.h"

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

        void Die(); 

    private:
        EntityID ColliderID = InvalidID;
        EntityID CameraID = InvalidID;
        EntityID SwordID = InvalidID;
    };
}

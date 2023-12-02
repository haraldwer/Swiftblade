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

        EntityID GetCameraID() const { return CameraID; }
        EntityID GetColliderID() const { return ColliderID; }

        void Die(); 

    private:
        EntityID ColliderID = InvalidID;
        EntityID CameraID = InvalidID;
    };
}

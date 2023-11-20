#pragma once

#include "Engine/ECS/UniqueComponent.h"

namespace ECS
{
    class PlayerInterface;

    class Player : public UniqueComponent<Player>
    {
    public:
        void Init() override;
        EntityID GetCameraID() const { return CameraID; }
        EntityID GetColliderID() const { return ColliderID; }

    private:
        EntityID ColliderID = InvalidID;
        EntityID CameraID = InvalidID;
    };
}

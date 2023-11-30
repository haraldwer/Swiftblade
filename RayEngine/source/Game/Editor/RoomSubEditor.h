#pragma once
#include "Engine/ECS/Entity.h"

namespace ECS
{
    struct CubeVolume;
    struct Transform;
}

class RoomSubEditor
{
public:
    void SetCubeVolume();
    
    ECS::EntityID GetVolumeID() const { return CubeVolume; }
    ECS::CubeVolume& GetVolume() const;

private:
    ECS::EntityID CubeVolume;
};

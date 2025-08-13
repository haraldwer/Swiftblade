#pragma once

#include "../RoomSubEditor.h"
#include "ECS/Volume/CubeVolumeCoord.h"

class RoomVolumeEditor : public RoomSubEditor
{
    TYPE_INFO(RoomVolumeEditor, RoomSubEditor);
    
public: 
    void Init() override;
    void Update() override;
    void DebugDraw() override;

    ECS::EntityID GetCubeVolumeID() const { return cubeVolume; } 

private:
    ECS::EntityID cubeVolume = ECS::INVALID_ID;
    uint32 placeStart = 0;
    bool placing = false;
    ECS::VolumeCoord lastTrace = 0;
};

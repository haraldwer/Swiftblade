#pragma once

#include "../RoomSubEditor.h"
#include "ECS/Volume/Coord.h"

class RoomVolumeEditor : public RoomSubEditor
{
public: 
    void Init() override;
    void Update() override;
    void DebugDraw() override;

private:

    ECS::EntityID cubeVolume = ECS::INVALID_ID;
    uint32 placeStart = 0;
    bool placing = false;
    Coord lastTrace = 0;
};

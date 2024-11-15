#pragma once

#include "../RoomSubEditor.h"
#include "ECS/Volume/Coord.h"

class RoomVolumeEditor : public RoomSubEditor
{
public: 
    void Init() override;
    void Update() override;
    void DebugDraw(bool InIsCameraControlling) override;

private:
    
    uint32 PlaceStart = 0;
    bool Placing = false;
    Coord LastTrace = 0;
};

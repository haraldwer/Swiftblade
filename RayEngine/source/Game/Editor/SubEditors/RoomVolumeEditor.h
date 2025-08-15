#pragma once

#include "../RoomSubEditor.h"
#include "ECS/Volume/CubeVolumeCoord.h"

namespace ECS {
    struct CubeVolumeData;
}

class RoomVolumeEditor : public RoomSubEditor
{
    TYPE_INFO(RoomVolumeEditor, RoomSubEditor);
    
public: 
    void Init() override;
    void Update() override;
    void Frame() override;
    void DebugDraw() override;
    void Enter() override;
    
    ECS::EntityID GetCubeVolumeID() const { return cubeVolume; } 

private:
    void SetData(const ECS::CubeVolumeData& InData) const;
    
    ECS::EntityID cubeVolume = ECS::INVALID_ID;
    
    ECS::VolumeCoord editStart = 0;
    ECS::VolumeCoord editEnd = 0;
};

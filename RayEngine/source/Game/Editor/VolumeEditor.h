#pragma once
#include "RoomSubEditor.h"
#include "Utility/History/History.h"

class VolumeEditor : public RoomSubEditor
{
public: 
    void Init() override;
    void Update(double InDelta) override;
    void UpdateUI(bool InIsCameraControlling) override;

private:
    
    static constexpr float PlaceDist = 5.0f;
    uint32 PlaceStart = 0;
    bool Placing = false;
};

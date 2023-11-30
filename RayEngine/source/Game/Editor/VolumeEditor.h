#pragma once
#include "RoomSubEditor.h"
#include "Utility/History/History.h"

class VolumeEditor : RoomSubEditor
{
public: 
    void Init();
    void Deinit() {}
    void Update(double InDelta);
    void UpdateUI() const;

private:
    Utility::History History;
    
    static constexpr float PlaceDist = 5.0f;
    uint32 PlaceStart = 0;
    bool Placing = false;
};

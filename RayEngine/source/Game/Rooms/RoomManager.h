#pragma once
#include "Engine/Scene/SceneResource.h"

class RoomManager
{
public:
    void Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset);
    void LoadConfig();
    void Unload();
    
private:
    Vector<SceneInstance> Scenes;
    
};

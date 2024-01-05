#pragma once
#include "Engine/Scene/Scene.h"

class RoomManager
{
public:
    void Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset);
    void LoadConfig();
    void Unload();
    
private:
    Vector<SceneInstance> Scenes;
    
};

#pragma once
#include "Engine/Scene/Scene.h"

class RoomManager
{
public:
    void Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset);
    void Unload();
    
private:
    Vector<SceneInstance> Scenes;
    
};

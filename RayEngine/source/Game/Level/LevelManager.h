#pragma once
#include "Engine/Scene/SceneResource.h"

struct LevelConfig;

class LevelManager
{
public:
    void Load(const Vector<ResScene>& InRooms, bool InApplyRootOffset);
    void LoadLevel(const LevelConfig& InLevel);
    void Unload();
    
private:
    Vector<SceneInstance> scenes = {};
};

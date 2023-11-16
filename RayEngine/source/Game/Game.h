#pragma once

#include "Engine/Instance/Instance.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Scene/Scene.h"

class Game : public Engine::Instance 
{
    
public:
    void Init() override;
    void Deinit() override;
    void Update(double InDelta) override;

private:

    Physics::Manager Physics;
    SceneInstance SceneInstance; 
    
};

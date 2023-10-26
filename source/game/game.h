#pragma once

#include "Utility/Singelton.h"
#include "Engine/ECS/Manager.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Scene.h"

class Game : public Utility::Singelton<Game>
{
    
public:

    void Init();
    void Update();
    void Deinit(); 

private:
    
    void FixedUpdate(double InDelta);

    ECS::Manager ECS;
    Physics::Manager Physics;
    Rendering::Scene RenderScene; 
    
};

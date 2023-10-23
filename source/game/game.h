#pragma once
#include "Engine/ECS/Manager.h"
#include "Engine/Rendering/Scene.h"
#include "Utility/Singelton.h"
#include "World/CubeVolume.h"

class Game : public Utility::Singelton<Game>
{
    
public:

    void Init();
    void Update();

    Rendering::LogicScene& GetRenderScene() { return RenderScene; }
    
private:
    
    void FixedUpdate(double InDelta);
    
    static constexpr double TickRate = 300.0; 
    double TickTimer = 0.0;

    Rendering::LogicScene RenderScene;
    ECS::Manager ECS;
    CubeVolume Room; 
};

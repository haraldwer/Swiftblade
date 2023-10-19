#pragma once
#include "Engine/ECS/Manager.h"
#include "Engine/Rendering/RenderScene.h"
#include "Utility/Singelton.h"
#include "World/CubeVolume.h"

class Game : public Singelton<Game>
{
    
public:

    void Init() override;
    void Update();
    
    RenderScene& GetRenderScene() { return RenderScene; }
    
private:
    
    void FixedUpdate(double InDelta);
    
    static constexpr double TickRate = 300.0; 
    double TickTimer = 0.0;

    RenderScene RenderScene;
    ECS::Manager ECS;
    CubeVolume Room; 
};

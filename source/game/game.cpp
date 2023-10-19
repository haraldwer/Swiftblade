#include "Game.h"

#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Mesh.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Rendering/Renderer.h"

void Game::Init()
{
    Singelton::Init();

    ECS.Init();
    Room.Init();

    auto& tSys = ECS.GetSystem<ECS::SysTransform>();
    auto& mSys = ECS.GetSystem<ECS::SysMesh>();
    auto& cSys = ECS.GetSystem<ECS::SysCamera>();

    const ECS::EntityID c = ECS.CreateEntity();
    tSys.Register(c);
    cSys.Register(c);
    
    //const EntityID e = ECS.CreateEntity();
    //tSys.Register(e);
    //mSys.Register(e);

}

void Game::Update()
{
    TickTimer += GetFrameTime();
    constexpr double delta = 1.0 / TickRate;
    while (TickTimer > delta)
    {
        TickTimer -= delta;
        FixedUpdate(delta);
    }
}

void Game::FixedUpdate(double InDelta)
{
    RenderScene = {};
    
    ECS.Update(InDelta);
    Room.Draw();

    auto& r = Renderer::Get(); 
    r.ClearScenes();
    r.PushScene(RenderScene);
}

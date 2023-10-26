#include "Game.h"

#include "raylib.h"
#include "Engine/Blueprints/Blueprint.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Mesh.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Renderer.h"

void Game::Init()
{
    ECS.Init();
    Physics.Init();

    if (BlueprintResource* bp = ResBlueprint("../content/test.json").Get())
        bp->Instantiate(); 
}

void Game::Deinit()
{
    ECS.Deinit();
    Physics.Deinit();
}

Rendering::LogicScene& Game::GetRenderScene()
{
    return *reinterpret_cast<Rendering::LogicScene*>(&RenderScene);
}

void Game::Update(double InDelta)
{
    // Reset render scene
    RenderScene = {};

    // Update
    Physics.Update(InDelta);
    ECS.Update(InDelta);

    // Push render scene
    auto& r = Rendering::Renderer::Get();
    r.Push(RenderScene);
}

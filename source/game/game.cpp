#include "Game.h"

#include "raylib.h"
#include "Engine/ECS/Systems/Camera.h"
#include "Engine/ECS/Systems/Mesh.h"
#include "Engine/ECS/Systems/Transform.h"
#include "Engine/Physics/Manager.h"
#include "Engine/Rendering/Renderer.h"

void Game::Init()
{
    ECS.Init();
    Physics.Init(); 
}

void Game::Deinit()
{
    
}

void Game::Update()
{
    
}

void Game::FixedUpdate(double InDelta)
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

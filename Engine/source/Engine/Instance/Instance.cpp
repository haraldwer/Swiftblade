#include "Instance.h"

#include "Rendering/Manager.h"

void Engine::Instance::Init()
{
    PROFILE();
    input.Init();
}

void Engine::Instance::Deinit()
{
    PROFILE();
    freeCamera.Deinit(); 
    menus.Clear();
}

void Engine::Instance::Tick(const double InDelta)
{
    PROFILE();
    time.Tick(InDelta);
    input.Update(); 
    menus.Update();
}

void Engine::Instance::Frame()
{
    PROFILE();
    input.Frame();
    Rendering::SceneRenderer::Get().Enqueue({ &renderScene });
}

#include "Instance.h"

#include "Rendering/Manager.h"

void Engine::Instance::Init()
{
    input.Init();
}

void Engine::Instance::Deinit()
{
    editorCamera.Deinit(); 
    renderScene = Rendering::Scene();
    menus.Clear();
}

void Engine::Instance::Logic(const double InDelta)
{
    renderScene.Clear();
    time.Tick(InDelta);
    input.Update(); 
    menus.Update();
}

void Engine::Instance::Frame()
{
    PROFILE();
    input.Frame();
    auto& man = Rendering::Manager::Get();
    renderScene.Build();
    man.Render(renderScene);
}

Rendering::Scene& Engine::Instance::GetRenderScene()
{
    return renderScene;
}

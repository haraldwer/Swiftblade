#include "Instance.h"

#include "Rendering/Manager.h"

void Engine::Instance::Init()
{
    Input.Init();
}

void Engine::Instance::Deinit()
{
    EditorCamera.Deinit(); 
    RenderScene = Rendering::Scene();
    Menus.Clear();
}

void Engine::Instance::Logic(double InDelta)
{
    RenderScene.Clear();
    Time.Tick(InDelta);
    Input.Update(); 
    Menus.Update();
}

void Engine::Instance::Frame()
{
    Input.Frame();
    auto& man = Rendering::Manager::Get();
    man.Render(RenderScene);
}

Rendering::Scene& Engine::Instance::GetRenderScene()
{
    return RenderScene;
}

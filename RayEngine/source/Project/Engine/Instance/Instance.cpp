#include "Instance.h"

#include "Engine/Rendering/Manager.h"

void Engine::Instance::Init()
{
    Input.Init();
}

void Engine::Instance::Deinit()
{
    RenderScene = Rendering::Scene();
    Menus.Clear();
}

void Engine::Instance::Logic(double InDelta)
{
    RenderScene = Rendering::Scene();
    Time.Tick(InDelta);
    Input.Update(); 
    Menus.Update(InDelta);
}

void Engine::Instance::Frame(double InDelta)
{
    Input.Frame();
    Rendering::Manager::Get().SubmitScene(RenderScene); 
    Menus.Draw();
}

Rendering::LogicScene& Engine::Instance::GetRenderScene()
{
    return *reinterpret_cast<Rendering::LogicScene*>(&RenderScene);
}

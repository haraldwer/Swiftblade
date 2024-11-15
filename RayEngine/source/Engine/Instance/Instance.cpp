#include "Instance.h"

#include "Rendering/Manager.h"

void Engine::Instance::Init()
{
    Input.Init();
}

void Engine::Instance::Deinit()
{
    EditorCamera.Deinit(); 
    RenderScene = Rendering::RenderScene();
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
    Rendering::Manager::Get().SubmitScene(RenderScene);
    Menus.Draw();
}

Rendering::RenderScene& Engine::Instance::GetRenderScene()
{
    return RenderScene;
}

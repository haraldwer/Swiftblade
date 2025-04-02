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
    man.MainViewport.BeginFrame();

    Rendering::RenderArgs args {
        .Scene= &RenderScene,
        .Context= &man.DefaultContext,
        .Viewport= &man.MainViewport,
        .Camera = RenderScene.GetCamera()
    };
    Rendering::Pipeline::Stats stats = man.DefaultPipeline.Render(args);
    man.FrameViewer.SetStats(stats);
    
    Menus.Draw();
    man.MainViewport.EndFrame();
}

Rendering::Scene& Engine::Instance::GetRenderScene()
{
    return RenderScene;
}

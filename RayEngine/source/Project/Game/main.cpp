
#include "Engine/EventScopes.h"
#include "Engine/Editor/Debugging/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Resource/Manager.h"
#include "Utility/Time/Time.h"

#include "Game/MenuInstance.h"
#include "Utility/File/File.h"

int main()
{
    Utility::SetWorkingDir();
    
    // These things are shared between instances
    Debug::Manager debugManager;
    Rendering::Renderer renderer;
    Resource::Manager resourceManager;
    Engine::Manager instanceManager;
    GlobalEventManager eventManager;
    
    debugManager.Init();
    renderer.Init();

    // Push game instance by default
    instanceManager.Push<MenuInstance>();

    // Timekeeping
    double tickTimer = 0.0;
    Utility::Timer timer;
    constexpr bool fixedUpdate = false;
    constexpr double tickRate = 300.0; 
    constexpr double fixedDelta = 1.0 / tickRate;
    constexpr int maxFrameIterations = 10;
    
    while (true)
    {
        resourceManager.Update();
        
        // Update instances
        if (IsKeyPressed(KEY_ESCAPE))
            instanceManager.Pop(); 
        instanceManager.Update();
        
        // Get instance
        const auto instance = instanceManager.Top();
        if (!instance)
            break;
        
        // Calculate delta
        const double delta = timer.Ellapsed();
        timer = Utility::Timer();

        // The update function
        const auto update = [&](double InDelta)
        {
            renderer.Clear();
            auto& rs = instance->GetRenderScene();
            rs = Rendering::LogicScene();
            instance->Update(InDelta);
            renderer.Push(rs); 
        };

        if (fixedUpdate)
        {
            tickTimer = MIN(tickTimer + delta, maxFrameIterations);
            while (tickTimer >= 0)
            {
                tickTimer -= fixedDelta;
                update(fixedDelta);
            }
        }
        else
        {
            update(delta);
        }

        if (renderer.ShouldClose())
            break;
        
        renderer.BeginVirtualFrame();
        renderer.RenderScenes(delta);
        instance->Draw();
        renderer.EndVirtualFrame();
        renderer.BeginFrame();
        debugManager.DrawDebugWindow();
        renderer.EndFrame(); 
    }
    
    debugManager.Deinit();
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit();
    
    return 0;
}

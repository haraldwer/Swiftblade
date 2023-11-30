
#include "Engine/Instance/Manager.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Resource/Manager.h"
#include "Utility/Time/Time.h"

#include "Editor/BlueprintEditor.h"
#include "Game/Menus/Menu.h"

int main()
{
    // Renderer and resource manager are shared between instances
    Rendering::Renderer renderer;
    Resource::Manager resourceManager;
    Engine::Manager instanceManager;
    renderer.Init();

    // Push game instance by default
    instanceManager.Push<Menu>();

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

        // Render
        if (!renderer.BeginRender())
            break; 
        renderer.RenderScenes(delta);
        instance->UpdateUI();
        renderer.EndRender();
    }
    
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit(); 
    
    return 0;
}

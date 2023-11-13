
#include "Engine/Instance/Manager.h"
#include "Engine/Rendering/Renderer.h"
#include "Engine/Resource/Manager.h"
#include "Utility/Time/Time.h"

#include "Editor/BlueprintEditor.h"
#include "Game/Game.h"
#include "Game/RoomEditor.h"

int main()
{
    // Renderer and resource manager are shared between instances
    Rendering::Renderer renderer;
    Resource::Manager resourceManager;
    Engine::Manager instanceManager;
    renderer.Init();

    // Push game instance by default
    instanceManager.Push<BlueprintEditor>();

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
        
        // Get instance
        const auto instance = instanceManager.Top();
        CHECK_BREAK(!instance);
        
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
        if (const auto lateInstance = instanceManager.Top())
            lateInstance->UpdateUI();
        renderer.EndRender();
    }
    
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit(); 
    
    return 0;
}

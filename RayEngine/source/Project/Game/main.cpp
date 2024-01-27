
#include "Engine/EventScopes.h"
#include "Engine/Editor/Debugging/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Rendering/Manager.h"
#include "Engine/Resource/Manager.h"
#include "Utility/Time/Time.h"

#include "Game/MenuInstance.h"
#include "Utility/File/File.h"

int main()
{
    Utility::SetWorkingDir();
    
    // These things are shared between instances
    Debug::Manager debugManager;
    Rendering::Manager renderer;
    Resource::Manager resourceManager;
    Engine::Manager instanceManager;
    GlobalEventManager eventManager;
    
    debugManager.Init();
    renderer.Init();

    // Push menu instance by default
    instanceManager.Push<MenuInstance>();

    // Timekeeping
    double tickTimer = 0.0;
    Utility::Timer timer;
    constexpr double tickRate = 300.0; 
    constexpr double fixedDelta = 1.0 / tickRate;
    constexpr double maxFrameTickTime = 10.0;
    
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
        
        // Update
        tickTimer = MIN(tickTimer + delta, maxFrameTickTime);
        while (tickTimer >= 0)
        {
            tickTimer -= fixedDelta;
            instance->Logic(fixedDelta);
            debugManager.Logic();
        }

        if (renderer.ShouldClose())
            break;

        // Render to target texture
        renderer.BeginVirtualFrame();
        instance->Frame(delta);
        renderer.EndVirtualFrame();

        // Render to screen
        renderer.BeginFrame();
        debugManager.Frame(delta);
        renderer.EndFrame(); 
    }
    
    debugManager.Deinit();
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit();
    
    return 0;
}

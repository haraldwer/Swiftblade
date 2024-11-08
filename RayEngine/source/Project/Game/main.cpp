
#include "Engine/EventScopes.h"
#include "Engine/Editor/Debugging/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Engine/Profiling/Manager.h"
#include "Engine/Profiling/Profile.h"
#include "Engine/Rendering/Manager.h"
#include "Engine/Resource/Manager.h"

#include "Game/MenuInstance.h"
#include "Utility/File/File.h"

int main()
{
    Utility::SetWorkingDir();
    
    // These things are shared between instances
    Debug::Manager debugManager;
    Profiling::Manager profilingManager;
    Resource::Manager resourceManager;
    Rendering::Manager renderer;
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
    constexpr double maxFrameTickTime = 0.5;
    
    while (true)
    {
        profilingManager.Frame();
        PROFILE_SCOPE_BEGIN("Update");
        
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
        tickTimer = Utility::Math::Min(tickTimer + delta, maxFrameTickTime);
        while (tickTimer >= 0)
        {
            PROFILE_SCOPE_BEGIN("Logic");
            tickTimer -= fixedDelta;
            instance->Logic(fixedDelta);
            debugManager.Logic();
            PROFILE_SCOPE_END();
        }

        if (renderer.ShouldClose())
            break;

        PROFILE_SCOPE_BEGIN("Rendering");

        // Render to target texture
        PROFILE_SCOPE_BEGIN("Virtual frame");
        renderer.BeginVirtualFrame();
        instance->Frame();
        renderer.EndVirtualFrame();
        PROFILE_SCOPE_END();

        // Render to screen
        renderer.BeginFrame();
        debugManager.Frame(delta);
        renderer.EndFrame();
        
        PROFILE_SCOPE_END();
        PROFILE_SCOPE_END();
    }
    
    debugManager.Deinit();
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit();
    
    return 0;
}

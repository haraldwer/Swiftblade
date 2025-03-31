
#include "raylib.h"

#include "Core/Debug/Manager.h"
#include "Core/Resource/Manager.h"
#include "Engine/Editor/Debug/Profiling/Manager.h"
#include "Engine/Editor/Debug/Profiling/Profile.h"
#include "Engine/Instance/Manager.h"
#include "Rendering/Manager.h"

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
    double logicTimeCounter = 0.0;
    Utility::Timer frameTimer;
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
        const double frameDelta = frameTimer.Ellapsed();
        frameTimer = Utility::Timer();
        
        // Update
        logicTimeCounter = Utility::Math::Min(logicTimeCounter + frameDelta, maxFrameTickTime);
        while (logicTimeCounter >= 0)
        {
            PROFILE_SCOPE_BEGIN("Logic");
            logicTimeCounter -= fixedDelta;
            instance->Logic(fixedDelta);
            debugManager.Logic();
            PROFILE_SCOPE_END();
        }

        if (renderer.Window.ShouldClose())
            break;

        PROFILE_SCOPE_BEGIN("Rendering");

        // Render to target texture
        PROFILE_SCOPE_BEGIN("Virtual frame");
        instance->Frame();
        PROFILE_SCOPE_END();

        // Render to screen
        PROFILE_SCOPE_BEGIN("Frame");
        renderer.BeginFrame();
        debugManager.Frame(frameDelta);
        renderer.EndFrame();
        PROFILE_SCOPE_END();
        
        PROFILE_SCOPE_END();
        PROFILE_SCOPE_END();
    }
    
    debugManager.Deinit();
    instanceManager.Clear(); 
    renderer.Deinit();
    resourceManager.Deinit();
    
    return 0;
}

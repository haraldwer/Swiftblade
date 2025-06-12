
#include "raylib.h"

#include "Core/Debug/Manager.h"
#include "Core/Resource/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Rendering/Manager.h"
#include "Engine/Audio/Manager.h"

#include "Game/MenuInstance.h"
#include "Utility/File/File.h"

int main()
{
    Utility::SetWorkingDir();

    // These things are shared between instances
    Debug::Manager debugManager;
    Resource::Manager resourceManager;
    Rendering::Manager renderer;
    Audio::Manager audio;
    Engine::Manager instanceManager;
    GlobalEventManager eventManager;
    
    debugManager.Init();
    renderer.Init();
    audio.Init();

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
        PROFILE();
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
            PROFILE_NAMED("Tick");
            logicTimeCounter -= fixedDelta;
            instance->Logic(fixedDelta);
            debugManager.Logic();
        }

        if (renderer.window.ShouldClose())
            break;
        
        audio.Update();
        
        // Render to target texture
        instance->GetRenderScene().Clear();
        instance->Frame();

        // Render to screen
        renderer.BeginFrame();
        debugManager.Frame(frameDelta);
        renderer.EndFrame();

        PROFILE_FRAME();
    }
    
    debugManager.Deinit();
    instanceManager.Clear();
    audio.Deinit();
    renderer.Deinit();
    resourceManager.Deinit();
    
    return 0;
}


#ifdef __EMSCRIPTEN__
    #include <emscripten/emscripten.h>
#endif
#include "raylib.h"

void Init();
void Deinit();
void Tick();
bool tick = true;

int main()
{
    LOG("Initializing!")
    Init();
    LOG("Finished init!")
    
#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop(Tick, 0, 1);
#else
    while (tick)
        Tick();
#endif

    LOG("Deinitializing!")
    Deinit();
    LOG("Exiting main loop")
    return 0;
}

#include "Core/Debug/Manager.h"
#include "Core/Resource/Manager.h"
#include "Database/Manager.h"
#include "Engine/Audio/Manager.h"
#include "Engine/Instance/Manager.h"
#include "Instance/Launcher.h"
#include "Rendering/Manager.h"

#include "Instances/MenuInstance.h"

constexpr double tickRate = 300.0; 
constexpr double fixedDelta = 1.0 / tickRate;
constexpr double maxFrameTickTime = 0.5;

struct GameData
{
    Debug::Manager debugManager;
    Resource::Manager resourceManager;
    Rendering::Manager renderer;
    Audio::Manager audio;
    Engine::Manager instanceManager;
    Engine::Launcher launcher;
    GlobalEventManager eventManager;
    DB::Manager db = {};
    
    double logicTimeCounter = 0.0;
    Utility::Timer frameTimer;
};
GameData* g = nullptr;

Texture tex2;

void Init()
{
    g = new GameData();
    
    Utility::SetWorkingDir();

    tex2 = LoadTexture("Defaults/T_Missing.png");
    if (IsTextureValid(tex2))
        LOG("Loaded Defaults/T_Missing.png");
        
    return;
    
    g->renderer.Init();
    g->debugManager.Init();
    g->audio.Init();
    g->launcher.Init();
    g->db.Init();
}

void Deinit()
{
    return;
    g->db.Deinit();
    g->launcher.Deinit(); 
    g->debugManager.Deinit();
    g->instanceManager.Clear();
    g->audio.Deinit();
    g->renderer.Deinit();
    g->resourceManager.Deinit();
}

void Tick()
{
    BeginDrawing();
    DrawTexture(tex2, 20, 20, WHITE);
    EndDrawing();
    
    return;
    
    PROFILE();
    g->resourceManager.Update();
    g->db.Update();
        
    // Update instances
    g->instanceManager.Update();
        
    // Get instance
    const auto instance = g->instanceManager.Top();
    if (!instance)
    {
        tick = false;
        return;
    }
        
    // Calculate delta
    const double frameDelta = g->frameTimer.Ellapsed();
    g->frameTimer = Utility::Timer();
        
    // Update
    g->logicTimeCounter = Utility::Math::Min(g->logicTimeCounter + frameDelta, maxFrameTickTime);
    while (g->logicTimeCounter >= 0)
    {
        PROFILE_NAMED("Tick");
        g->logicTimeCounter -= fixedDelta;
        instance->Logic(fixedDelta);
        g->debugManager.Logic();
    }

    if (Rendering::Window::ShouldClose())
    {
        tick = false;
        return;
    }

    g->audio.Update();

    // Render to target texture
    instance->GetRenderScene().Clear();
    instance->Frame();
        
    // Render to screen
    g->renderer.BeginFrame();
    g->debugManager.Frame(frameDelta);
    g->renderer.EndFrame();
    PROFILE_FRAME();
}
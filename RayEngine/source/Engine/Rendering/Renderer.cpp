#include "Renderer.h"

#include "ImGui/rlImGui.h"

using namespace Rendering;

void Renderer::Init()
{
    CurrConfig.Load();
    
    InitWindow(
        CurrConfig.Width,
        CurrConfig.Height,
        "RayEngine");
    
    ApplyConfig(CurrConfig); 

    rlImGuiSetup(false); 
}

void Renderer::Deinit()
{
    // TODO: Cleanup rendering
    rlImGuiShutdown();
    CloseWindow();
}

bool Renderer::BeginRender()
{
    if (WindowShouldClose())
        return false;
    BeginDrawing();
    ClearBackground(SKYBLUE);
    rlImGuiBegin();
    return true; 
}

void Renderer::RenderScenes(double InDelta)
{
    // TODO: Interpolate render instances
    for (auto s : Scenes)
        reinterpret_cast<RenderScene*>(&s)->Render();
}

void Renderer::EndRender()
{
    rlImGuiEnd();
    DrawFPS(10, 10);
    EndDrawing();
}

void Renderer::Clear()
{
    Scenes.clear();
}

void Renderer::Push(const Scene& InScene)
{
    Scenes.push_back(InScene);
}

void Renderer::ApplyConfig(const Config& InConfig)
{
    CurrConfig = InConfig;
    SetTargetFPS(InConfig.TargetFPS);
    SetWindowSize(InConfig.Width, InConfig.Height);
    if (InConfig.Fullscreen != IsWindowFullscreen())
        ToggleFullscreen();

    unsigned flags = 0;
    if (InConfig.Fullscreen)
        flags |= FLAG_FULLSCREEN_MODE;
    if (InConfig.VSync)
        flags |= FLAG_VSYNC_HINT;
    if (InConfig.MSAA)
        flags |= FLAG_MSAA_4X_HINT;
    flags |= FLAG_WINDOW_ALWAYS_RUN;
    SetWindowState(flags);
    
    LOG("Render config applied");
    CurrConfig.Save();
}

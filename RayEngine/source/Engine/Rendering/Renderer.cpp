#include "Renderer.h"

#include "ImGui/rlImGui.h"

using namespace Rendering;

void Renderer::Init()
{
    CurrentSettings.Load(GetPath());
    
    InitWindow(
        CurrentSettings.Width,
        CurrentSettings.Height,
        "RayEngine");
    
    ApplySettings(CurrentSettings); 

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

void Renderer::ApplySettings(const Settings& InSettings)
{
    CurrentSettings = InSettings;
    SetTargetFPS(InSettings.TargetFPS);
    SetWindowSize(InSettings.Width, InSettings.Height);
    if (InSettings.Fullscreen != IsWindowFullscreen())
        ToggleFullscreen();

    unsigned flags = 0;
    if (InSettings.Fullscreen)
        flags |= FLAG_FULLSCREEN_MODE;
    if (InSettings.VSync)
        flags |= FLAG_VSYNC_HINT;
    if (InSettings.MSAA)
        flags |= FLAG_MSAA_4X_HINT;
    flags |= FLAG_WINDOW_ALWAYS_RUN;
    SetWindowState(flags);
    
    LOG("Render settings applied");
    CurrentSettings.Save(GetPath());
}

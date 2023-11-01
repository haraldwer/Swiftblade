#include "renderer.h"

using namespace Rendering;

void Renderer::Init()
{
    InitWindow(1280, 720, "rayengine");

    Settings s;
    if (s.Load("renderSettings.json"))
        ApplySettings(s); 
}

void Renderer::Deinit()
{
    // TODO: Cleanup rendering
    CloseWindow();
}

bool Renderer::BeginRender()
{
    if (WindowShouldClose())
        return false;
    BeginDrawing();
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
    LOG("Render settings applied");
    CurrentSettings.Save("renderSettings.json");
}

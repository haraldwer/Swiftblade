#include "renderer.h"

#include "raylib.h"

using namespace Rendering;

void Renderer::Init()
{
    // TODO: Load config
    constexpr int ScreenWidth  = 800;
    constexpr int ScreenHeight = 450;

    // Apply render settings
    InitWindow(50, 50, "rayengine");
    SetTargetFPS(60);
    SetWindowSize(ScreenWidth, ScreenHeight);
}

void Renderer::Render(double InDelta)
{
    // TODO: Interpolate render instances
    
    // Render world
    BeginDrawing();
    for (auto s : Scenes)
        reinterpret_cast<RenderScene*>(&s)->Render();
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

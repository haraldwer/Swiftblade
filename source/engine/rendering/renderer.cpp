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

void Renderer::Render()
{
    BeginDrawing();
    for (const Scene& scene : Scenes)
        static_cast<RenderScene>(scene).Render();
    DrawFPS(10, 10);
    EndDrawing();
}

void Renderer::ClearScenes()
{
    Scenes.clear();
}

void Renderer::PushScene(const LogicScene& InScene)
{
    Scenes.push_back(InScene);
}


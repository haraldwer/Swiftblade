#include "renderer.h"

#include "raylib.h"

void Renderer::Init()
{
    Singelton::Init();

    // Apply render settings
    InitWindow(50, 50, "rayengine");
    SetTargetFPS(60);
    SetWindowSize(ScreenWidth, ScreenHeight);
}

void Renderer::Render()
{
    BeginDrawing();
    for (RenderScene& scene : Scenes)
        scene.Render();
    DrawFPS(10, 10);
    EndDrawing();
}

void Renderer::ClearScenes()
{
    Scenes.clear();
}

void Renderer::PushScene(const RenderScene& InScene)
{
    Scenes.push_back(InScene);
}


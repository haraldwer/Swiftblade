#include "Renderer.h"

#include "ImGui/rlImGui.h"
#include "..\..\ImGui\imgui_themes.h"
#include "ImGui/Gizmo/ImGuizmo.h"

using namespace Rendering;

void Renderer::Init()
{
    CurrConfig.LoadConfig();
    ApplyConfig(CurrConfig); 

    rlImGuiSetup(false);
    ImGui::ThemeMoonlight();
    ImGui::LoadFont();

    SetExitKey(KEY_F4);
}

void Renderer::Deinit()
{
    // TODO: Cleanup rendering
    UnloadRenderTexture(VirtualTarget);
    rlImGuiShutdown();
    CloseWindow();
}

bool Renderer::ShouldClose()
{
    return WindowShouldClose();
}

void Renderer::BeginVirtualFrame(const RenderTexture2D* InTarget)
{
    BeginTextureMode(InTarget ? *InTarget : VirtualTarget);
    ClearBackground(DARKGRAY);
}

void Renderer::RenderScenes(double InDelta)
{
    // TODO: Interpolate render instances
    for (auto s : Scenes)
        reinterpret_cast<RenderScene*>(&s)->Render();
}

void Renderer::EndVirtualFrame()
{
    
    EndTextureMode();
}

void Renderer::BeginFrame()
{
    BeginDrawing(); 
    ClearBackground(DARKGRAY);
    
    // Blip
    const Vec2F windowSize = GetWindowSize();
    const float virtualRatio = windowSize.x / windowSize.y;
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(VirtualTarget.texture.width),
        -static_cast<float>(VirtualTarget.texture.height)
    };
    const Rectangle destRec = {
        -virtualRatio,
        -virtualRatio,
        windowSize.x + (virtualRatio * 2),
        windowSize.y + (virtualRatio * 2)
    };
    DrawTexturePro(
        VirtualTarget.texture,
        sourceRec,
        destRec,
        { 0.0, 0.0 },
        0.0,
        WHITE);
    
    rlImGuiBegin();
    ImGui::PushDefaultFont();
    ImGuizmo::BeginFrame();
}

void Renderer::EndFrame()
{
    ImGui::PopDefaultFont(); 
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
    
    unsigned flags = 0;
    if (InConfig.Fullscreen)
        flags |= FLAG_FULLSCREEN_MODE;
    if (InConfig.VSync)
        flags |= FLAG_VSYNC_HINT;
    if (InConfig.MSAA)
        flags |= FLAG_MSAA_4X_HINT;
    flags |= FLAG_WINDOW_ALWAYS_RUN;
    SetWindowState(flags);

    // Create window
    if (IsWindowReady())
        CloseWindow();
    InitWindow(
        CurrConfig.Width,
        CurrConfig.Height,
        "RayEngine");
    
    SetTargetFPS(InConfig.TargetFPS);

    // Recreate render target
    UnloadRenderTexture(VirtualTarget);

    // The height is already set
    // Calculate the width based on aspect
    const float aspect = static_cast<float>(CurrConfig.Width) / static_cast<float>(CurrConfig.Height);
    const int virtualWidth = CurrConfig.RenderSize ?
        static_cast<int>(static_cast<float>(CurrConfig.RenderSize) * aspect) : CurrConfig.Width; 
    const int virtualHeight = CurrConfig.RenderSize ? CurrConfig.RenderSize : CurrConfig.Height;
    VirtualTarget = LoadRenderTexture( virtualWidth, virtualHeight);
    
    LOG("Render config applied");
    CurrConfig.SaveConfig();
}

#include "Renderer.h"

#include "RenderEventData.h"
#include "Engine/EventScopes.h"
#include "Engine/Editor/Debugging/Manager.h"
#include "ImGui/rlImGui.h"
#include "ImGui\imgui_themes.h"
#include "Engine/Editor/Gizmo/ImGuizmo.h"
#include "Utility/Events/Event.h"

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

void Renderer::DrawDebugWindow()
{
    // Adjust size
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const ImVec2 size = { vMax.x - vMin.x, vMax.y - vMin.y };
    if (static_cast<int>(size.x) != VirtualTarget.texture.width ||
        static_cast<int>(size.y) != VirtualTarget.texture.height)
        CreateVirtualTarget(static_cast<int>(size.x), static_cast<int>(size.y)); 
    
    rlImGuiImageRenderTexture(&VirtualTarget);
}

void Renderer::BeginFrame()
{
    BeginDrawing(); 

    // Blip if not debug drawing
    const auto& debugMan = Debug::Manager::Get();
    if (debugMan.IsOpen(DebugWindowName()) && debugMan.Enabled())
    {
        ClearBackground(Color(13, 14, 15, 255));
    }
    else
    {
        ClearBackground(DARKGRAY);
        const Vec2F windowSize = GetWindowSize();

        // Adjust size
        if (static_cast<int>(windowSize.x) != VirtualTarget.texture.width ||
            static_cast<int>(windowSize.y) != VirtualTarget.texture.height)
            CreateVirtualTarget(
                static_cast<int>(windowSize.x),
                static_cast<int>(windowSize.y)); 

        // Flip and blip
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

        if (!Debug::Manager::Get().Enabled())
            DrawFPS(10, 10); 
    }
    
    rlImGuiBegin();
    ImGui::PushDefaultFont();
    ImGuizmo::BeginFrame();
}

void Renderer::EndFrame()
{
    ImGui::PopDefaultFont(); 
    rlImGuiEnd();
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
    CreateVirtualTarget(CurrConfig.Width, CurrConfig.Height);
    
    LOG("Render config applied");
    CurrConfig.SaveConfig();
}

void Renderer::CreateVirtualTarget(const int InUnscaledWidth, const int InUnscaledHeight)
{
    OnCreateVirtualTargetData data;
    data.PreviousHeight = VirtualTarget.texture.height; 
    data.PreviousWidth = VirtualTarget.texture.width; 
    
    UnloadRenderTexture(VirtualTarget);
    
    // The height is already set
    // Calculate the width based on aspect
    const float aspect = static_cast<float>(InUnscaledWidth) / static_cast<float>(InUnscaledHeight);
    const int virtualWidth = CurrConfig.RenderSize ?
        static_cast<int>(static_cast<float>(CurrConfig.RenderSize) * aspect) : InUnscaledWidth; 
    const int virtualHeight = CurrConfig.RenderSize ? CurrConfig.RenderSize : InUnscaledHeight;
    VirtualTarget = LoadRenderTexture( virtualWidth, virtualHeight);

    data.NewHeight = VirtualTarget.texture.height; 
    data.NewWidth = VirtualTarget.texture.width;
    GlobalEvent<OnCreateVirtualTargetData> event;
    event.Invoke(data);
}

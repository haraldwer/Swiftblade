#include "Manager.h"

#include "Editor/Debugging/Manager.h"

#include <thread>

#include "Editor/Gizmo/ImGuizmo.h"
#include "EventScopes.h"
#include "ImGui/imgui_themes.h"
#include "ImGui/rlImGui.h"
#include "RenderEvents.h"

void Rendering::Manager::Init()
{
    CurrConfig.LoadConfig();
    ApplyConfig(CurrConfig); 

    rlImGuiSetup(false);
    ImGui::ThemeMoonlight();
    ImGui::LoadFont();

    SetExitKey(KEY_F4);

    Pipeline.Init(); 
}

void Rendering::Manager::Deinit()
{
    Pipeline = Rendering::Pipeline();
    rlImGuiShutdown();
    CloseWindow();
}

bool Rendering::Manager::ShouldClose()
{
    return WindowShouldClose();
}

void Rendering::Manager::BeginVirtualFrame()
{
    BeginTextureMode(VirtualTarget);
    ClearBackground(BLACK);
}

void Rendering::Manager::EndVirtualFrame()
{
    EndTextureMode();
}

void Rendering::Manager::DrawDebugWindow()
{
    // Adjust size
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };
    if ((size - ViewportSize).LengthSqr() > 1.0f)
        SetViewportSize(
            static_cast<int>(size.x),
            static_cast<int>(size.y));

    // Send to ImGui
    rlImGuiImageRect(
        &VirtualTarget.texture,
        static_cast<int>(size.x),
        static_cast<int>(size.y),
        Rectangle{ 0,0,
            RenderResolution.width,
            -RenderResolution.height
        });

    // Calculate viewport pos
    const ImVec2 windowPos = ImGui::GetWindowPos();
    ViewportPosition = { vMin.x + windowPos.x, vMin.y + windowPos.y };
}

void Rendering::Manager::SubmitScene(RenderScene& InScene)
{
    InScene.BeginFrame();
    Pipeline.Render(InScene, VirtualTarget);
}

void Rendering::Manager::BeginFrame()
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
        ViewportPosition = Vec2F::Zero(); 

        // Adjust size
        if ((windowSize - ViewportSize).LengthSqr() > 1.0f)
            SetViewportSize(
                static_cast<int>(windowSize.x),
                static_cast<int>(windowSize.y)); 

        // Flip and blip
        const float virtualRatio = windowSize.x / windowSize.y;
        const Rectangle sourceRec = {
            0.0f, 0.0f,
            RenderResolution.width,
            -RenderResolution.height
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

void Rendering::Manager::EndFrame()
{
    ImGui::PopDefaultFont(); 
    rlImGuiEnd();
    EndDrawing();
    CapFPS();
}

void Rendering::Manager::ApplyConfig(const Config& InConfig)
{
    auto prev = CurrConfig;
    CurrConfig = InConfig;

    // Create window
    if (!IsWindowReady() ||
        prev.Height != CurrConfig.Height ||
        prev.Width != CurrConfig.Width ||
        prev.Fullscreen != CurrConfig.Fullscreen)
    {
        if (IsWindowReady())
            CloseWindow();
        InitWindow(
            CurrConfig.Width,
            CurrConfig.Height,
            "RayEngine");
    }
    
    unsigned flags = 0;
    if (InConfig.Fullscreen)
        flags |= FLAG_BORDERLESS_WINDOWED_MODE;
    if (InConfig.VSync)
        flags |= FLAG_VSYNC_HINT;
    if (InConfig.MSAA)
        flags |= FLAG_MSAA_4X_HINT;
    flags |= FLAG_WINDOW_ALWAYS_RUN;
    SetWindowState(flags);

    // Recreate render target
    SetViewportSize(CurrConfig.Width, CurrConfig.Height);
    
    LOG("Render config applied");
    CurrConfig.SaveConfig();
}

Vec2F Rendering::Manager::GetWindowSize() const
{
    return {
        static_cast<float>(CurrConfig.Width),
        static_cast<float>(CurrConfig.Height)
    };
}

Vec2F Rendering::Manager::GetViewportSize() const
{
    return ViewportSize; 
}

Vec2F Rendering::Manager::GetViewportPosition() const
{
    return ViewportPosition; 
}

Vec2F Rendering::Manager::GetResolution() const
{
    return {
        static_cast<float>(RenderResolution.width),
        static_cast<float>(RenderResolution.height)
    };
}

void Rendering::Manager::SetViewportSize(const int InUnscaledWidth, const int InUnscaledHeight)
{
    OnSetViewportSize data;
    data.PreviousRenderResolution = RenderResolution;
    data.PreviousViewportSize = ViewportSize;
    
    // The height is already set
    // Calculate the width based on aspect
    ViewportSize = {
        static_cast<float>(InUnscaledWidth),
        static_cast<float>(InUnscaledHeight)
    };
    
    const float aspect = static_cast<float>(InUnscaledWidth) / static_cast<float>(InUnscaledHeight);
    const int virtualWidth = CurrConfig.RenderSize ?
        static_cast<int>(static_cast<float>(CurrConfig.RenderSize) * aspect) : InUnscaledWidth; 
    const int virtualHeight = CurrConfig.RenderSize ? CurrConfig.RenderSize : InUnscaledHeight;
    RenderResolution = {
        static_cast<float>(virtualWidth),
        static_cast<float>(virtualHeight)
    };

    // Setup the renderer with the new resolution
    UnloadRenderTexture(VirtualTarget);
    VirtualTarget = LoadRenderTexture( virtualWidth, virtualHeight);

    data.NewRenderResolution = RenderResolution;
    data.NewViewportSize = ViewportSize;
    GlobalEvent<OnSetViewportSize> event;
    event.Invoke(data);
}

void Rendering::Manager::CapFPS()
{
    int targetFPS = CurrConfig.TargetFPS.Get();
    if (targetFPS > 0)
    {
        targetFPS = Utility::Math::Max(targetFPS, 10);
        double desiredFrameTime = (1.0 / targetFPS) - LeftoverFrameTime;
        while (FrameTimer.Ellapsed() < desiredFrameTime) {}
        LeftoverFrameTime = FrameTimer.Ellapsed() - desiredFrameTime;
        FrameTimer = {};
    }
}

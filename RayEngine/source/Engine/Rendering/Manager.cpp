#include "Manager.h"

#include "Core/Debug/Manager.h"
#include "Menu/Manager.h"

#include "raylib.h"
#include "rlgl.h"
#include "ImGui/Gizmo/ImGuizmo.h"
#include "ImGui/imgui_themes.h"
#include "ImGui/rlImGui.h"
#include "Menu/Menu.h"
#include "Rendering/Scene/Scene.h"

void Rendering::Manager::Init()
{
    CurrConfig.LoadConfig();
    QueuedConfig = CurrConfig;
    Window.Open(CurrConfig.Window);
    MainViewport.Init(CurrConfig.Viewport);
    DefaultContext.Init(CurrConfig.Context, CurrConfig.Lumin, true);

    rlImGuiSetup(false);
    ImGui::ThemeMoonlight();
    ImGui::LoadFont();
}

void Rendering::Manager::Deinit()
{
    rlImGuiShutdown();
    MainViewport.Deinit();
    Window.Close();
}

void Rendering::Manager::Render(const Scene& InScene)
{
    RenderArgs args {
        .Scene= &InScene,
        .Context= &DefaultContext,
        .Viewport= &MainViewport,
        .Camera = InScene.GetCamera()
    };
    MainViewport.BeginFrame();
    
    Pipeline::Stats stats = DefaultPipeline.Render(args);
    FrameViewer.SetStats(stats);

    BeginTextureMode(MainViewport.GetVirtualTarget());
    rlEnableColorBlend();
    Menu::Manager::Get().Draw();
    EndTextureMode();
}

void Rendering::Manager::DrawDebugWindow()
{
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2I size = {
        static_cast<int>(vMax.x - vMin.x),
        static_cast<int>(vMax.y - vMin.y)
    };
    MainViewport.Resize(size);
    MainViewport.ImDraw();
}

void Rendering::Manager::BeginFrame()
{
    BeginDrawing();
    
    // Blip if not debug drawing
    const auto& debugMan = Debug::Manager::Get();
    if (debugMan.IsOpen(DebugWindowName()) && debugMan.Enabled())
    {
        rlClearScreenBuffers();
    }
    else
    {
        rlClearScreenBuffers();
        MainViewport.ResetPosition();
        MainViewport.Resize(Window.GetSize());
        Window.Draw(MainViewport.GetVirtualTarget().texture);
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
    EndDrawing(); // Swap buffers
    
    if (CurrConfig != QueuedConfig)
        ApplyConfig(QueuedConfig);
    Window.CapFPS();
}

void Rendering::Manager::QueueConfig(const Config& InConfig)
{
    QueuedConfig = InConfig;
}

void Rendering::Manager::ApplyConfig(const Config& InConfig)
{
    auto prev = CurrConfig;
    CurrConfig = InConfig;
    QueuedConfig = InConfig;

    if (CurrConfig.Context != prev.Context || CurrConfig.Lumin != prev.Lumin)
        DefaultContext.Init(CurrConfig.Context, CurrConfig.Lumin, true);

    if (CurrConfig.Viewport != prev.Viewport)
        MainViewport.Init(CurrConfig.Viewport);

    if (CurrConfig.Window != prev.Window)
        Window.Open(CurrConfig.Window);
    
    LOG("Render config applied");
    CurrConfig.SaveConfig();
}

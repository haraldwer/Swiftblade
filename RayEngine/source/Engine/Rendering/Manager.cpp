#include "Manager.h"

#include "Core/Debug/Manager.h"

#include "ImGui/Gizmo/ImGuizmo.h"
#include "ImGui/imgui_themes.h"
#include "ImGui/rlImGui.h"

void Rendering::Manager::Init()
{
    CurrConfig.LoadConfig();
    QueuedConfig = CurrConfig;
    Window.Open(CurrConfig.Window);
    MainViewport.Init(CurrConfig.Viewport);
    DefaultContext.Init(CurrConfig.Context, true);

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
        ClearBackground(Color(13, 14, 15, 255));
    }
    else
    {
        ClearBackground(::DARKGRAY);
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
    EndDrawing();
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

    if (CurrConfig.Context != prev.Context)
        DefaultContext.Init(CurrConfig.Context);

    if (CurrConfig.Viewport != prev.Viewport)
        MainViewport.Init(CurrConfig.Viewport);

    if (CurrConfig.Window != prev.Window)
        Window.Open(CurrConfig.Window);
    
    LOG("Render config applied");
    CurrConfig.SaveConfig();
}

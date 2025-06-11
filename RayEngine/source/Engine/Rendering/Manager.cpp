#include "Manager.h"

#include "raylib.h"
#include "rlgl.h"

#include "ImGui/Gizmo/ImGuizmo.h"
#include "ImGui/imgui_themes.h"
#include "ImGui/rlImGui.h"

#include "Core/Debug/Manager.h"
#include "Menu/Manager.h"

#include "Rendering/GLProfile.h"
#include "Rendering/Scene/Scene.h"
#include "Rendering/State/State.h"

void Rendering::Manager::Init()
{
    currConfig.LoadConfig();
    queuedConfig = currConfig;
    
    window.Open(currConfig.Window);
    PROFILE_GL_INIT();
    
    mainViewport.Init(currConfig.Viewport);
    defaultContext.Init(currConfig.Context, currConfig.Lumin, true);

    rlImGuiSetup(false);
    ImGui::Theme1();
    ImGui::LoadFont();
}

void Rendering::Manager::Deinit()
{
    rlImGuiShutdown();
    mainViewport.Deinit();
    window.Close();
}

void Rendering::Manager::Render(const Scene& InScene)
{
    PROFILE_GL();
    const RenderArgs args {
        .scenePtr= &InScene,
        .contextPtr= &defaultContext,
        .viewportPtr= &mainViewport,
        .perspectives = {{
            .referenceRect= Vec4F(),
            .targetRect= Vec4I(),
            .camera= InScene.GetCamera()
        }}
    };
    mainViewport.BeginFrame();

    rlDrawRenderBatchActive();
    rlState::current.Reset();
    const Pipeline::Stats stats = defaultPipeline.Render(args);
    frameViewer.SetStats(stats);
    rlState::current.Reset();

    BeginTextureMode(mainViewport.GetVirtualTarget());
    rlEnableColorBlend();
    Menu::Manager::Get().Draw();
    EndTextureMode();
}

void Rendering::Manager::DrawDebugPanel()
{
    PROFILE_GL();
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2I size = {
        static_cast<int>(vMax.x - vMin.x),
        static_cast<int>(vMax.y - vMin.y)
    };
    if (size.x > 0 && size.y > 0)
    {
        mainViewport.Resize(size);
        mainViewport.ImDraw();
    }
}

void Rendering::Manager::BeginFrame()
{
    PROFILE_GL();
    BeginDrawing();
    
    // Blip if not debug drawing
    const auto& debugMan = Debug::Manager::Get();
    if (debugMan.IsOpen(DebugPanelName()) && debugMan.Enabled())
    {
        rlClearScreenBuffers();
    }
    else
    {
        rlClearScreenBuffers();
        mainViewport.ResetPosition();
        mainViewport.Resize(window.GetSize());
        window.Draw(mainViewport.GetVirtualTarget().texture);
        if (!Debug::Manager::Get().Enabled())
            DrawFPS(10, 10);
    }

    rlImGuiBegin();
    ImGui::PushDefaultFont();
    ImGuizmo::BeginFrame();
}

void Rendering::Manager::EndFrame()
{
    PROFILE_GL();
    ImGui::PopDefaultFont();
    rlImGuiEnd();
    EndDrawing(); // Swap buffers
    PROFILE_GL_COLLECT();
    
    if (currConfig != queuedConfig)
        ApplyConfig(queuedConfig);
    window.CapFPS();
}

void Rendering::Manager::QueueConfig(const Config& InConfig)
{
    queuedConfig = InConfig;
}

void Rendering::Manager::ApplyConfig(const Config& InConfig)
{
    const auto prev = currConfig;
    currConfig = InConfig;
    queuedConfig = InConfig;

    if (currConfig.Context != prev.Context || currConfig.Lumin != prev.Lumin)
        defaultContext.Init(currConfig.Context, currConfig.Lumin, true);

    if (currConfig.Viewport != prev.Viewport)
        mainViewport.Init(currConfig.Viewport);

    if (currConfig.Window != prev.Window)
        window.Open(currConfig.Window);
    
    LOG("Render config applied");
    currConfig.SaveConfig();
}

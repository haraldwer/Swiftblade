#include "Manager.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

#ifdef IMGUI_ENABLE
#include "ImGui/imgui.h"
#include "ImGui/Gizmo/ImGuizmo.h"
#include "ImGui/imgui_themes.h"
#endif

#include "Core/Debug/Manager.h"
#include "Menu/Manager.h"

#include "Instance/Instance.h"
#include "Rendering/GLProfile.h"
#include "Rendering/Scene/Scene.h"
#include "Rendering/State/State.h"

void Rendering::Manager::Init()
{
    currConfig.LoadConfig();
    queuedConfig = currConfig;
    
    window.Open(currConfig.Window);
    
    mainViewport.Init(currConfig.Viewport, currConfig.Context.Get().FX);
    defaultContext.Init(currConfig.Context, true);
}

void Rendering::Manager::Deinit()
{
    defaultContext.Deinit();
    mainViewport.Deinit();
    window.Close();
}

void Rendering::Manager::Render(const Scene& InScene)
{
    PROFILE_GL();
    
    if (currConfig.UpdateCulling)
    {
        auto res = mainViewport.GetResolution().To<float>();
        auto corners = InScene.GetCamera().GetFrustumCorners(res);
        cullPoints = { corners.begin(), corners.end() };
    }
    
    const RenderArgs args {
        .scenePtr= &InScene,
        .contextPtr= &defaultContext,
        .viewportPtr= &mainViewport,
        .perspectives = {{
            .referenceRect= Vec4F(),
            .targetRect= Vec4F(),
            .camera= InScene.GetCamera()
        }},
        .cullPoints = cullPoints,
        .cullMask = static_cast<uint8>(VisibilityMask::DEFAULT)
    };
    Pipeline::Stats stats;
        
    mainViewport.BeginFrame();
    
    rlState::current.Reset();
    stats += Pipeline::Render(args);
    stats += Pipeline::RenderCustom(args, [&]
    {
        if (currConfig.DrawUI)
            Menu::Manager::Get().Draw();
    });
    stats += Pipeline::RenderPost(args);
    frameViewer.SetStats(stats);
}

#ifdef IMGUI_ENABLE

bool Rendering::Manager::PanelBegin(bool &InOutOpen)
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.f, 0.f));
    ImGuiWindowFlags flags = ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoCollapse;
    //if (hovered)
    //    flags |= ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove;
    return ImGui::Begin((PanelName() + "##Panel").c_str(), &InOutOpen, flags);
}

void Rendering::Manager::PanelEnd()
{
    ImGui::End();
    ImGui::PopStyleVar();
}

void Rendering::Manager::DrawPanel()
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
        frameViewer.DrawViewportPanel();
    }
    
    const ImVec2 windowPos = ImGui::GetWindowPos();
    mainViewport.SetPosition({
        vMin.x + windowPos.x,
        vMin.y + windowPos.y
    });

    
    bool imguizmo = ImGuizmo::IsOver() && !Engine::Instance::Get().GetEditorCamera().IsControlling();
    hovered = ImGui::IsMouseHoveringRect(vMin + windowPos, vMax + windowPos) && !imguizmo;
    //ImGui::IsAnyItemHovered()
    //ImGui::IsWindowHovered(ImGuiHoveredFlags_AnyWindow)
    //ImGui::IsPopupOpen(nullptr, ImGuiPopupFlags_AnyPopup)
}

bool Rendering::Manager::IsViewportClickable() const
{
    // TODO: Focus     return (!IsPanelOpen() || hovered) && IsWindowFocused();
}

#else

void Rendering::Manager::DrawDebugPanel() {  }
bool Rendering::Manager::IsViewportClickable() const { return IsWindowFocused(); }

#endif

void Rendering::Manager::BeginFrame()
{
    PROFILE_GL();
    /*
    BeginDrawing();
    rlEnableColorBlend();
    BeginBlendMode(BLEND_ALPHA);
    
    // Blip if not debug drawing
    const auto& debugMan = Debug::Manager::Get();
    if (debugMan.IsOpen(PanelName()))
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

#ifdef IMGUI_ENABLE
    {
        PROFILE_GL_NAMED("ImGui");
        rlImGuiBegin();
        ImGui::PushDefaultFont();
        ImGuizmo::BeginFrame();
    }
#endif
    */
}

void Rendering::Manager::EndFrame()
{
    PROFILE_GL();
    /*

#ifdef IMGUI_ENABLE
    ImGui::PopDefaultFont();
    {
        PROFILE_GL_NAMED("ImGui End");
        rlImGuiEnd();
    }
#endif
    
    {
        PROFILE_GL_NAMED("EndDrawing (SwapBuffers)");
        EndBlendMode();
        EndDrawing();
    }
    PROFILE_GL_COLLECT();

    if (currConfig == queuedConfig)
        AutoResize();
    if (currConfig != queuedConfig)
        ApplyConfig(queuedConfig);
    */
    window.CapFPS();
}

void Rendering::Manager::QueueConfig(const Config& InConfig)
{
    queuedConfig = InConfig;
}

void Rendering::Manager::ApplyConfig(const Config& InConfig)
{
    PROFILE_GL();
    
    const auto prev = currConfig;
    currConfig = InConfig;
    queuedConfig = InConfig;

    if (currConfig.Context != prev.Context)
        defaultContext.Init(currConfig.Context, true);

    if (currConfig.Viewport != prev.Viewport || currConfig.Context.Get().FX != prev.Context.Get().FX)
        mainViewport.Init(currConfig.Viewport, currConfig.Context.Get().FX);

    if (currConfig.Window != prev.Window)
        window.Open(currConfig.Window);
    
    LOG("Render config applied");
    currConfig.SaveConfig();
}

void Rendering::Manager::AutoResize()
{
    PROFILE_GL();
    
    const auto& debugMan = Debug::Manager::Get();
    auto& win = queuedConfig.Window.Get();

    /*
#ifdef __EMSCRIPTEN__

    // Auto-resize based on browser
    int fullscreen = 0;
    emscripten_get_canvas_size(&win.Width.Get(), &win.Height.Get(), &fullscreen);

#else

    win.Fullscreen = IsWindowFullscreen();
    win.Width = GetScreenWidth();
    win.Height = GetScreenHeight();
    
#endif
    */
    
    if (debugMan.IsOpen(PanelName()))
        return;

    auto& view = queuedConfig.Viewport.Get();
    view.Width = win.Width.Get();
    view.Height = win.Height.Get();
}

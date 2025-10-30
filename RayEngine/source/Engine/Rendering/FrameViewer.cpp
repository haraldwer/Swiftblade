#include "FrameViewer.h"

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Manager.h"
#include "raylib.h"
#include "rlgl.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"

void Rendering::FrameViewer::DrawViewportPanel()
{
    auto& man = Manager::Get();

    struct NamedTex
    {
        String name;
        Texture* ptr;
    };
    Vector<NamedTex> views;
    for (auto& buff : man.mainViewport.GetTargets().GetNamed())
        for (const auto& tex : buff.second)
            views.insert(views.begin(), { buff.first + " | " + tex.name, tex.tex });

    if (IsKeyPressed(KEY_F5))
        debugView--;
    if (IsKeyPressed(KEY_F6))
        debugView++;

    if (debugView < 0)
        debugView = static_cast<int>(views.size()) - 1;
    if (debugView >= static_cast<int>(views.size()))
        debugView = 0;
    
    if (debugView == 0)
    {
        if (Texture* frame = man.mainViewport.GetFrameTexture())
            DrawViewTex(*frame);
    }
    else
    {
        DrawViewTex(*views[debugView].ptr);
        ImGui::SetCursorPos(ImGui::GetWindowContentRegionMin() + ImVec2( 10, 10 ));
        ImGui::Text(views[debugView].name.c_str());
    }

    auto c = man.GetConfig();
    if (c.ShowLights)
    {
        if (ImGui::Begin("Light debug", &c.ShowLights.Get()))
        {
            if (const auto l = man.defaultContext.GetLights())
                for (auto& tex : l->GetShadowTarget().GetTextures())
                    DrawTex(*tex.tex);
        }
        ImGui::End();
    }

    if (c.ShowLumin)
    {
        if (ImGui::Begin("Lumin debug", &c.ShowLumin.Get()))
        {
            ImGui::SeparatorText("Frame");
            if (const auto l = man.defaultContext.GetLumin())
                for (auto& tex : l->GetFrameTarget().GetTextures())
                    DrawTex(*tex.tex);
            ImGui::SeparatorText("Probes");
            if (const auto l = man.defaultContext.GetLumin())
                for (auto& tex : l->GetProbeTarget().GetTextures())
                    DrawTex(*tex.tex);
        }
        ImGui::End();
    }

    if (c != man.GetConfig())
        man.QueueConfig(c);
}

void Rendering::FrameViewer::DrawTex(const Texture& tex)
{
    // Adjust size
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
    const float mul = static_cast<float>(tex.width) / size.width;

    // Send to ImGui
    rlImGuiImageRect(
        &tex,
        static_cast<int>(size.x),
        static_cast<int>(static_cast<float>(tex.height) / mul),
        Rectangle{
            0,
            -static_cast<float>(tex.height),
            static_cast<float>(tex.width),
            -static_cast<float>(tex.height)
        });
}

void Rendering::FrameViewer::DrawViewTex(const Texture &tex)
{
    const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
    const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
    const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };

    rlImGuiImageRect(
        &tex,
        static_cast<int>(size.x),
        static_cast<int>(size.y),
        Rectangle{
            0,
            -static_cast<float>(tex.height),
            static_cast<float>(tex.width),
            -static_cast<float>(tex.height)
        });
}

void Rendering::FrameViewer::DrawDebugPanel()
{
    if (ImGui::CollapsingHeader("Stats"))
    {
        int total = 0;
        for (auto& entry : stats.meshDrawCount)
            total += entry.second;
        ImGui::Text("Meshes: %i", total);
        ImGui::Text("Deferred passes: %i", stats.deferredDrawCount);
        ImGui::Text("Debug shapes: %i", stats.debugDrawCount);
        ImGui::Text("Renders: %i", stats.renders);
        ImGui::Text("Fullscreen passes: %i", stats.fullscreenPasses);
        ImGui::Text("Probes: %i", stats.probes);
        ImGui::Text("Lights: %i", stats.lights);
        ImGui::Text("Skyboxes: %i", stats.skyboxes);
        ImGui::Text("Particle simulations: %i", stats.particleSims);
        ImGui::Text("Particles: %i", stats.particles);
    }
    
    auto& man = Manager::Get();
    auto c = man.GetConfig();
    if (c.Edit())
        man.QueueConfig(c);
}

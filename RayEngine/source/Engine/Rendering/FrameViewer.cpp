#include "FrameViewer.h"

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Manager.h"
#include "Rendering/Lights/Lights.h"
#include "Rendering/Lumin/Lumin.h"
#include "raylib.h"

void Rendering::FrameViewer::DrawDebugPanel()
{
    auto& man = Manager::Get();
    auto c = man.GetConfig();
    if (c.Edit())
        man.QueueConfig(c);

    int total = 0;
    for (auto& entry : stats.meshDrawCount)
        total += entry.second;
    ImGui::Text("Meshes: %i", total);
    ImGui::Text("Lights: %i", stats.lights);
    ImGui::Text("Probes: %i", stats.probes);
    ImGui::Text("Renders: %i", stats.renders);
    ImGui::Text("Skyboxes: %i", stats.skyboxes);
    ImGui::Text("Deferred passes: %i", stats.deferredDrawCount);
    ImGui::Text("Fullscreen passes: %i", stats.fullscreenPasses);
    ImGui::Text("Debug shapes: %i", stats.debugDrawCount);

    if (ImGui::CollapsingHeader("Scene Targets"))
    {
        for (auto& buff : man.mainViewport.GetTargets().GetNamed())
        {
            for (const auto& tex : buff.second)
            {
                CHECK_ASSERT(!tex.tex, "Tex nullptr")
                
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.tex->width),
                        static_cast<float>(-tex.tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }

    if (ImGui::CollapsingHeader("Shadow Target"))
    {
        if (const auto l = man.defaultContext.GetLights())
        {
            for (auto& tex : l->GetShadowTarget().GetTextures())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.tex->width),
                        static_cast<float>(-tex.tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }

    if (ImGui::CollapsingHeader("Lumin Target"))
    {
        if (const auto l = man.defaultContext.GetLumin())
        {
            for (auto& tex : l->GetProbeTarget().GetTextures())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.tex->width),
                        static_cast<float>(-tex.tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }
}

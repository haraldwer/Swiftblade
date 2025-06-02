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
    for (auto& entry : Stats.MeshDrawCount)
        total += entry.second;
    ImGui::Text("Meshes: %i", total);
    ImGui::Text("Lights: %i", Stats.Lights);
    ImGui::Text("Probes: %i", Stats.Probes);
    ImGui::Text("Renders: %i", Stats.Renders);
    ImGui::Text("Skyboxes: %i", Stats.Skyboxes);
    ImGui::Text("Deferred passes: %i", Stats.DeferredDrawCount);
    ImGui::Text("Fullscreen passes: %i", Stats.FullscreenPasses);
    ImGui::Text("Debug shapes: %i", Stats.DebugDrawCount);

    if (ImGui::CollapsingHeader("Scene Targets"))
    {
        for (auto& buff : man.MainViewport.GetTargets().GetNamed())
        {
            for (auto& tex : buff.second)
            {
                CHECK_ASSERT(!tex.Tex, "Tex nullptr")
                
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.Tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.Tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.Tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.Tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.Tex->width),
                        static_cast<float>(-tex.Tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }

    if (ImGui::CollapsingHeader("Shadow Target"))
    {
        if (auto l = man.DefaultContext.GetLights())
        {
            for (auto& tex : l->GetShadowTarget().GetTextures())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.Tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.Tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.Tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.Tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.Tex->width),
                        static_cast<float>(-tex.Tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }

    if (ImGui::CollapsingHeader("Lumin Target"))
    {
        if (auto l = man.DefaultContext.GetLumin())
        {
            for (auto& tex : l->GetProbeTarget().GetTextures())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(tex.Tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*tex.Tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    tex.Tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(tex.Tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(tex.Tex->width),
                        static_cast<float>(-tex.Tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }
}

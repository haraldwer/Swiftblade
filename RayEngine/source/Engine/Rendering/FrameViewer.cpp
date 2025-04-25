#include "FrameViewer.h"

#include "Manager.h"
#include "raylib.h"
#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Rendering/Lights/Lights.h"

void Rendering::FrameViewer::DrawDebugWindow()
{
    auto& man = Manager::Get();
    auto c = man.GetConfig();
    if (c.Edit())
        man.QueueConfig(c);

    int total = 0;
    for (auto& entry : Stats.MeshDrawCount)
        total += entry.second;
    ImGui::Text(("Meshes: " + std::to_string(total)).c_str());
    ImGui::Text(("Lights: " + std::to_string(Stats.Lights)).c_str());
    ImGui::Text(("Probes: " + std::to_string(Stats.Probes)).c_str());
    ImGui::Text(("Renders: " + std::to_string(Stats.Renders)).c_str());
    ImGui::Text(("Skyboxes: " + std::to_string(Stats.Skyboxes)).c_str());
    ImGui::Text(("Deferred passes: " + std::to_string(Stats.DeferredDrawCount)).c_str());
    ImGui::Text(("Fullscreen passes: " + std::to_string(Stats.FullscreenPasses)).c_str());
    ImGui::Text(("Debug shapes: " + std::to_string(Stats.DebugDrawCount)).c_str());

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
            for (auto& target : l->GetShadowTarget().All())
            {
                for (auto& tex : target.GetTextures())
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
}

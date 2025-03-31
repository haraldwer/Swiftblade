#include "FrameViewer.h"

#include "Manager.h"
#include "raylib.h"
#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"

void Rendering::FrameViewer::DrawDebugWindow()
{
    auto& man = Rendering::Manager::Get();
    auto c = man.GetConfig();
    if (c.Edit())
        man.QueueConfig(c);

    ImGui::Text("Draw count");
    int total = 0;
    for (auto& entry : Stats.MeshDrawCount)
    {
        total += entry.second;
        ImGui::Text((std::to_string(entry.first) + ": " + std::to_string(entry.second)).c_str());
    }
    ImGui::Text(("Total: " + std::to_string(total)).c_str());
    ImGui::Text(("Debug shapes: " + std::to_string(Stats.DebugDrawCount)).c_str());
        
    auto buffers = man.MainViewport.GetBuffers();
    for (auto& buff : buffers)
    {
        if (ImGui::CollapsingHeader(buff.first.c_str()))
        {
            for (auto& buff : buff.second)
            {
                CHECK_ASSERT(!buff.Tex, "Tex nullptr")
                
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(buff.Tex->width) / size.width;

                // Send to ImGui
                SetTextureWrap(*buff.Tex, TEXTURE_WRAP_REPEAT);
                rlImGuiImageRect(
                    buff.Tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(buff.Tex->height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(buff.Tex->width),
                        static_cast<float>(-buff.Tex->height)
                    });
                ImGui::TableNextColumn();
            }
        }
    }
}

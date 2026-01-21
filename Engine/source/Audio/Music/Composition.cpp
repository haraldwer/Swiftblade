#include "Composition.h"

#include "ImGui/imgui.h"

void Music::Composition::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    for (auto& track : Tracks.Get())
    {
        // TODO: Offset frame with track start
        track.Fill(InFrame, InSamples, InFrames, InSampleRate);
    }

    Effects.Get().Fill(InFrame, InSamples, InFrames, InSampleRate);
}


bool Music::Composition::Edit(const String &InName, uint32 InOffset)
{
#ifdef IMGUI_ENABLE

    bool result = false;
    if (ImGui::BeginTable("Comp", 2, ImGuiTableFlags_BordersInner))
    {
        ImGui::TableSetupColumn("Track", ImGuiTableColumnFlags_WidthFixed, 200);
        
        ImGui::TableNextRow();
        ImGui::TableSetColumnIndex(0);
        if (ImGui::Button("Add track"))
        {
            Tracks.Get().emplace_back();
            result = true;
        }

        ImGui::TableSetColumnIndex(1);
        // Draw timeline time points
        ImGui::Text("0");

        ImGui::TableNextRow();
        
        ImGui::PushStyleVar(ImGuiStyleVar_CellPadding, ImVec2(0, 0));
        int off = 0;
        for (auto& track : Tracks.Get())
        {
            result |= track.Edit(InName, InOffset + off);
            off++;
            ImGui::TableNextRow();
        }
        ImGui::PopStyleVar();
        
        ImGui::EndTable();
    }
        
    return result;
    
#else
    return false;
#endif
}


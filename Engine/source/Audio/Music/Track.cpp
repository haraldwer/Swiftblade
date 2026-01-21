#include "Track.h"

#include "ImGui/imgui.h"
#include "ImGui/imgui_stdlib.h"

void Music::Track::Add(const Note& InNote)
{
    
}

void Music::Track::Fill(uint64 InFrame, float* InSamples, uint32 InFrames, uint32 InSampleRate) const
{
    Effects.Get().Fill(InFrame, InSamples, InFrames, InSampleRate);
}

#ifdef IMGUI_ENABLE

bool Music::Track::Edit(const String &InName, uint32 InOffset)
{
    String off = "##" + InName + std::to_string(InOffset);
    
    ImGui::TableSetColumnIndex(0);

    ImGui::Spacing();
    Vector<String> tones = { String("Bop 1"), String("Bop 2") };
    static int curr = 0;
    if (ImGui::Combo(off.c_str(), &curr, tones))
        Tone = tones[curr];
    
    ImGui::SameLine();
    ImGui::Button(("fx" + off).c_str()); // Also number of active effects
    
    ImGui::TableSetColumnIndex(1);
    
    // Should be dynamic based on content
    constexpr int columns = 10;
    constexpr int rows = 5; 
    if (ImGui::BeginTable(("Track" + off).c_str(), columns, ImGuiTableFlags_Borders))
    {
        static bool selected[columns][rows] = {};
        
        for (int row = 0; row < rows; row++)
        {
            ImGui::TableNextRow();
            for (int column = 0; column < columns; column++)
            {
                char buf[32];
                sprintf(buf, "%d:%d", column, row);
                ImGui::TableSetColumnIndex(column);
                ImGui::Selectable(buf, &selected[column][row]);
            }
        }
        ImGui::EndTable();
    }
    return false;
}

#else

bool Music::Track::Edit(const String &InName, uint32 InOffset) { return false; }

#endif
#include "Manager.h"

#include "ImGui/Knobs/imgui-knobs.h"

void Audio::Manager::Init()
{
    config.LoadConfig();
    generator.Init(config);
}

void Audio::Manager::Deinit()
{
    generator.Deinit();
}

void Audio::Manager::Update()
{
}

void Audio::Manager::DrawDebugPanel()
{
    PROFILE();
    
    // Edit everything
    if (ImGui::Begin("Tones"))
    {
        workingGenData.Tones.Edit();
        ImGui::End();
    }
    workingGenData.Comp.Edit();

    generator.genData.Front() = workingGenData;
    generator.genData.SwapFront();
    
    // Display the output waves
    if (ImGui::Begin("Audio Out"))
    {
        ImVec2 plotSize = ImVec2(0, 50); 
        const AudioData& audioData = generator.audioData.SwapBack();
        ImGui::SetNextItemWidth(-FLT_MIN);
        ImGui::PlotLines("##AudioOut", audioData.Data.data(), static_cast<int>(audioData.Data.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
        
        if (ImGuiKnobs::Knob("Master", &config.Master.Get(), 0.0f, 1.0f))
            generator.Init(config);
        
        ImGui::End();
    }
}

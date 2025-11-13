#include "Manager.h"

#ifdef IMGUI_ENABLE
#include "ImGui/Knobs/imgui-knobs.h"
#endif

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


#ifdef IMGUI_ENABLE

void Audio::Manager::DrawPanel()
{
    PROFILE();
    
    workingGenData.Tones.Edit();
    
    if (ImGui::Begin("Composition"))
        workingGenData.Comp.Edit();
    ImGui::End();

    generator.genData.Front() = workingGenData;
    generator.genData.SwapFront();
    
    if (ImGuiKnobs::Knob("Master", &config.Master.Get(), 0.0f, 1.0f, 0, "%.3f", ImGuiKnobVariant_Tick, 50.0f, ImGuiKnobFlags_NoTitle))
        generator.Init(config);
    
    // Display the output waves
    const AudioData& audioData = generator.audioData.SwapBack();
    ImVec2 plotSize = ImVec2(0, 50); 
    ImGui::SameLine();
    ImGui::SetNextItemWidth(-FLT_MIN);
    ImGui::PlotLines("##AudioOut", audioData.Data.data(), static_cast<int>(audioData.Data.size()), 0, 0, FLT_MAX, FLT_MAX, plotSize);
}

#else

void Audio::Manager::DrawDebugPanel() { }

#endif

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
    
    const AudioData& audioData = generator.audioData.SwapBack();

    // Display the output waves
    
    ImGui::PlotLines("Audio", audioData.Data.data(), static_cast<int>(audioData.Data.size()));


    GenData& genData = generator.genData.Front();

    // Edit everything
    generator.genData.SwapFront();
    
    if (ImGuiKnobs::Knob("Master", &config.Master.Get(), 0.0f, 0.01f))
        generator.Init(config);
}

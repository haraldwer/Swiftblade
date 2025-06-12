#include "Manager.h"

void Audio::Manager::Init()
{
    generator.Init();
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
    auto& feedback = generator.feedback.SwapBack();
    // Show feedback data...

    ImGui::Text("DJ HarryBoyy in da houuuse");
    
    auto& conf = generator.config.Front();
    conf.Edit();
    generator.config.SwapFront();
}

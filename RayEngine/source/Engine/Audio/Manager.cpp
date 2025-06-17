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

    // Display the output waves
    
    auto& conf = generator.config.Front();

    // Edit everything
    
    generator.config.SwapFront();
}

#pragma once
#include "Generator.h"
#include "Debug/Panel.h"

namespace Audio
{
    class Manager : public Debug::Panel
    {
    public:
        void Init();
        void Deinit();
        void Update();
        
        String DebugPanelName() const override { return "Audio"; }
        void DrawDebugPanel() override;

    private:
        Generator generator;
        Config config;
    };
}

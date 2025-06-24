#pragma once
#include "Debug/Panel.h"
#include "Generator.h"

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
        GenData workingGenData;
    };
}

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
        
        String PanelName() const override { return "Audio"; }
        void DrawPanel() override;

    private:
        Generator generator;
        Config config;
        GenData workingGenData;
    };
}

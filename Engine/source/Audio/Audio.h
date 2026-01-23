#pragma once
#include "Core/Debug/Panel.h"
#include "Audio/Music/Generator.h"

namespace Audio
{
    class Audio : public Debug::Panel
    {
    public:
        void Init();
        void Deinit();
        void Update();
        
        String PanelName() const override { return "Audio"; }
        void DrawPanel() override;

    private:
        Music::Generator generator;
        Music::Config config;
        Music::GenData workingGenData;
    };
}

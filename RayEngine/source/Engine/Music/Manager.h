#pragma once
#include "Debug/Panel.h"

namespace Music
{
    class Manager : public Debug::Panel
    {
    public:
        void Init();
        
        String DebugPanelName() const override { return "Music"; }
        void DrawDebugPanel() override;
    private:
        
    };
}

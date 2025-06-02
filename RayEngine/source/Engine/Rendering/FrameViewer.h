#pragma once
#include "Core/Debug/Panel.h"
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    class FrameViewer : public Debug::Panel
    {
    public:
        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Rendering"; }
        void SetStats(const Pipeline::Stats& InStats) { Stats = InStats; }
        
    private:
        Pipeline::Stats Stats = {};
    };
}

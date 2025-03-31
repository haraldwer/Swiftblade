#pragma once
#include "Core/Debug/Window.h"
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    class FrameViewer : public Debug::Window
    {
    public:
        void DrawDebugWindow() override;
        String DebugWindowName() const override { return "Rendering"; }
        void SetStats(const Pipeline::Stats& InStats) { Stats = InStats; }
        
    private:
        Pipeline::Stats Stats;
    };
}

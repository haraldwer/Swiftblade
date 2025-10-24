#pragma once
#include "Core/Debug/Panel.h"
#include "Rendering/Pipeline/Pipeline.h"
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    class FrameViewer : public Debug::Panel
    {
    public:
        void DrawDebugPanel() override;
        String DebugPanelName() const override { return "Rendering"; }
        void SetStats(const Pipeline::Stats& InStats) { stats = InStats; }
        void DrawViewportPanel();

    private:
        void DrawTex(const Texture& tex);
        void DrawViewTex(const Texture& tex);
        Pipeline::Stats stats = {};
        int debugView = 0;
    };
}

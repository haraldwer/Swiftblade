#pragma once
#include "Core/Debug/Panel.h"

struct Texture;

namespace Rendering
{
    class FrameViewer : public Debug::Panel
    {
    public:
        void DrawPanel() override;
        String PanelName() const override { return "Rendering"; }
        void SetStats(const int& InStats) { stats = InStats; }
        void DrawViewportPanel();

    private:
        static void DrawTex(const Texture& tex);
        static void DrawViewTex(const Texture& tex);
        
        int stats = {};
        int debugView = 0;
    };
}

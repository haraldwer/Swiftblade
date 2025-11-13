#pragma once

#include "FrameViewer.h"
#include "Core/Utility/Singelton.h"
#include "Rendering/Config.h"
#include "Rendering/Context/Context.h"
#include "Rendering/Pipeline/Pipeline.h"
#include "Rendering/Viewport/Viewport.h"
#include "Rendering/Window/Window.h"

struct RenderTexture;

namespace Rendering
{
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Panel
    {
    public:
        void Init(); 
        void Deinit();

        // Draw to viewport
        void Render(const Scene& InScene);
        void BeginFrame();


        void EndFrame();
        
        void QueueConfig(const Config& InConfig);
        Config GetConfig() const { return currConfig; }

        String PanelName() const override { return "Viewport"; }
        int PanelPriority() const override { return -100; }
        bool PanelBegin(bool &InOutOpen) override;
        void DrawPanel() override;
        void PanelEnd() override;

        bool IsViewportClickable() const;
        
        Window window = {};
        Viewport mainViewport = {};
        Context defaultContext = {};
        FrameViewer frameViewer = {};
        
    private:
        void ApplyConfig(const Config& InConfig);
        void AutoResize();
        
        Config currConfig = {}; 
        Config queuedConfig = {};
        bool hovered = false;

        Vector<Vec3F> cullPoints;
    };
}


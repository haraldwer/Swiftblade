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
        Config GetConfig() const { return CurrConfig; }

        String DebugPanelName() const override { return "Viewport"; }
        void DrawDebugPanel() override;

        Window Window = {};
        Viewport MainViewport = {};
        Context DefaultContext = {};
        Pipeline DefaultPipeline = {};
        FrameViewer FrameViewer = {};
        
    private:
        void ApplyConfig(const Config& InConfig);
        
        Config CurrConfig = {}; 
        Config QueuedConfig = {};
    };
}


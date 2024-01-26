#pragma once

#include "Utility/Singelton.h"
#include "Scene.h"
#include "Config.h"

namespace Rendering
{
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Window
    {

    public:

        void Init(); 
        void Deinit();
        bool ShouldClose();

        // Draw to virtual target
        void BeginVirtualFrame(const RenderTexture2D* InTarget = nullptr);
        void EndVirtualFrame();

        // Draw to viewport
        void BeginFrame();
        void EndFrame();
        
        void ApplyConfig(const Config& InConfig);

        Vec2F GetWindowSize() const;
        Vec2F GetViewportSize() const;
        Vec2F GetViewportPosition() const;
        Vec2F GetResolution() const;
        
        String DebugWindowName() const override { return "Viewport"; }
        void DrawDebugWindow() override;
        
        void SubmitScene(const Scene& InScene) const;

    private:

        void CreateVirtualTarget(int InUnscaledWidth, int InUnscaledHeight);

        Config CurrConfig; 
        RenderTexture2D VirtualTarget = RenderTexture2D();
        Vec2F ViewportPosition;
        Vec2F ViewportSize;
    };
}


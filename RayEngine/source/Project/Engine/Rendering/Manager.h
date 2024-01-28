#pragma once

#include "Utility/Singelton.h"
#include "RenderScene.h"
#include "Config.h"
#include "SceneRenderer.h"

namespace Rendering
{
    class Manager : public Utility::Singelton<Manager, true>, public Debug::Window
    {

    public:

        void Init(); 
        void Deinit();
        bool ShouldClose();

        // Draw to virtual target
        void BeginVirtualFrame();
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
        
        void SubmitScene(const RenderScene& InScene);

    private:

        void SetViewportSize(int InUnscaledWidth, int InUnscaledHeight);
        
        Config CurrConfig; 
        Vec2F ViewportPosition;
        Vec2F ViewportSize;
        Vec2F RenderResolution;
        
        SceneRenderer Renderer;
        RenderTexture2D VirtualTarget = RenderTexture2D();
    };
}


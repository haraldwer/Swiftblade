#pragma once

#include "Rendering/TextureTargets/FrameTargetCollection.h"
#include "ViewportConfig.h"

namespace Rendering
{
    class Viewport
    {
        friend class Pipeline;
        friend class Renderer;
        
    public: 
        void Init(const ViewportConfig& InConfig);
        void Deinit();
        void Resize(const Vec2I& InSize);
        void BeginFrame();

        RenderTexture& GetVirtualTarget() const;
        FrameTargetCollection& GetTargets() { return targets; }
        
        void ImDraw();
        Vec2I GetResolution() const;
        Vec2I GetSize() const;
        Vec2F GetPosition() const { return position; }
        void ResetPosition();

        Vec2F ScreenToViewport(const Vec2F& InScreenPos);
        Vec2F ScreenToViewportAbsolute(const Vec2F& InScreenPos); // 0 - 1

    private:
        ViewportConfig config = {};
        
        Vec2F position = {};
        Mat4F viewProj = {};
        Mat4F viewProjPrev = {};
        
        Utility::Timer deltaTimer = {};
        double delta = 0;

        FrameTargetCollection targets = {};
        RenderTexture* virtualTarget = nullptr;
    };
}

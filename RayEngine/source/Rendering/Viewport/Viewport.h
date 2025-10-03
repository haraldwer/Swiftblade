#pragma once

#include "Rendering/TextureTargets/FrameTargetCollection.h"
#include "ViewportConfig.h"
#include "Context/FXConfig.h"

namespace Rendering
{
    struct FXConfig;

    class Viewport
    {
        friend class Pipeline;
        friend class Renderer;
        
    public: 
        void Init(const ViewportConfig& InConfig, const FXConfig& InFX);
        void Deinit();
        void Resize(const Vec2I& InSize);
        void BeginFrame();

        RenderTexture GetVirtualTarget() const;
        Texture* GetFrameTexture() { return targets.frameTargets.Curr().GetTextures().at(0).tex; }
        FrameTargetCollection& GetTargets() { return targets; }
        
        void ImDraw();
        Vec2I GetResolution() const;
        Vec2I GetSize() const;
        Vec2F GetPosition() const { return position; }
        void ResetPosition();

        Vec2F ScreenToViewport(const Vec2F& InScreenPos);
        Vec2F ScreenToViewportAbsolute(const Vec2F& InScreenPos); // 0 - 1
        Vec2F DistortCoord(const Vec2F& InAbsView);

    private:
        ViewportConfig config = {};
        FXConfig fx = {};
        
        Vec2F position = {};
        Mat4F viewProj = {};
        Mat4F viewProjPrev = {};
        
        Utility::Timer deltaTimer = {};
        double delta = 0;

        FrameTargetCollection targets = {};
        RenderTexture* virtualTarget = nullptr;
    };
}

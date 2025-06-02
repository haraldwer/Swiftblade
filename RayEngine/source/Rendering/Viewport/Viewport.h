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
        FrameTargetCollection& GetTargets() { return Targets; }
        
        void ImDraw();
        Vec2I GetResolution() const;
        Vec2I GetSize() const;
        Vec2F GetPosition() const { return Position; }
        void ResetPosition();

    private:
        ViewportConfig Config = {};
        
        Vec2F Position = {};
        Mat4F ViewProj = {};
        Mat4F ViewProjPrev = {};
        
        Utility::Timer DeltaTimer = {};
        double Delta = 0;

        FrameTargetCollection Targets = {};
        RenderTexture* VirtualTarget = nullptr;
    };
}

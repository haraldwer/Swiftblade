#pragma once

#include "ViewportConfig.h"
#include "Rendering/TextureTargets/FrameTargetCollection.h"

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
        void EndFrame();

        RenderTexture& GetVirtualTarget() const;
        Map<String, Vector<RenderTarget::Buffer>> GetBuffers() { return Targets.GetBuffers(); }
        
        void ImDraw();
        Vec2I GetResolution() const;
        Vec2I GetSize() const;
        Vec2F GetPosition() const { return Position; }

    private:
        ViewportConfig Config;
        
        Vec2F Position;
        Mat4F PreviousMVP;
        Mat4F PendingMVP;
        
        Utility::Timer DeltaTimer;
        double Delta = 0;

        RenderTexture* VirtualTarget = nullptr;
        FrameTargetCollection Targets;
    };
}

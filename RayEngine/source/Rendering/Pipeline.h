#pragma once

#include "Renderer.h"
#include "Targets/SwapTarget.h"

struct RenderTexture;

namespace Rendering
{
    class Pipeline : Debug::Window
    {
    public:
        void Init();
        void Setup(const RenderTexture& InVirtualTarget);
        void Render(const RenderScene& InScene, const RenderTexture& InVirtualTarget);
        ~Pipeline() override;

        void DrawDebugWindow() override;
        String DebugWindowName() const override { return "Scene Rendering"; }
        
    private:
        Renderer Renderer;

        RenderTarget SceneTarget;
        SwapTarget SSAOTargets = SwapTarget(2);
        SwapTarget FireTargets = SwapTarget(2);
        RenderTarget FrameTarget;
        RenderTarget QuantizeTarget;        
        
        Map<uint64, int> MeshDrawCount;
        int DeferredDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true;

        ResShader FireShader;
        ResShader FireBlipShader;
        ResShader SSAOShader; 
        ResShader QuantizeShader;
        ResShader FXAAShader;
    };
}

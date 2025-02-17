#pragma once
#include "Renderer.h"
#include "Targets/SwapTarget.h"

namespace Rendering
{
    class Pipeline : Debug::Window
    {
    public:
        void Init();
        void Setup(const RenderTexture2D& InVirtualTarget);
        void Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget);
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
        
        int MeshDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true;

        ResShader FireShader;
        ResShader FireBlipShader;
        ResShader SSAOShader; 
        ResShader QuantizeShader;
        ResShader FXAAShader;
    };
}

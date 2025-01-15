#pragma once
#include "Renderer.h"

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

        SwapTarget SceneTargets = SwapTarget(3);
        SwapTarget SSAOTargets = SwapTarget(2);
        
        RenderTarget FrameTarget;
        RenderTarget QuantizeTarget;        
        RenderTarget FXAATarget;
        
        int MeshDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true;

        ResShader SSAOShader; 
        ResShader QuantizeShader;
        ResShader FXAAShader;
        ResShader FireShader;
    };
}

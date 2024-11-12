#pragma once
#include "RenderScene.h"
#include "RenderTarget.h"

namespace Rendering
{
    class SceneRenderer : Debug::Window
    {
    public:
        void Init();
        void Setup(const RenderTexture2D& InVirtualTarget);
        void Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget);
        ~SceneRenderer() override;

        void DrawDebugWindow() override;
        String DebugWindowName() const override { return "Scene Rendering"; }
        
    private:
        void DrawEntries(const RenderScene& InScene, const RenderTarget& InSceneTarget);
        void DrawDeferredScene(const RenderScene& InScene, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        void DrawPostProcessing(const RenderScene& InScene, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers, const ResShader& InShader);
        void DrawDebug(const RenderScene& InScene);
        static void Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer);

        void SetShaderValues(ShaderResource& InShader, const RenderScene& InScene, const RenderTarget& InSceneTarget, uint32 InDeferredID);
        void SetCustomShaderValues(ShaderResource& InShader);

        RenderTarget SceneTarget;
        RenderTarget FrameTarget;
        RenderTarget QuantizeTarget;        
        RenderTarget FXAATarget;
        std::array<RenderTarget, 2> SSAOTargets;
        uint32 CurrentSSAOTarget = 0;
        
        int MeshDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true;

        ResShader SSAOShader; 
        ResShader QuantizeShader;
        ResShader FXAAShader;

        Mat4F PreviousMVP = { };
        Mat4F PendingMVP = { };
    };
}

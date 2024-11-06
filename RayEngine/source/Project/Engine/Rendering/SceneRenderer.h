#pragma once
#include "RenderScene.h"
#include "SceneRenderTarget.h"

namespace Rendering
{
    class SceneRenderer : Debug::Window
    {
    public:
        void Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget);
        ~SceneRenderer() override;

        void DrawDebugWindow() override;
        String DebugWindowName() const override { return "Scene Rendering"; }
        
    private:
        void DrawEntries(const RenderScene& InScene, const SceneRenderTarget& InSceneTarget);
        void DrawDeferredScene(const RenderScene& InScene, const SceneRenderTarget& InSceneTarget);
        void DrawDebug(const RenderScene& InScene);
        
        void SetShaderValues(const Shader* InShader, const RenderScene& InScene, const SceneRenderTarget& InSceneTarget);
        void SetCustomShaderValues(const Shader* InShader);

        SceneRenderTarget SceneTarget;
        int MeshDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true; 
    };
}

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
        static void DrawInstances(const ::Mesh& InMesh, const Shader& InShader, const Vector<Mat4F>& InMatrices, const Vec3F& InCameraPosition);
        static void DrawDeferredScene(const RenderScene& InScene, const SceneRenderTarget& InSceneTarget, const ResShader& InShader);
        void DrawDebug(const RenderScene& InScene);

        SceneRenderTarget SceneTarget;
        ResShader DeferredShader = ResShader("Defaults/SH_Deferred_Default");
        int MeshDrawCount = 0;
        int DebugDrawCount = 0;
        bool DebugDraw = true; 
    };
}

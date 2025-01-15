#pragma once
#include "RenderScene.h"
#include "Targets/RenderTarget.h"
#include "Targets/SwapTarget.h"

namespace Rendering
{
    class Renderer
    {
    public:
        int DrawScene(const RenderScene& InScene, SwapTarget& InSceneTarget);
        void DrawDeferredScene(const RenderScene& InScene, const RenderTarget& InTarget, SwapTarget& InSceneBuffers, const Vector<RenderTarget*>& InBuffers) const;
        void DrawFullscreen(const RenderScene& InScene, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, const Vector<RenderTarget*>& InPrevBuffers = {}, bool InClear = true) const;
        static int DrawDebug(const RenderScene& InScene);
        static void Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer);
    
    private:
        static void SetValue(ShaderResource& InShader, const String& InName, const void* InValue, int InType);
        static void SetValue(ShaderResource& InShader, const String& InName, const Matrix& InValue);

        void SetShaderValues(ShaderResource& InShader, const RenderScene& InScene, const RenderTarget& InSceneTarget, uint32 InDeferredID) const;
        void SetCustomShaderValues(ShaderResource& InShader) const;
        
        Mat4F PreviousMVP = { };
        Mat4F PendingMVP = { };
    };
}

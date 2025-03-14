#pragma once

#include "RenderScene.h"
#include "Targets/RenderTarget.h"

struct RenderTexture; 
struct Matrix;

namespace Rendering
{
    class Renderer
    {
    public:
        Map<uint64, int> DrawScene(const RenderScene& InScene, RenderTarget& InSceneTarget);
        int DrawDeferredScene(const RenderScene& InScene, const RenderTarget& InTarget,
                              const Vector<RenderTarget*>& InBuffers) const;
        void DrawFullscreen(const RenderScene& InScene, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, const Vector<RenderTarget*>& InPrevBuffers = {}, int InBlend = -1, bool InClear = true) const;
        static int DrawDebug(const RenderScene& InScene);
        static void Blip(const RenderTexture& InTarget, const RenderTarget& InBuffer);
    
    private:
        static void SetValue(ShaderResource& InShader, const String& InName, const void* InValue, int InType);
        static void SetValue(ShaderResource& InShader, const String& InName, const Matrix& InValue);

        void SetShaderValues(ShaderResource& InShader, const RenderScene& InScene, const RenderTarget& InSceneTarget, uint32 InDeferredID) const;
        void SetCustomShaderValues(ShaderResource& InShader) const;
        void SetNoiseTextures(ShaderResource& InShader) const;
        
        Mat4F PreviousMVP = { };
        Mat4F PendingMVP = { };
    };
}

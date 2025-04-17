#pragma once

#include "Rendering/Resources/Shader.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "Rendering/TextureTargets/RenderTarget.h"

struct RenderTexture; 
struct Matrix;

namespace Rendering
{
    class Lumin;
    struct LuminProbe;
    class Context;
    class Viewport;
    class Scene;

    struct RenderArgs
    {
        const Scene* Scene = nullptr;
        const Context* Context = nullptr;
        Viewport* Viewport = nullptr;
        Lumin* Lumin = nullptr;
        CameraInstance Camera;
    };
    
    class Renderer
    {
    public:
        static Map<uint64, int> DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget);
        static int DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawLuminProbes(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget);
        static void DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend = -1, bool InClear = true);
        static void DrawCubeFace(const RenderArgs& InArgs, const RenderTarget& InTarget, int InFaceIndex, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend = -1, bool InClear = true);
        static int DrawDebug(const RenderArgs& InArgs);
        static void Blip(const RenderTexture& InTarget, const RenderTarget& InBuffer);

    private:
        static void SetValue(ShaderResource& InShader, const String& InName, const void* InValue, int InType);
        static void SetValue(ShaderResource& InShader, const String& InName, const Matrix& InValue);

        static void SetShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget, uint32 InDeferredID = 0);
        static void SetNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader);
        static void SetCustomShaderValues(ShaderResource& InShader);
    };
}

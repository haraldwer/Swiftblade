#pragma once

#include "Rendering/Scene/Instances/CameraInstance.h"
#include "Rendering/Resources/Shader.h"

struct RenderTexture; 
struct Matrix;
struct Mesh;

namespace Rendering
{
    class RenderTarget;
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
        static void DrawQuad();
        static int DrawInstances(const Mesh& InMesh, const Vector<Mat4F>& InVector);
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
        static void SetValue(ShaderResource& InShader, const String& InName, const Mat4F& InValue);

        static void SetFrameShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget);
        static void BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        static void SetCustomShaderValues(ShaderResource& InShader);
    };
}

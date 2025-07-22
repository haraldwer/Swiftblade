#pragma once

#include "Collections/SplitContainer.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "Rendering/Resources/Shader.h"

struct RenderTexture; 
struct Matrix;
struct Mesh;

namespace Rendering
{
    class Lights;
    class RenderTarget;
    class Lumin;
    struct LuminProbe;
    class Context;
    class Viewport;
    class Scene;

    struct Perspective
    {
        Vec4F referenceRect = {};
        Vec4I targetRect = {};
        CameraInstance camera = {};
    };
    
    struct RenderArgs
    {
        const Scene* scenePtr = nullptr;
        const Context* contextPtr = nullptr;
        Viewport* viewportPtr = nullptr;
        Lumin* luminPtr = nullptr;
        Lights* lightsPtr = nullptr;
        Vector<Perspective> perspectives = {};
        Vector<Vec3F> cullPoints = {};
        uint8 cullMask = 255;
    };
    
    class Renderer
    {
    public:
        static int DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget);
        static Map<uint64, int> DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget);
        static void DrawQuad();
        static int DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawLuminProbes(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static int DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        static void DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend = -1, bool InClear = true);
        static int DrawDebug(const RenderArgs& InArgs);
        static void Blip(const RenderTexture& InTarget, const RenderTarget& InBuffer);
    
    private:
        static void SetValue(ShaderResource& InShader, const String& InName, const void* InValue, int InType, int InCount = 1);
        static void SetValue(ShaderResource& InShader, const String& InName, const Mat4F& InValue);
        static void SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const void* InValue, int InType, int InCount = 1);
        static void SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const Mat4F& InValue);

        static void SetFrameShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget);
        static void SetPerspectiveShaderValues(const RenderArgs& InArgs, const Perspective& InPerspective, ShaderResource& InShader);
        static void BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        static void SetCustomShaderValues(ShaderResource& InShader);
        
        static int DrawInstances(const Mesh& InMesh, int InCount);
    };
}

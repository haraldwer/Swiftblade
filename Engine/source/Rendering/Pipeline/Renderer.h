#pragma once

#include "Utility/Collections/SplitContainer.h"
#include "Interface/Shaders.h"
#include "Rendering/Scene/Instances/CameraInstance.h"
#include "Rendering/Resources/Shader.h"
#include "TextureTargets/SwapTarget.h"

struct RenderTexture; 
struct Matrix;
struct Mesh;

namespace Rendering
{
    class Lights;
    class RenderTarget;
    class Lumin;
    struct LuminProbe;
    class Particles;
    class Context;
    class Viewport;
    class Scene;
    class BakedTexture;

    struct Perspective
    {
        Vec4F referenceRect = {};
        Vec4F targetRect = {};
        CameraInstance camera = {};
        int layerFace = 0;
    };
    
    struct RenderArgs
    {
        const Scene* scenePtr = nullptr;
        const Context* contextPtr = nullptr;
        Viewport* viewportPtr = nullptr;
        Lumin* luminPtr = nullptr;
        Lights* lightsPtr = nullptr;
        Particles* particlesPtr = nullptr;
        Vector<Perspective> perspectives = {};
        Vector<Vec3F> cullPoints = {};
        uint8 cullMask = 255;
    };
    
    class Renderer
    {
    public:
        static void DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend = -1, bool InClear = false);
        static void DrawBloom(const RenderArgs & InArgs, SwapTarget& InBloom, SwapTarget& InFrame);
        static int DrawCustom(const RenderArgs & InArgs, const std::function<void()> & InFunc);
        static int DrawDebug(const RenderArgs& InArgs);
        static void Blip(const RenderTexture& InTarget, const RenderTarget& InBuffer);
        static bool Bake(const BakedTexture& InTex);
    
    protected:
        static void SetValue(ShaderResource& InShader, const String& InName, const void* InValue, UniformType InType, int InCount = 1);
        static void SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const void* InValue, UniformType InType, int InCount = 1);

        static void SetFrame(const RenderArgs& InArgs, ShaderResource& InShader);
        static void SetPerspective(const RenderArgs& InArgs, const Perspective& InPerspective, const RenderTarget& InTarget, ShaderResource& InShader);
        static void BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        
        static void SetCustomShaderValues(ShaderResource& InShader);
        
        static int DrawInstances(const Mesh& InMesh, int InCount);
        static void DrawQuad();
    };
}

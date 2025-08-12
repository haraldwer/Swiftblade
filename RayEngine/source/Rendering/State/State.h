#pragma once
#include "Command.h"

namespace Rendering
{
    class State
    {
    public:
        
        void Set(const UniformCommand& InCmd, int InSlot);
        void Set(const TextureCommand& InCmd, int InSlot);
        bool Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices);
        bool Set(const Vector<Mat4F>& InMatrices);
        void Set(const ShaderCommand& InCmd, bool InForce = false);
        void Set(const PerspectiveCommand& InCmd, bool InForce = false);
        void Set(const FrameCommand& InCmd);
        
        void Reset();
        void ResetUniforms();
        void ResetTextures();
        void ResetShader();
        void ResetPerspective();
        void ResetFrame();
        void ResetMesh();
        void ResetTransforms();

        static void Check();
        
    private:
        
        Map<int, TextureCommand> textures = {};
        Map<int, UniformCommand> uniforms = {};
        
        ShaderCommand shader = {};
        FrameCommand frame = {};
        PerspectiveCommand perspective = {};
        MeshCommand mesh = {};

        uint32 vbo = static_cast<uint32>(-1);
        int activeTexSlot = -1;
    };

    struct rlState
    {
        inline static State current = {};
    };
    
}

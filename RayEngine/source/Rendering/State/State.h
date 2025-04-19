#pragma once
#include "Command.h"

namespace Rendering
{
    class State
    {
    public:
        void Set(const TextureCommand& InCmd, int InSlot);
        void Set(const ShaderCommand& InCmd);
        void Set(const FrameCommand& InCmd);
        bool Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices = {});
        
        void Reset();
        void Check();

    private:

        void ResetTextures();
        void ResetShader();
        void ResetFrame();
        void ResetMesh();
        
        Map<int, TextureCommand> Textures;
        ShaderCommand Shader;
        FrameCommand Frame;
        MeshCommand Mesh;

        uint32 VBO = static_cast<uint32>(-1);
    };

    struct rlState
    {
        inline static State Current;
    };
    
}

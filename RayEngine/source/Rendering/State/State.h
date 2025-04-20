#pragma once
#include "Command.h"

namespace Rendering
{
    class State
    {
    public:

        // State hierarhcy:
        // 1 Frame
        // 2 Shader
        // 3 Texture / Mesh

        // State resets
        // Frame: Shader, Textures
        // Shader: Textures?, Mesh?
        // Texture: -
        // Mesh: - 
        
        void Set(const TextureCommand& InCmd, int InSlot);
        bool Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices);
        void Set(const ShaderCommand& InCmd, bool InForce = false);
        void Set(const FrameCommand& InCmd);
        
        void Reset();
        void ResetTextures();
        void ResetShader();
        void ResetFrame();
        void ResetMesh();
        
        void Check();
        
    private:
        
        Map<int, TextureCommand> Textures;
        
        ShaderCommand Shader;
        FrameCommand Frame;

        uint32 VBO = static_cast<uint32>(-1);
    };

    struct rlState
    {
        inline static State Current;
    };
    
}

#include "State.h"

#include "RayRenderUtility.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void Rendering::State::Set(const UniformCommand& InCmd, int InSlot)
{
    
}

void Rendering::State::Set(const TextureCommand& InCmd, int InSlot)
{
    PROFILE_GL_GPU("Set texture");
    
    auto& tex = Textures[InSlot];
    
    if (InCmd.Wrap != tex.Wrap)
    {
        int w = InCmd.Wrap >= 0 ? InCmd.Wrap : RL_TEXTURE_WRAP_CLAMP;
        rlTextureParameters(InCmd.ID, RL_TEXTURE_WRAP_S, w);
        rlTextureParameters(InCmd.ID, RL_TEXTURE_WRAP_T, w);
    }

    if (InCmd.Filter != tex.Filter)
    {
        int f = InCmd.Filter > 0 ? InCmd.Filter : RL_TEXTURE_FILTER_NEAREST;
        rlTextureParameters(InCmd.ID, RL_TEXTURE_MIN_FILTER, f);
        rlTextureParameters(InCmd.ID, RL_TEXTURE_MAG_FILTER, f);
    }
    
    if (InCmd.ID != tex.ID)
    {
        rlActiveTextureSlot(InSlot);
        if (InCmd.ID != static_cast<uint32>(-1))
        {
            InCmd.Cubemap ? rlEnableTextureCubemap(InCmd.ID) : rlEnableTexture(InCmd.ID);
            if (InCmd.ShaderLoc >= 0) // Textures are cleared when switching shader
                rlSetUniform(InCmd.ShaderLoc, &InSlot, SHADER_UNIFORM_INT, 1);
        }
        else
        {
            InCmd.Cubemap ? rlDisableTextureCubemap() : rlDisableTexture();
            int l = 0;
            rlSetUniform(InCmd.ShaderLoc, &l, SHADER_UNIFORM_INT, 1);
        }
    }
    
    Textures[InSlot] = InCmd;
}

void Rendering::State::ResetTextures()
{
    if (!Textures.empty())
    {
        PROFILE_GL_GPU("Reset textures");
        for (auto& tex : Textures)
        {
            rlActiveTextureSlot(tex.first);
            tex.second.Cubemap ? rlDisableTextureCubemap() : rlDisableTexture();
            int l = 0;
            rlSetUniform(tex.second.ShaderLoc, &l, SHADER_UNIFORM_INT, 1);
        }
        Textures.clear();
        rlActiveTextureSlot(0);
    }
}

bool Rendering::State::Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices)
{
    ResetMesh();
    
    if (InCmd.vaoID != static_cast<uint32>(-1) && Shader.Locs)
    {
        PROFILE_GL_GPU("Set mesh");
        
        rlEnableVertexArray(InCmd.vaoID);

        int instances = static_cast<int>(InMatrices.size());
        int bufferSize = instances * static_cast<int>(sizeof(Mat4F));
        VBO = rlLoadVertexBuffer(InMatrices.data(), bufferSize, false);

        // Set instance matrix data location
        int matLoc = Shader.Locs[SHADER_LOC_MATRIX_MODEL];
        if (matLoc != -1)
        {
            for (unsigned int i = 0; i < 4; i++)
            {
                rlEnableVertexAttribute(matLoc + i);
                rlSetVertexAttribute(matLoc + i, 4, RL_FLOAT, false, sizeof(Mat4F), static_cast<int>(i * sizeof(Vec4F)));
                rlSetVertexAttributeDivisor(matLoc + i, 1);
            }
        }

        // TODO: Remove?
        if (Shader.Locs[SHADER_LOC_MATRIX_NORMAL] != -1)
            rlSetUniformMatrix(Shader.Locs[SHADER_LOC_MATRIX_NORMAL], MatrixIdentity());
        
        // TODO: Only disable if no vert color
        int colorLoc = Shader.Locs[SHADER_LOC_VERTEX_COLOR];
        if (colorLoc != -1)
            rlDisableVertexAttribute(colorLoc);
    }
    
    return true;
}

void Rendering::State::ResetMesh()
{
    if (VBO != static_cast<uint32>(-1))
    {
        PROFILE_GL_GPU("Reset mesh");
        rlDisableVertexArray();
        rlDisableVertexBuffer();
        rlDisableVertexBufferElement();
        rlUnloadVertexBuffer(VBO);
        VBO = static_cast<uint32>(-1);
    }
}


void Rendering::State::Set(const ShaderCommand& InCmd, bool InForce)
{
    PROFILE_GL_GPU("Set shader");
    
    if (InCmd.ID != Shader.ID || InForce)
    {
        Textures.clear();
        InCmd.ID != static_cast<uint32>(-1) ?
            rlEnableShader(InCmd.ID) : rlDisableShader();
    }

    if (InCmd.BlendMode != Shader.BlendMode || InForce)
        RaylibRenderUtility::SetBlendMode(InCmd.BlendMode);

    if (InCmd.BackfaceCulling != Shader.BackfaceCulling || InForce)
        InCmd.BackfaceCulling ? rlEnableBackfaceCulling() : rlDisableBackfaceCulling();
    if (InCmd.DepthTest != Shader.DepthTest || InForce)
        InCmd.DepthTest ? rlEnableDepthTest() : rlDisableDepthTest();
    if (InCmd.DepthMask != Shader.DepthMask || InForce)
        InCmd.DepthMask ? rlEnableDepthMask() : rlDisableDepthMask();

    Shader = InCmd;
}

void Rendering::State::ResetShader()
{
    PROFILE_GL_GPU("Reset shader");
    RaylibRenderUtility::SetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlEnableBackfaceCulling();
    rlDisableDepthTest();
    rlDisableDepthMask();
    rlDisableShader();
    Shader = {};
}

void Rendering::State::Set(const PerspectiveCommand& InCmd, const bool InForce)
{
    PROFILE_GL_GPU("Set perspective");
    
    if (Perspective.Rect != InCmd.Rect || InForce)
    {
        int w = InCmd.Rect.z > 0 ? InCmd.Rect.z : Frame.Size.x;
        int h = InCmd.Rect.w > 0 ? InCmd.Rect.w : Frame.Size.y;
        rlViewport(InCmd.Rect.x, InCmd.Rect.y, w, h);
    }
    Perspective = InCmd;
}

void Rendering::State::ResetPerspective()
{
    PROFILE_GL_GPU("Reset perspective");
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    Perspective = {};
}

void Rendering::State::Set(const FrameCommand& InCmd)
{
    PROFILE_GL_GPU("Set frame");
    
    if (InCmd.fboID != Frame.fboID)
    {
        Textures.clear();
        if (InCmd.fboID != static_cast<uint32>(-1))
            rlEnableFramebuffer(InCmd.fboID);
        else rlDisableFramebuffer();
        Perspective = PerspectiveCommand();
        CHECK_ASSERT(InCmd.Size.x == 0, "Invalid size x")
        CHECK_ASSERT(InCmd.Size.y == 0, "Invalid size y")
        rlViewport(0, 0, InCmd.Size.x, InCmd.Size.y);
    }

    if (InCmd.fboID != static_cast<uint32>(-1) && (InCmd.ClearTarget || InCmd.ClearDepth))
    {
        int mask = 0;
        if (InCmd.ClearTarget)
            mask |= GL_COLOR_BUFFER_BIT;
        if (InCmd.ClearDepth)
            mask |= GL_DEPTH_BUFFER_BIT;
        
        if (InCmd.ClearDepth)
        {
            // Enable depth before clearing
            rlEnableDepthTest();
            rlEnableDepthMask();
        }

        glClear(mask);
        
        if (InCmd.ClearDepth && InCmd.fboID == Frame.fboID) 
        {
            // Shader will not be reset, set the depth values manually
            if (!Shader.DepthMask)
                rlDisableDepthMask();
            if (!Shader.DepthTest)
                rlDisableDepthTest();
        }
    }
    
    if (InCmd.fboID != Frame.fboID)
        Set(ShaderCommand(), true);

    Frame = InCmd;
}

void Rendering::State::ResetFrame()
{
    PROFILE_GL_GPU("Reset frame");
    rlDisableFramebuffer();
    Frame = {};
}

void Rendering::State::Reset()
{
    PROFILE_GL();
    PROFILE_GL_GPU("Reset state");
    ResetUniforms();
    ResetTextures();
    ResetMesh();
    ResetShader();
    ResetFrame();
    ResetPerspective();
}

void Rendering::State::ResetUniforms()
{
    Uniforms.clear();
}

void Rendering::State::Check()
{
    rlCheckErrors();
}

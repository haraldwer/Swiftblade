#include "State.h"

#include "raylib.h"
#include "raymath.h"
#include "RayRenderUtility.h"
#include "rlgl.h"

void Rendering::State::Set(const TextureCommand& InCmd, int InSlot)
{
    auto& tex = Textures[InSlot];
    
    if (InCmd.Wrap != tex.Wrap)
    {
        int w = InCmd.Wrap >= 0 ? InCmd.Wrap : RL_TEXTURE_WRAP_CLAMP;
        rlTextureParameters(InCmd.ID, RL_TEXTURE_WRAP_S, w);
        rlTextureParameters(InCmd.ID, RL_TEXTURE_WRAP_T, w);
    }

    if (InCmd.Filter != tex.Filter)
    {
        int f = InCmd.Filter != 0 ? InCmd.Filter : RL_TEXTURE_FILTER_TRILINEAR;
        rlTextureParameters(InCmd.ID, RL_TEXTURE_MAG_FILTER, f);
        rlTextureParameters(InCmd.ID, RL_TEXTURE_MIN_FILTER, f);
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
        rlDisableVertexArray();
        rlDisableVertexBuffer();
        rlDisableVertexBufferElement();
        rlUnloadVertexBuffer(VBO);
        VBO = static_cast<uint32>(-1);
    }
}


void Rendering::State::Set(const ShaderCommand& InCmd, bool InForce)
{
    if (InCmd.ID != Shader.ID || InForce)
    {
        Textures.clear();
        InCmd.ID != static_cast<uint32>(-1) ?
            rlEnableShader(InCmd.ID) : rlDisableShader();
    }

    if (InCmd.BlendMode != Shader.BlendMode || InForce)
    {
        if (InCmd.BlendMode >= 0)
        {
            rlEnableColorBlend();
            RaylibRenderUtility::SetBlendMode(InCmd.BlendMode);
        }
        else
        {
            rlDisableColorBlend();
        }
    }

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
    RaylibRenderUtility::SetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlEnableBackfaceCulling();
    rlDisableDepthTest();
    rlDisableDepthMask();
    rlDisableShader();
    Shader = {};
}

void Rendering::State::Set(const PerspectiveCommand& InCmd, bool InForce)
{
    if (Perspective.Rect != InCmd.Rect || InForce)
    {
        int w = InCmd.Rect.z > 0 ? InCmd.Rect.z : rlGetFramebufferWidth();
        int h = InCmd.Rect.w > 0 ? InCmd.Rect.w : rlGetFramebufferHeight();
        rlViewport(InCmd.Rect.x, InCmd.Rect.y, w, h);
    }
    Perspective = InCmd;
}

void Rendering::State::ResetPerspective()
{
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    Perspective = {};
}

void Rendering::State::Set(const FrameCommand& InCmd)
{
    if (InCmd.fboID != Frame.fboID)
    {
        Textures.clear();
        if (InCmd.fboID != static_cast<uint32>(-1))
            rlEnableFramebuffer(InCmd.fboID);
        else rlDisableFramebuffer();
        ResetPerspective();
    }
    
    if (InCmd.Clear && InCmd.fboID != static_cast<uint32>(-1))
        rlClearScreenBuffers();
    
    if (InCmd.fboID != Frame.fboID)
        Set(ShaderCommand(), true);

    Frame = InCmd;
}

void Rendering::State::ResetFrame()
{
    rlDisableFramebuffer();
    Frame = {};;
}

void Rendering::State::Reset()
{
    ResetTextures();
    ResetMesh();
    ResetShader();
    ResetFrame();
    ResetPerspective();
    rlDrawRenderBatchActive();
}

void Rendering::State::Check()
{
    rlCheckErrors();
}

#include "State.h"

#include "RayRenderUtility.h"
#include "raylib.h"
#include "raymath.h"
#include "rlgl.h"

void Rendering::State::Set(const UniformCommand& InCmd, int InSlot)
{
    
}

void Rendering::State::Set(const TextureCommand& InCmd, const int InSlot)
{
    PROFILE_GL_GPU("Set texture");

    const auto& tex = textures[InSlot];
    
    if (InCmd.wrap != tex.wrap)
    {
        const int w = InCmd.wrap >= 0 ? InCmd.wrap : RL_TEXTURE_WRAP_CLAMP;
        rlTextureParameters(InCmd.id, RL_TEXTURE_WRAP_S, w);
        rlTextureParameters(InCmd.id, RL_TEXTURE_WRAP_T, w);
    }

    if (InCmd.filter != tex.filter)
    {
        const int f = InCmd.filter > 0 ? InCmd.filter : RL_TEXTURE_FILTER_NEAREST;
        rlTextureParameters(InCmd.id, RL_TEXTURE_MIN_FILTER, f);
        rlTextureParameters(InCmd.id, RL_TEXTURE_MAG_FILTER, f);
    }
    
    if (InCmd.id != tex.id)
    {
        rlActiveTextureSlot(InSlot);
        if (InCmd.id != static_cast<uint32>(-1))
        {
            InCmd.cubemap ? rlEnableTextureCubemap(InCmd.id) : rlEnableTexture(InCmd.id);
            if (InCmd.shaderLoc >= 0) // Textures are cleared when switching shader
                rlSetUniform(InCmd.shaderLoc, &InSlot, SHADER_UNIFORM_INT, 1);
        }
        else
        {
            InCmd.cubemap ? rlDisableTextureCubemap() : rlDisableTexture();
            const int l = 0;
            rlSetUniform(InCmd.shaderLoc, &l, SHADER_UNIFORM_INT, 1);
        }
    }
    
    textures[InSlot] = InCmd;
}

void Rendering::State::ResetTextures()
{
    if (!textures.empty())
    {
        PROFILE_GL_GPU("Reset textures");
        for (const auto& tex : textures)
        {
            rlActiveTextureSlot(tex.first);
            tex.second.cubemap ? rlDisableTextureCubemap() : rlDisableTexture();
            int l = 0;
            rlSetUniform(tex.second.shaderLoc, &l, SHADER_UNIFORM_INT, 1);
        }
        textures.clear();
        rlActiveTextureSlot(0);
    }
}

bool Rendering::State::Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices)
{
    ResetMesh();
    
    if (InCmd.vaoID != static_cast<uint32>(-1) && shader.locs)
    {
        PROFILE_GL_GPU("Set mesh");
        
        rlEnableVertexArray(InCmd.vaoID);

        Set(InMatrices);

        // Set instance matrix data location
        const int matLoc = shader.locs[SHADER_LOC_MATRIX_MODEL];
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
        if (shader.locs[SHADER_LOC_MATRIX_NORMAL] != -1)
            rlSetUniformMatrix(shader.locs[SHADER_LOC_MATRIX_NORMAL], MatrixIdentity());
        
        // TODO: Only disable if no vert color
        const int colorLoc = shader.locs[SHADER_LOC_VERTEX_COLOR];
        if (colorLoc != -1)
            rlDisableVertexAttribute(colorLoc);
    }
    
    return true;
}


bool Rendering::State::Set(const Vector<Mat4F>& InMatrices)
{
    const int instances = static_cast<int>(InMatrices.size());
    const int bufferSize = instances * static_cast<int>(sizeof(Mat4F));

    if (vbo != static_cast<uint32>(-1))
        ResetTransforms();
    vbo = rlLoadVertexBuffer(InMatrices.data(), bufferSize, false);
    
    return true;
}

void Rendering::State::ResetMesh()
{
    if (mesh.vaoID != static_cast<uint32>(-1))
    {
        rlDisableVertexArray();
        ResetTransforms();
    }
    mesh = {};
}

void Rendering::State::ResetTransforms()
{
    if (vbo != static_cast<uint32>(-1))
    {
        PROFILE_GL_GPU("Reset mesh");
        rlDisableVertexBuffer();
        rlDisableVertexBufferElement();
        rlUnloadVertexBuffer(vbo);
        vbo = static_cast<uint32>(-1);
    }
}

void Rendering::State::Set(const ShaderCommand& InCmd, const bool InForce)
{
    PROFILE_GL_GPU("Set shader");
    
    if (InCmd.id != shader.id || InForce)
    {
        textures.clear();
        InCmd.id != static_cast<uint32>(-1) ?
            rlEnableShader(InCmd.id) : rlDisableShader();
    }

    if (InCmd.blendMode != shader.blendMode || InForce)
        RaylibRenderUtility::SetBlendMode(InCmd.blendMode);

    if (InCmd.backfaceCulling != shader.backfaceCulling || InForce)
        InCmd.backfaceCulling ? rlEnableBackfaceCulling() : rlDisableBackfaceCulling();
    if (InCmd.depthTest != shader.depthTest || InForce)
        InCmd.depthTest ? rlEnableDepthTest() : rlDisableDepthTest();
    if (InCmd.depthMask != shader.depthMask || InForce)
        InCmd.depthMask ? rlEnableDepthMask() : rlDisableDepthMask();

    shader = InCmd;
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
    shader = {};
}

void Rendering::State::Set(const PerspectiveCommand& InCmd, const bool InForce)
{
    PROFILE_GL_GPU("Set perspective");
    
    if (perspective.rect != InCmd.rect || InForce)
    {
        const int w = InCmd.rect.z > 0 ? InCmd.rect.z : frame.size.x;
        const int h = InCmd.rect.w > 0 ? InCmd.rect.w : frame.size.y;
        rlViewport(InCmd.rect.x, InCmd.rect.y, w, h);
    }
    perspective = InCmd;
}

void Rendering::State::ResetPerspective()
{
    PROFILE_GL_GPU("Reset perspective");
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    perspective = {};
}

void Rendering::State::Set(const FrameCommand& InCmd)
{
    PROFILE_GL_GPU("Set frame");
    
    if (InCmd.fboID != frame.fboID)
    {
        textures.clear();
        if (InCmd.fboID != static_cast<uint32>(-1))
            rlEnableFramebuffer(InCmd.fboID);
        else rlDisableFramebuffer();
        perspective = PerspectiveCommand();
        CHECK_ASSERT(InCmd.size.x == 0, "Invalid size x")
        CHECK_ASSERT(InCmd.size.y == 0, "Invalid size y")
        rlViewport(0, 0, InCmd.size.x, InCmd.size.y);
    }

    if (InCmd.fboID != static_cast<uint32>(-1) && (InCmd.clearTarget || InCmd.clearDepth))
    {
        int mask = 0;
        if (InCmd.clearTarget)
            mask |= GL_COLOR_BUFFER_BIT;
        if (InCmd.clearDepth)
            mask |= GL_DEPTH_BUFFER_BIT;
        
        if (InCmd.clearDepth)
        {
            // Enable depth before clearing
            rlEnableDepthTest();
            rlEnableDepthMask();
        }

        glClear(mask);
        
        if (InCmd.clearDepth && InCmd.fboID == frame.fboID) 
        {
            // Shader will not be reset, set the depth values manually
            if (!shader.depthMask)
                rlDisableDepthMask();
            if (!shader.depthTest)
                rlDisableDepthTest();
        }
    }
    
    if (InCmd.fboID != frame.fboID)
        Set(ShaderCommand(), true);

    frame = InCmd;
}

void Rendering::State::ResetFrame()
{
    PROFILE_GL_GPU("Reset frame");
    rlDisableFramebuffer();
    frame = {};
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
    uniforms.clear();
}

void Rendering::State::Check()
{
    rlCheckErrors();
}

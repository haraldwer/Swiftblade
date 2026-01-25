#include "State.h"

#include "Interface/Meshes.h"
#include "Interface/Textures.h"
#include "RenderingOld/Utility.h"

// TODO: Completely switch to opengl? 

void Rendering::State::Set(const UniformCommand& InCmd)
{
    PROFILE_GL_GPU("Set uniform");

    // TODO: Set value hash
    
    UniformCommand& uniform = uniforms[InCmd.loc];
    if (InCmd != uniform)
    {
        RHI::SetUniform(InCmd.loc, InCmd.type, InCmd.ptr, InCmd.count);
        uniform = InCmd;
    }
}

void Rendering::State::ResetUniforms()
{
    uniforms.clear();
}

void Rendering::State::Set(const TextureCommand& InCmd, const int InSlot)
{
    PROFILE_GL_GPU("Set texture");

    TextureCommand& tex = textures[InSlot];

    if (InCmd != tex)
    {
        if (activeTexSlot != InSlot)
        {
            RHI::ActivateTextureSlot(InSlot);
            activeTexSlot = InSlot;
        }
        
        if (InCmd.id != tex.id)
        {
            if (InCmd.id != static_cast<uint32>(-1))
            {
                RHI::EnableTexture(InCmd.id, InCmd.type);
                if (InCmd.shaderLoc >= 0) // Textures are cleared when switching shader
                    RHI::SetUniform(InCmd.shaderLoc, &InSlot);
            }
            else
            {
                RHI::DisableTexture(tex.type);
                const int l = 0;
                RHI::SetUniform(InCmd.shaderLoc, &l);
            }
        }
        
        if (InCmd.wrap != tex.wrap || InCmd.filter != tex.filter)
        {
            RHI::BindTexture(InCmd.id, InCmd.type);
            if (InCmd.wrap != tex.wrap)
            {
                auto w = InCmd.wrap != TextureParamValue::NONE ? InCmd.wrap : TextureParamValue::WRAP;
                RHI::SetTextureParam(TextureParam::WRAP_S, w, InCmd.type);
                RHI::SetTextureParam(TextureParam::WRAP_T, w, InCmd.type);
            }

            if (InCmd.filter != tex.filter)
            {
                auto f = InCmd.filter != TextureParamValue::NONE ? InCmd.filter : TextureParamValue::NEAREST;
                RHI::SetTextureParam(TextureParam::MIN_FILTER, f, InCmd.type);
                RHI::SetTextureParam(TextureParam::MAG_FILTER, f, InCmd.type);
            }
        }
        
        tex = InCmd;
    }
}

void Rendering::State::ResetTextures()
{
    if (!textures.empty())
    {
        PROFILE_GL_GPU("Reset textures");
        for (const auto& tex : textures)
        {
            RHI::ActivateTextureSlot(tex.first);
            RHI::SetTextureParam(TextureParam::WRAP_S, TextureParamValue::CLAMP_TO_EDGE, tex.second.type);
            RHI::SetTextureParam(TextureParam::WRAP_T, TextureParamValue::CLAMP_TO_EDGE, tex.second.type);
            RHI::SetTextureParam(TextureParam::MIN_FILTER, TextureParamValue::NEAREST, tex.second.type);
            RHI::SetTextureParam(TextureParam::MAG_FILTER, TextureParamValue::NEAREST, tex.second.type);
            
            int l = 0;
            RHI::SetUniform(tex.second.shaderLoc, &l);
            RHI::DisableTexture(tex.second.type);
        }
        textures.clear();
        RHI::ActivateTextureSlot(0);
        activeTexSlot = -1;
    }
}

bool Rendering::State::Set(const MeshCommand& InCmd, const Vector<Mat4F>& InMatrices)
{
    ResetMesh();
    
    if (InCmd.vaoID != static_cast<uint32>(-1) && shader.locs)
    {
        PROFILE_GL_GPU("Set mesh");
        RHI::BindVertexArray(InCmd.vaoID);
        Set(InMatrices);
        RHI::EnableVertexMatrixAttributes();
    }
    
    return true;
}


bool Rendering::State::Set(const Vector<Mat4F>& InMatrices)
{
    RHI::LoadMatrixVertexBuffer(InMatrices);
    return true;
}

void Rendering::State::ResetMesh()
{
    if (mesh.vaoID != static_cast<uint32>(-1))
    {
        RHI::UnbindVertexArray();
        ResetTransforms();
    }
    mesh = {};
}

void Rendering::State::ResetTransforms()
{
    if (vbo != static_cast<uint32>(-1))
    {
        PROFILE_GL_GPU("Reset mesh");
        RHI::DisableVertexBuffer();
        RHI::DisableVertexBufferElement();
        RHI::UnloadVertexBuffer(vbo);
        vbo = static_cast<uint32>(-1);
    }
}

void Rendering::State::Set(const ShaderCommand& InCmd, const bool InForce)
{
    PROFILE_GL_GPU("Set shader");

    /*
    if (InCmd.id != shader.id || InForce)
    {
        textures.clear();
        uniforms.clear();
        InCmd.id != static_cast<uint32>(-1) ?
            rlEnableShader(InCmd.id) : rlDisableShader();
    }

    if (InCmd.blendMode != shader.blendMode || InForce)
        SetBlendMode(InCmd.blendMode);

    if (InCmd.backfaceCulling != shader.backfaceCulling || InForce)
        InCmd.backfaceCulling ? rlEnableBackfaceCulling() : rlDisableBackfaceCulling();
    if (InCmd.depthTest != shader.depthTest || InForce)
        InCmd.depthTest ? rlEnableDepthTest() : rlDisableDepthTest();
    if (InCmd.depthMask != shader.depthMask || InForce)
        InCmd.depthMask ? rlEnableDepthMask() : rlDisableDepthMask();
*/
    shader = InCmd;
}

void Rendering::State::ResetShader()
{
    /*
    if (shader.id == static_cast<uint32>(-1))
        return;
    PROFILE_GL_GPU("Reset shader");
    SetBlendMode(RL_BLEND_ALPHA);
    rlDisableColorBlend();
    rlEnableBackfaceCulling();
    rlDisableDepthTest();
    rlDisableDepthMask();
    rlDisableShader();
    */
    shader = {};
}

void Rendering::State::Set(const PerspectiveCommand& InCmd, const bool InForce)
{
    PROFILE_GL_GPU("Set perspective");
    /*
    if (perspective.rect != InCmd.rect || InForce)
    {
        Vec4F rect = {
            InCmd.rect.x,
            InCmd.rect.y,
            InCmd.rect.z > 0.0f ? InCmd.rect.z : 1.0f,
            InCmd.rect.w > 0.0f ? InCmd.rect.w : 1.0f
        };
        Vec2F size = frame.size.To<float>();
        rect.x *= size.x;
        rect.y *= size.y;
        rect.z *= size.x;
        rect.w *= size.y;
        const Vec4I intRect = rect.To<int>();
        rlViewport(intRect.x, intRect.y, intRect.z, intRect.w);
    }
    */
    perspective = InCmd;
}

void Rendering::State::ResetPerspective()
{
    /*
    if (perspective.rect == Vec4F())
        return;
    PROFILE_GL_GPU("Reset perspective");
    rlViewport(0, 0, rlGetFramebufferWidth(), rlGetFramebufferHeight());
    */
    perspective = {};
}

void Rendering::State::Set(const FrameCommand& InCmd)
{
    PROFILE_GL_GPU("Set frame");
    /*
    if (InCmd.fboID != frame.fboID)
    {
        textures.clear();
        activeTexSlot = -1;
        if (InCmd.fboID != static_cast<uint32>(-1))
            rlEnableFramebuffer(InCmd.fboID);
        else
            rlDisableFramebuffer();
        perspective = PerspectiveCommand();
        CHECK_ASSERT(InCmd.size.x == 0, "Invalid size x")
        CHECK_ASSERT(InCmd.size.y == 0, "Invalid size y")
        rlViewport(0, 0, InCmd.size.x, InCmd.size.y);
    }

    if (InCmd.fboID != static_cast<uint32>(-1) && (InCmd.clearTarget || InCmd.clearDepth))
    {
        if (InCmd.clearDepth)
        {
            // Enable depth before clearing
            rlEnableDepthTest();
            rlEnableDepthMask();
        }

        int mask = 0;
        if (InCmd.clearTarget)
            mask |= GL_COLOR_BUFFER_BIT;
        if (InCmd.clearDepth)
            mask |= GL_DEPTH_BUFFER_BIT;
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
    */
    frame = InCmd;
}

void Rendering::State::ResetFrame()
{
    /*
    if (frame.fboID == static_cast<uint32>(-1))
        return;
    PROFILE_GL_GPU("Reset frame");
    rlDisableFramebuffer();
    */
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
    Check();
}

void Rendering::State::Check()
{
    /*
#ifdef _DEBUG
    rlCheckErrors();
#endif
    */
}

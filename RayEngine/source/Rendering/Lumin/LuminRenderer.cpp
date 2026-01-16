#include "LuminRenderer.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"

void Rendering::LuminRenderer::ApplyLumin(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    Lumin &lumin = *InArgs.luminPtr;
    auto data = lumin.GetFrameData();
    
    // Set layer data
    SetValue(InShader, "ProbeSize", &data.probeSize, SHADER_UNIFORM_FLOAT, 1);

    Vec2F nearfar = Vec2F(lumin.config.Near, lumin.config.Far);
    SetValue(InShader, "ProbeNearFar", &nearfar, SHADER_UNIFORM_VEC2);

    // Bind textures
    BindBRDF(InArgs, InShader, InOutSlot);
    if (data.shTarget)
        data.shTarget->Bind(InShader, InOutSlot, RL_TEXTURE_FILTER_LINEAR);
}

void Rendering::LuminRenderer::BindBRDF(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    if (auto brdf = InArgs.luminPtr->config.TexBRDF.Get().Get())
        brdf->Get().Bind(InShader, InOutSlot, RL_TEXTURE_FILTER_LINEAR);
}

int Rendering::LuminRenderer::DrawLuminProbesDebug(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_GL_NAMED("Lumin debug");
    
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    auto& lumin = *InArgs.luminPtr;
    
    LuminConfig conf = InArgs.luminPtr->config;
    CHECK_RETURN(!conf.Enabled, {})
    CHECK_RETURN(!conf.Debug, {})
    
    Vector<Mat4F> probes = lumin.GetDebugProbes(InArgs);
    CHECK_RETURN(probes.empty(), {})
    
    const auto& debugRes = conf.DebugShader;
    ShaderResource* shaderRes = debugRes.Get().Get();
    CHECK_RETURN(!shaderRes, 0);
    const Shader* shader = shaderRes->Get();
    CHECK_RETURN(!shader, 0);

    auto model = conf.SphereModel.Get().Get();
    auto* modelRes = model->Get();
    CHECK_RETURN(!modelRes, 0);
    CHECK_RETURN(!modelRes->meshCount, 0);
    Mesh& mesh = modelRes->meshes[0];

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    rlState::current.Set(frameCmd);
    
    ShaderCommand debugShaderCmd;
    debugShaderCmd.locs = shader->locs;
    debugShaderCmd.id = shader->id;
    debugShaderCmd.depthTest = true;
    debugShaderCmd.depthMask = true;
    rlState::current.Set(debugShaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderRes);
    
    int texSlot = 0;
    ApplyLumin(InArgs, *shaderRes, texSlot);
    BindNoiseTextures(InArgs, *shaderRes, texSlot);
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderRes, texSlot);

    MeshCommand cmd;
    cmd.vaoID = mesh.vaoId;

    // Draw all probes in one call
    if (rlState::current.Set(cmd, probes))
    {
        // Draw every perspective
        for (auto& perspective : InArgs.perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.rect = perspective.targetRect;
            rlState::current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, InTarget, *shaderRes);
            DrawInstances(mesh, 1);
        }
    }
    
    rlState::current.ResetMesh();
    return static_cast<int>(probes.size());
}

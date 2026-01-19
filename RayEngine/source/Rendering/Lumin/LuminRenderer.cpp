#include "LuminRenderer.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"

void Rendering::LuminRenderer::SetLuminValues(const RenderArgs& InArgs, ShaderResource& InShader)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    Lumin &lumin = *InArgs.luminPtr;
    auto data = lumin.GetFrameData(InArgs);
    
    // Set layer data
    SetValue(InShader, "CellSize", &data.cellSize, SHADER_UNIFORM_FLOAT, 1);
    SetValue(InShader, "ChunkSize", &data.chunkSize, SHADER_UNIFORM_FLOAT, 1);
}

void Rendering::LuminRenderer::ApplyLumin(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    // Bind textures
    BindBRDF(InArgs, InShader, InOutSlot);
    
    // Bind lumin texture from frame targets
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
    
    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.depthTest = true;
    shaderCmd.depthMask = true;
    rlState::current.Set(shaderCmd);
    
    SetFrame(InArgs, *shaderRes);
    
    int texSlot = 0;
    SetLuminValues(InArgs, *shaderRes);
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderRes, texSlot);

    int chunkTexSlot = texSlot;
    auto luminData = lumin.GetFrameData(InArgs);
    Vector<Mat4F> probes = lumin.GetDebugProbes();
    for (auto& chunk : luminData.visibleChunks)
    {
        SetValue(*shaderRes, "ChunkPosition", &chunk.position, SHADER_UNIFORM_VEC3);
        chunk.target->Bind(*shaderRes, chunkTexSlot);
        
        // Draw one chunk per call
        MeshCommand cmd;
        cmd.vaoID = mesh.vaoId;
        if (rlState::current.Set(cmd, probes))
        {
            // Draw every perspective
            for (auto& perspective : InArgs.perspectives)
            {
                PerspectiveCommand perspCmd;
                perspCmd.rect = perspective.targetRect;
                rlState::current.Set(perspCmd);
                SetPerspective(InArgs, perspective, InTarget, *shaderRes);
                DrawInstances(mesh, probes.size());
            }
        }
    }
    
    rlState::current.ResetMesh();
    return static_cast<int>(probes.size());
}

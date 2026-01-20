#include "LuminRenderer.h"

#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "Rendering/Utility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"

int Rendering::LuminRenderer::DrawChunks(const RenderArgs &InArgs, const ResShader& InShader, RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers, const LuminData& InData)
{
    ShaderResource* shaderRes = InShader.Get();
    CHECK_RETURN_LOG(!shaderRes, "Failed to get shader resource", 0);
    const Shader* shader = shaderRes->GetProgram();
    CHECK_RETURN_LOG(!shader, "Failed to get shader", 0);
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = true;
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    rlState::current.Set(shaderCmd);
    
    SetFrame(InArgs, *shaderRes);
    SetValue(*shaderRes, "CellSize", &InData.cellSize, SHADER_UNIFORM_VEC3, 1);
    SetValue(*shaderRes, "ChunkSize", &InData.chunkSize, SHADER_UNIFORM_VEC3, 1);
    
    int texSlot = 0;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderRes, texSlot);
    
    for (auto& chunk : InData.visibleChunks)
    {
        // Set chunk specific data
        int chunkTexSlot = texSlot;
        SetValue(*shaderRes, "ChunkPosition", &chunk.position, SHADER_UNIFORM_VEC3);
        chunk.targets03->Bind(*shaderRes, chunkTexSlot);
        chunk.targets48->Bind(*shaderRes, chunkTexSlot);
        
        for (auto& persp : InArgs.perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.rect = persp.targetRect;
            rlState::current.Set(perspCmd);
            SetPerspective(InArgs, persp, InTarget, *shaderRes);
            DrawQuad();
        }
    }
    
    return InData.visibleChunks.size() * InArgs.perspectives.size();
}

int Rendering::LuminRenderer::DrawLumin(const RenderArgs& InArgs, LuminTargets& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    auto& lumin = *InArgs.luminPtr;
    LuminConfig& conf = lumin.config;
    CHECK_RETURN(!conf.Enabled, {})
    
    auto luminData = lumin.GetFrameData(InArgs);
    int passes = 0;
    passes += DrawChunks(InArgs, conf.RadianceShader, InTarget.radianceTarget, InBuffers, luminData);
    passes += DrawChunks(InArgs, conf.IrradianceShader, InTarget.irradianceTarget, InBuffers, luminData);
    
    // Lumin has now been collected in those two shaders
    // Do some fancy blending?
    // And then, ofc, upscale!
    
    return passes;
}

void Rendering::LuminRenderer::BindBRDF(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    if (auto brdf = InArgs.luminPtr->config.TexBRDF.Get().Get())
        brdf->Get().Bind(InShader, InOutSlot, RL_TEXTURE_FILTER_LINEAR);
}

int Rendering::LuminRenderer::CollectSHCoefficients(const RenderArgs& InArgs, RenderTarget& InTarget, int InStartCoeff, int InEndCoeff, const Vector<RenderTarget*>& InBuffers)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    auto& lumin = *InArgs.luminPtr;
    
    LuminConfig& conf = lumin.config;
    CHECK_RETURN(!conf.Enabled, {})
    ShaderResource* shaderRes = conf.CollectShader.Get().Get();
    CHECK_RETURN_LOG(!shaderRes, "Failed to get shader resource", 0);
    const Shader* shader = shaderRes->GetProgram();
    CHECK_RETURN_LOG(!shader, "Failed to get shader", 0);
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = false;
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    rlState::current.Set(shaderCmd);
        
    SetFrame(InArgs, *shaderRes);
    SetValue(*shaderRes, "CoeffStart", &InStartCoeff, SHADER_UNIFORM_INT, 1 );
    SetValue(*shaderRes, "CoeffEnd", &InEndCoeff, SHADER_UNIFORM_INT, 1 );
        
    int texSlot = 0;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderRes, texSlot);
    
    for (auto& persp : InArgs.perspectives)
    {
        PerspectiveCommand perspCmd;
        perspCmd.rect = persp.targetRect;
        rlState::current.Set(perspCmd);
        SetPerspective(InArgs, persp, InTarget, *shaderRes);
        DrawQuad();
    }
    return InArgs.perspectives.size();
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
    const Shader* shader = shaderRes->GetProgram();
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
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderRes, texSlot);

    int chunkTexSlot = texSlot;
    auto luminData = lumin.GetFrameData(InArgs);
    SetValue(*shaderRes, "CellSize", &luminData.cellSize, UniformType::FLOAT);
    SetValue(*shaderRes, "ChunkSize", &luminData.chunkSize, UniformType::FLOAT);
    
    Vector<Mat4F> probes = lumin.GetDebugProbes();
    for (auto& chunk : luminData.visibleChunks)
    {
        SetValue(*shaderRes, "ChunkPosition", &chunk.position, UniformType::FLOAT);
        chunk.targets03->Bind(*shaderRes, chunkTexSlot);
        chunk.targets48->Bind(*shaderRes, chunkTexSlot);
        
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

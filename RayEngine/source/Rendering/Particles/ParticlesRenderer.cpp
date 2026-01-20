#include "ParticlesRenderer.h"

#include "Particles.h"
#include "Context/Context.h"
#include "Scene/Scene.h"
#include "Scene/Instances/ParticleInstance.h"
#include "State/State.h"

int Rendering::ParticlesRenderer::Simulate(const RenderArgs& InArgs, SwapTarget& InTarget, const ParticleInstance& InInstance)
{
    PROFILE_GL_NAMED("Particles");
    
    // Get resource
    Particle* res = InInstance.particle.Get();
    CHECK_RETURN(!res, 0);

    // Get sim shader
    ShaderResource* shaderResource = res->data.SimShader.Get().Get();
    CHECK_RETURN(!shaderResource, 0);
    const Shader* shader = shaderResource->GetProgram();
    CHECK_RETURN(!shader, 0);

    InTarget.Iterate();
    auto& target = InTarget.Curr();
    auto& prev = InTarget.Prev();
    
    FrameCommand frameCmd;
    frameCmd.fboID = target.GetFBO();
    frameCmd.size = target.Size();
    rlState::current.Set(frameCmd);
    
    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    rlState::current.Set(shaderCmd);
    
    SetFrame(InArgs, *shaderResource);
    float age = static_cast<float>(InArgs.contextPtr->Time() - InInstance.startTimestamp) / res->data.SystemLifetime.Get();
    SetValue(*shaderResource, "Age", &age, UniformType::FLOAT);
    
    int texSlot = 0;
    prev.Bind(*shaderResource, texSlot);
    res->BakedExpressions.Bind("TexExpr", *shaderResource, texSlot, TextureParamValue::LINEAR);
    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    
    PerspectiveCommand perspCmd;
    rlState::current.Set(perspCmd);
    DrawQuad();
    
    return 1;
}

int Rendering::ParticlesRenderer::DrawParticles(const RenderArgs& InArgs, const RenderTarget& InSceneTarget)
{
    CHECK_ASSERT(!InArgs.particlesPtr, "Invalid particles");

    Vector<ParticleInstanceData> instances = InArgs.particlesPtr->GetFrameInstances(InArgs);
    CHECK_RETURN(instances.empty(), 0);
    
    FrameCommand frameCmd;
    frameCmd.fboID = InSceneTarget.GetFBO();
    frameCmd.size = InSceneTarget.Size();
    rlState::current.Set(frameCmd);

    int count = 0;
    for (ParticleInstanceData& data : instances)
    {
        CHECK_CONTINUE(!(data.instance.mask & InArgs.cullMask));

        const ::Mesh* meshes = nullptr;
        int32 meshCount = 0;
        if (const auto resModel = data.instance.model.Get())
        {
            if (const auto rlModel = resModel->Get())
            {
                if (rlModel->meshCount > 0)
                {
                    meshes = rlModel->meshes;
                    meshCount = rlModel->meshCount;
                }
            }
        }
        CHECK_CONTINUE(!meshes);
        CHECK_CONTINUE(meshCount == 0);
        
        // Get resource
        Particle* res = data.instance.particle.Get();
        CHECK_RETURN(!res, 0);
        
        const MaterialResource* resMat = data.instance.material.Get();
        CHECK_CONTINUE(!resMat);
        ShaderResource* resShader = resMat->data.SurfaceShader.Get().Get();
        CHECK_CONTINUE(!resShader);
        Shader* shader = resShader->GetProgram();
        CHECK_CONTINUE(!shader);

        // Enable shader
        ShaderCommand shaderCmd;
        shaderCmd.id = shader->id;
        shaderCmd.locs = shader->locs;
        shaderCmd.backfaceCulling = !resMat->data.TwoSided;
        shaderCmd.depthTest = true;
        shaderCmd.depthMask = true;
        rlState::current.Set(shaderCmd);

        const int id = 1234; // TODO: Fix
        SetValue(*resShader, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
        float age = static_cast<float>(InArgs.contextPtr->Time() - data.instance.startTimestamp) / res->data.SystemLifetime.Get();
        SetValue(*resShader, "Age", &age, SHADER_UNIFORM_FLOAT);
        SetFrame(InArgs, *resShader);
        SetCustomShaderValues(*resShader);
        SetValue(*resShader, "SystemTransform", Mat4F::Transpose(data.instance.transform));

        int texSlot = 0;
        data.target->Bind(*resShader, texSlot);
        res->BakedExpressions.Bind("TexExpr", *resShader, texSlot, RL_TEXTURE_FILTER_LINEAR);
        BindNoiseTextures(InArgs, *resShader, texSlot);

        // Draw every mesh
        for (int i = 0; i < meshCount; i++)
        {
            PROFILE_GL_NAMED("Mesh");
            MeshCommand cmd;
            cmd.vaoID = meshes[i].vaoId;

            // Copy to GPU
            if (rlState::current.Set(cmd, { Mat4F() }))
            {
                for (auto& perspective : InArgs.perspectives)
                {
                    PROFILE_GL_NAMED("Perspective");
                    PerspectiveCommand perspCmd;
                    perspCmd.rect = perspective.targetRect;
                    SetPerspective(InArgs, perspective, InSceneTarget, *resShader);
                    rlState::current.Set(perspCmd);
                    count += DrawInstances(meshes[i], res->data.MaxInstances);
                }
                rlState::current.ResetMesh();
            }
        }

        // TODO: Inject deferred shader
    }
    return count;
}

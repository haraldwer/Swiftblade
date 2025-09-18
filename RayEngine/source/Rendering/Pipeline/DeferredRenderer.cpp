#include "DeferredRenderer.h"

#include "Lumin/LuminRenderer.h"
#include "Context/Context.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"

int Rendering::DeferredRenderer::DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget)
{
    PROFILE_GL();

    ModelResource* model = InArgs.contextPtr->config.DefaultCube.Get().Get();
    CHECK_RETURN(!model, 0);
    auto* modelRes = model->Get();
    CHECK_RETURN(!modelRes, 0);
    CHECK_RETURN(!modelRes->meshCount, 0);
    Mesh& mesh = modelRes->meshes[0];
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearDepth = true;
    frameCmd.clearTarget = true;
    rlState::current.Set(frameCmd);
    
    int c = 0;
    for (auto& environment : InArgs.scenePtr->environments)
    {
        MaterialResource* rm = environment.skybox.Get();
        CHECK_CONTINUE(!rm);
        ShaderResource* shaderResource = rm->data.SurfaceShader.Get().Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        ShaderCommand shaderCmd;
        shaderCmd.locs = shader->locs;
        shaderCmd.id = shader->id;
        rlState::current.Set(shaderCmd);

        int id = 0;
        SetValue(*shaderResource, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
        SetFrameShaderValues(InArgs, *shaderResource);
        SetValue(*shaderResource, "Bounds", &environment.shape, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Position", &environment.position, SHADER_UNIFORM_VEC3);

        int texSlot = 0;
        BindNoiseTextures(InArgs, *shaderResource, texSlot);

        MeshCommand cmd;
        cmd.vaoID = mesh.vaoId;
        if (rlState::current.Set(cmd, { environment.position }))
        {
            // Draw every perspective
            for (auto& perspective : InArgs.perspectives)
            {
                PerspectiveCommand perspCmd;
                perspCmd.rect = perspective.targetRect;
                rlState::current.Set(perspCmd);
                SetPerspectiveShaderValues(InArgs, perspective, InTarget, *shaderResource);
                c += DrawInstances(mesh, 1);
            }
            rlState::current.ResetMesh();
        }
    }
    return c;
}

Map<uint64, int> Rendering::DeferredRenderer::DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(InArgs.cullMask == 0, "Invalid mask");
    
    FrameCommand frameCmd;
    frameCmd.fboID = InSceneTarget.GetFBO();
    frameCmd.size = InSceneTarget.Size();
    frameCmd.clearDepth = true;
    rlState::current.Set(frameCmd);
    
    Map<uint64, int> count;
    for (auto& persistence : InArgs.scenePtr->meshes.GetEntries())
    {
        for (auto& entry : persistence.second)
        {
            PROFILE_GL_NAMED("Mesh entry");
            
            CHECK_CONTINUE(entry.second.transforms.Empty());
            CHECK_CONTINUE(!(entry.second.mask & InArgs.cullMask));
            
            const ::Mesh* meshes = nullptr;
            int32 meshCount = 0;
            if (const auto resModel = entry.second.model.Get())
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

            const MaterialResource* resMat = entry.second.material.Get();
            CHECK_CONTINUE(!resMat);
            ShaderResource* resShader = resMat->data.SurfaceShader.Get().Get();
            CHECK_CONTINUE(!resShader);
            Shader* shader = resShader->Get();
            CHECK_CONTINUE(!shader);

            // Enable shader
            ShaderCommand shaderCmd;
            shaderCmd.id = shader->id;
            shaderCmd.locs = shader->locs;
            shaderCmd.backfaceCulling = !resMat->data.TwoSided;
            shaderCmd.depthTest = true;
            shaderCmd.depthMask = true;
            rlState::current.Set(shaderCmd);

            const int id = static_cast<int32>(entry.second.deferredID);
            SetValue(*resShader, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
            SetFrameShaderValues(InArgs, *resShader);

            int texSlot = 0;
            BindNoiseTextures(InArgs, *resShader, texSlot);

            // Set values and textures from material or from model instance?
            SetCustomShaderValues(*resShader);

            // Draw every mesh
            for (int i = 0; i < meshCount; i++)
            {
                PROFILE_GL_NAMED("Mesh");
                MeshCommand cmd;
                cmd.vaoID = meshes[i].vaoId;

                // Culling
                const Vector<Mat4F>& data = entry.second.transforms.GetCulled(InArgs.cullPoints);

                // Copy to GPU
                if (rlState::current.Set(cmd, data))
                {
                    for (auto& perspective : InArgs.perspectives)
                    {
                        PROFILE_GL_NAMED("Perspective");
                        PerspectiveCommand perspCmd;
                        perspCmd.rect = perspective.targetRect;
                        SetPerspectiveShaderValues(InArgs, perspective, InSceneTarget, *resShader);
                        rlState::current.Set(perspCmd);
                        count[entry.first] += DrawInstances(meshes[i], static_cast<int>(data.size()));
                    }
                    rlState::current.ResetMesh();
                }
            }
        }
    }

    return count;
}

int Rendering::DeferredRenderer::DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_GL();
    
    auto& scene = *InArgs.scenePtr;
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = true;
    rlState::current.Set(frameCmd);

    Map<uint32, ResShader> passes = scene.meshes.GetDeferredShaders();

    // Inject skybox
    for (auto& environment : InArgs.scenePtr->environments)
    {
        MaterialResource* rm = environment.skybox.Get();
        CHECK_CONTINUE(!rm);
        passes[0] = rm->data.DeferredShader;
    }

    for (auto& entry : passes)
    {
        PROFILE_GL_NAMED("Deferred pass");
        
        ShaderResource* shaderResource = entry.second.Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        ShaderCommand shaderCmd;
        shaderCmd.locs = shader->locs;
        shaderCmd.id = shader->id;
        rlState::current.Set(shaderCmd);

        const int id = static_cast<int32>(entry.first);
        SetValue(*shaderResource, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
        SetFrameShaderValues(InArgs, *shaderResource);
        
        int texSlot = 0;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, texSlot);
        LuminRenderer::ApplyLumin(InArgs, *shaderResource, texSlot);
        BindNoiseTextures(InArgs, *shaderResource, texSlot);
        
        for (auto& perspective : InArgs.perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.rect = perspective.targetRect;
            rlState::current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, InTarget, *shaderResource);
            DrawQuad();
        }
    }
    return static_cast<int>(passes.size());
}

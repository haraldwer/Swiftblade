#include "LightsRenderer.h"

#include "Lumin/LuminRenderer.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"

int Rendering::LightsRenderer::DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.lightsPtr, "Invalid lightptr");
    auto& lightMan = *InArgs.lightsPtr;
    
    auto lights = lightMan.GetLights(InArgs);
    CHECK_RETURN(lights.empty(), 0);

    LightConfig conf = lightMan.config;
    
    // Get shader
    auto res = conf.LightShader;
    ShaderResource* shaderResource = res.Get().Get();
    CHECK_RETURN(!shaderResource, 0);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN(!shader, 0);

    const auto& debugRes = conf.DebugShader;
    ShaderResource* debugShaderResource = debugRes.Get().Get();
    CHECK_RETURN(!debugShaderResource, 0);
    const Shader* debugShader = debugShaderResource->Get();
    CHECK_RETURN(!debugShader, 0);

    auto model = conf.CubeModel.Get().Get();
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
    shaderCmd.blendMode = RL_BLEND_ADDITIVE;
    rlState::current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    SetValue(*shaderResource, "UpdateFrequency", &conf.UpdateFrequency.Get(), SHADER_UNIFORM_FLOAT);

    int texSlot = 0;
    auto& t = lightMan.GetShadowTarget();
    t.Bind(*shaderResource, texSlot, RL_TEXTURE_FILTER_LINEAR);
    Vec2F faceTexel = lightMan.GetFaceTexel();
    SetValue(*shaderResource, "FaceTexel", &faceTexel, SHADER_UNIFORM_VEC2);

    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);
    
    for (auto& light : lights)
    {
        PROFILE_GL_NAMED("Light");

        // TODO: Some lights shouldn't be rendered during Lumin!!
        
        CHECK_ASSERT(!light, "Invalid light");
    
        SetValue(*shaderResource, "Position", &light->data.position, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Direction", &light->data.direction, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Color", &light->data.color, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Range", &light->data.range, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "ConeRadius", &light->data.radius, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "Intensity", &light->data.intensity, SHADER_UNIFORM_FLOAT);

        auto find = lightMan.cache.find(light->id);
        LightData* shadowCache = find == lightMan.cache.end() ? nullptr : &find->second;

        if (shadowCache && light->shadows)
        {
            float time = static_cast<float>(shadowCache->timestamp);
            SetValue(*shaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
            SetValue(*shaderResource, "ShadowPosition", &shadowCache->pos, SHADER_UNIFORM_VEC3);
            SetValue(*shaderResource, "ShadowRect", &shadowCache->rect, SHADER_UNIFORM_VEC4);
        }
        else
        {
            float time = 0;
            SetValue(*shaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
        }

        for (auto& perspective : InArgs.perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.rect = perspective.targetRect;
            rlState::current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
            DrawQuad(); // TODO: Replace with cube mesh
        }
    }

    // Debug draw shadow maps
    if (conf.Debug)
    {
        PROFILE_GL_NAMED("Light debug");
        
        ShaderCommand debugShaderCmd;
        debugShaderCmd.locs = debugShader->locs;
        debugShaderCmd.id = debugShader->id;
        debugShaderCmd.depthTest = true;
        debugShaderCmd.depthMask = true;
        rlState::current.Set(debugShaderCmd);
        
        SetFrameShaderValues(InArgs, *debugShaderResource, InTarget);
        SetValue(*debugShaderResource, "UpdateFrequency", &conf.UpdateFrequency.Get(), SHADER_UNIFORM_FLOAT);

        int debugTexSlot = 0;
        t.Bind(*debugShaderResource, debugTexSlot);
        BindNoiseTextures(InArgs, *debugShaderResource, debugTexSlot);
        for (auto& b : InBuffers)
            if (b) b->Bind(*debugShaderResource, debugTexSlot);
        
        for (auto& light : lights)
        {
            PROFILE_GL_NAMED("Light cube debug");
            
            auto find = lightMan.cache.find(light->id);
            LightData* shadowCache = find == lightMan.cache.end() ? nullptr : &find->second;
            CHECK_CONTINUE(!shadowCache);
            CHECK_CONTINUE(!light->shadows);

            SetValue(*debugShaderResource, "Position", &light->data.position, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "Direction", &light->data.direction, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "Color", &light->data.color, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "Range", &light->data.range, SHADER_UNIFORM_FLOAT);
            SetValue(*debugShaderResource, "ConeRadius", &light->data.radius, SHADER_UNIFORM_FLOAT);
            SetValue(*debugShaderResource, "Intensity", &light->data.intensity, SHADER_UNIFORM_FLOAT);
            
            float time = static_cast<float>(shadowCache->timestamp);
            SetValue(*debugShaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
            SetValue(*debugShaderResource, "ShadowPosition", &shadowCache->pos, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "ShadowRect", &shadowCache->rect, SHADER_UNIFORM_VEC4);
            
            MeshCommand cmd;
            cmd.vaoID = mesh.vaoId;
            if (rlState::current.Set(cmd, { light->data.position }))
            {
                // Draw every perspective
                for (auto& perspective : InArgs.perspectives)
                {
                    PerspectiveCommand perspCmd;
                    perspCmd.rect = perspective.targetRect;
                    rlState::current.Set(perspCmd);
                    SetPerspectiveShaderValues(InArgs, perspective, *debugShaderResource);
                    DrawInstances(mesh, 1);
                }
                rlState::current.ResetMesh();
            }
        }
    }
    
    return static_cast<int>(lights.size());
}

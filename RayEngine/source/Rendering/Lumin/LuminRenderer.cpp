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
    LuminRenderData probeData = lumin.GetFrameProbes(InArgs);

    double time = InArgs.contextPtr->Time();
    
    // Collect probe data
    Vector<Vec4F> positions;
    Vector<Vec2F> rects;

    // Add fallback
    rects.emplace_back(probeData.fallback->rect.xy);
    positions.emplace_back(
        probeData.fallback->rect.x,
        probeData.fallback->rect.y,
        probeData.fallback->rect.z,
        static_cast<float>(time - probeData.fallback->atlasTimestamp)
    );
    Vec2F rectSize = {
        probeData.fallback->rect.z,
        probeData.fallback->rect.w
    };
    
    for (auto& probe : probeData.probes)
    {
        double timeFade = 0; // time since rendered
        if (probe->atlasTimestamp > 0) 
            timeFade = time - probe->atlasTimestamp;
        
        CHECK_CONTINUE(!probe);
        positions.emplace_back(
            probe->pos.x,
            probe->pos.y,
            probe->pos.z,
            timeFade
        );
        rects.push_back(probe->rect.xy);
    }
    
    // Collect layer data
    Vector<Vec3F> densities;
    Vector<Vec4I> sizes;
    Vector<Vec4I> starts;
    for (auto& layer : probeData.layers)
    {
        densities.push_back(layer.density);
        sizes.emplace_back(
            layer.size.x,
            layer.size.y,
            layer.size.z,
            layer.startIndex
        );
        starts.emplace_back(
            layer.start.x,
            layer.start.y,
            layer.start.z,
            layer.endIndex
        );
    }

    // Set layer data
    SetValue(InShader, "ProbeDensities", densities.data(), SHADER_UNIFORM_VEC3, static_cast<int>(densities.size()));
    SetValue(InShader, "ProbeSizes", sizes.data(), SHADER_UNIFORM_IVEC4, static_cast<int>(sizes.size()));
    SetValue(InShader, "ProbeStarts", starts.data(), SHADER_UNIFORM_IVEC4, static_cast<int>(starts.size()));

    // Set probe data
    SetValue(InShader, "ProbeIndices", probeData.indices.data(), SHADER_UNIFORM_INT, static_cast<int>(probeData.indices.size()));
    SetValue(InShader, "ProbePositions", positions.data(), SHADER_UNIFORM_VEC4, static_cast<int>(positions.size()));
    SetValue(InShader, "ProbeRects", rects.data(), SHADER_UNIFORM_VEC2, static_cast<int>(rects.size()));
    SetValue(InShader, "ProbeRectSize", &rectSize, SHADER_UNIFORM_VEC2);

    // Set probe texture data
    auto& target = lumin.GetProbeTarget();
    Vec2F texel = Vec2F::One() / target.Size().To<float>();
    SetValue(InShader, "ProbeFaceTexel", &texel, SHADER_UNIFORM_VEC2);

    Vec2F nearfar = Vec2F(lumin.config.Near, lumin.config.Far);
    SetValue(InShader, "ProbeNearFar", &nearfar, SHADER_UNIFORM_VEC2);
    SetValue(InShader, "ProbeOffset", &lumin.config.Offset.Get(), SHADER_UNIFORM_VEC3);

    // Bind textures
    target.Bind(InShader, InOutSlot, RL_TEXTURE_FILTER_LINEAR);
    BindBRDF(InArgs, InShader, InOutSlot);
}

void Rendering::LuminRenderer::BindBRDF(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    if (auto brdf = InArgs.luminPtr->config.TexBRDF.Get().Get())
        if (brdf->IsBaked())
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
    
    LuminRenderData probeData = lumin.GetFrameProbes(InArgs);
    auto& t = lumin.GetProbeTarget();
    
    const auto& debugRes = conf.DebugShader;
    ShaderResource* debugShaderResource = debugRes.Get().Get();
    CHECK_RETURN(!debugShaderResource, 0);
    const Shader* debugShader = debugShaderResource->Get();
    CHECK_RETURN(!debugShader, 0);

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
    debugShaderCmd.locs = debugShader->locs;
    debugShaderCmd.id = debugShader->id;
    debugShaderCmd.depthTest = true;
    debugShaderCmd.depthMask = true;
    rlState::current.Set(debugShaderCmd);
    
    SetFrameShaderValues(InArgs, *debugShaderResource, InTarget);

    Vec2F texel = Vec2F(1.0f) / t.Size().To<float>();
    SetValue(*debugShaderResource, "LuminTexel", &texel, SHADER_UNIFORM_VEC2);

    int debugTexSlot = 0;
    t.Bind(*debugShaderResource, debugTexSlot);
    BindNoiseTextures(InArgs, *debugShaderResource, debugTexSlot);
    for (auto& b : InBuffers)
        if (b) b->Bind(*debugShaderResource, debugTexSlot);

    MeshCommand cmd;
    cmd.vaoID = mesh.vaoId;

    int l = 0;
    int c = 0;
    for (auto& layer : probeData.layers)
    {
        l++;
        for (int i = layer.startIndex; i < layer.endIndex; i++)
        {
            int probeIndex = probeData.indices[i];
            CHECK_CONTINUE(probeIndex == -1);
            PROFILE_GL_NAMED("Lumin probe debug");
            
            LuminProbe* probe = probeData.probes[probeIndex];
            SetValue(*debugShaderResource, "ProbePosition", &probe->pos, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "ProbeRect", &probe->rect, SHADER_UNIFORM_VEC4);

            Mat4F mat = { probe->pos, QuatF::Identity(), Vec3F(l * l * 0.4f) };
            if (rlState::current.Set(cmd, { mat }))
            {
                // Draw every perspective
                for (auto& perspective : InArgs.perspectives)
                {
                    PerspectiveCommand perspCmd;
                    perspCmd.rect = perspective.targetRect;
                    rlState::current.Set(perspCmd);
                    SetPerspectiveShaderValues(InArgs, perspective, *debugShaderResource);
                    DrawInstances(mesh, 1);
                    c++;
                }
            }
        }
    }   
    rlState::current.ResetMesh();
    return c;
}

#include "Renderer.h"

#include "Context/Context.h"
#include "Core/Utility/RayUtility.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Viewport/Viewport.h"
#include "rlgl.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const void* InValue, const int InType)
{
    const int loc = InShader.GetLocation(InName);
    if (loc >= 0)
        rlSetUniform(loc, InValue, InType, 1);
}

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const Matrix& InValue)
{
    const int loc = InShader.GetLocation(InName);
    if (loc >= 0)
        rlSetUniformMatrix(loc, InValue);
}

void Rendering::Renderer::SetShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget, uint32 InDeferredID)
{
    auto& viewport = *InArgs.Viewport;
    auto& context = *InArgs.Context;
    auto& cam = InArgs.Camera; 
    
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    SetValue(InShader, "CameraPosition", &cam.Position.data[0], SHADER_UNIFORM_VEC3);
    Vec2F nearFar = { cam.Near, cam.Far };
    SetValue(InShader, "NearFar", &nearFar.data[0], SHADER_UNIFORM_VEC2);
    const float time = static_cast<float>(context.Timer.Ellapsed()); 
    SetValue(InShader, "Time", &time, SHADER_UNIFORM_FLOAT);
    const float delta = static_cast<float>(viewport.Delta); 
    SetValue(InShader, "Delta", &delta, SHADER_UNIFORM_FLOAT);
    SetValue(InShader, "Resolution", &InSceneTarget.Size().data[0], SHADER_UNIFORM_VEC2);
    const int id = static_cast<int32>(InDeferredID);
    SetValue(InShader, "DeferredID", &id, SHADER_UNIFORM_INT);
    SetValue(InShader, "WorldToScreen", Utility::Ray::ConvertMat(viewport.PendingMVP));
    SetValue(InShader, "WorldToPrevScreen", Utility::Ray::ConvertMat(viewport.PreviousMVP));
    SetValue(InShader, "ScreenToWorld", Utility::Ray::ConvertMat(Mat4F::GetInverse(viewport.PendingMVP)));
    
    SetNoiseTextures(InArgs, InShader);
}

void Rendering::Renderer::SetCustomShaderValues(ShaderResource& InShader)
{
    // TODO
}

void Rendering::Renderer::SetNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader)
{
    auto& config = InArgs.Context->Config;
    for (auto& entry : config.NoiseTextures.Get())
    {
        auto noiseRes = entry.second.Get();
        CHECK_CONTINUE(!noiseRes);
        auto texRes = noiseRes->Get().Get();
        CHECK_CONTINUE(!texRes);
        auto tex = texRes->Get();
        CHECK_CONTINUE(!tex);

        // Bind texture to slot!
        int loc = InShader.GetLocation(entry.first);
        CHECK_CONTINUE(loc < 0);
        
        rlTextureParameters(tex->id, RL_TEXTURE_MAG_FILTER, RL_TEXTURE_FILTER_LINEAR);
        rlTextureParameters(tex->id, RL_TEXTURE_MIN_FILTER, RL_TEXTURE_FILTER_LINEAR);
        
        rlActiveTextureSlot(loc);
        rlEnableTexture(tex->id);
        rlSetUniform(loc, &loc, RL_SHADER_UNIFORM_SAMPLER2D, 1);
        rlActiveTextureSlot(0);
    }
}

Map<uint64, int> Rendering::Renderer::DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget)
{
    rlEnableDepthTest();

    InSceneTarget.BeginWrite();
    BeginMode3D(Utility::Ray::ConvertCamera(InArgs.Camera));

    Mat4F view = Utility::Ray::ConvertBack(rlGetMatrixModelview());
    Mat4F proj = Utility::Ray::ConvertBack(rlGetMatrixProjection());
    InArgs.Viewport->PreviousMVP = InArgs.Viewport->PendingMVP;
    InArgs.Viewport->PendingMVP = view * proj;
    
    // Instanced rendering
    Map<uint64, int> count;
    for (auto& entry : InArgs.Scene->Meshes.Entries)
    {
        const ::Mesh* meshes = nullptr;
        int32 meshCount = 0;
        if (const auto resModel = entry.second.Model.Get())
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
        CHECK_CONTINUE(entry.second.Transforms.empty());
        
        const MaterialResource* resMat = entry.second.Material.Get();
        CHECK_CONTINUE(!resMat);
        ShaderResource* resShader = resMat->SurfaceShader.Get().Get();
        CHECK_CONTINUE(!resShader);
        Shader* shader = resShader->Get();
        CHECK_CONTINUE(!shader);

        resMat->TwoSided ? rlDisableBackfaceCulling() : rlEnableBackfaceCulling();
        resMat->Transparent ? rlDisableDepthTest() : rlEnableDepthTest();
        
        // Enable shader
        rlEnableShader(shader->id);
        SetShaderValues(InArgs, *resShader, InSceneTarget, entry.second.DeferredID);
        SetCustomShaderValues(*resShader);

        // Data has been prepared for this entry
        // Time to draw all the instances
        for (int i = 0; i < meshCount; i++)
        {
            count[entry.first] += static_cast<int>(entry.second.Transforms.size());
            RaylibRenderUtility::DrawInstances(meshes[i], *shader, entry.second.Transforms);
        }

        // Disable shader
        rlDisableShader();
    }
    
    EndMode3D();
    InSceneTarget.EndWrite();
    rlEnableBackfaceCulling();
    rlDisableDepthTest();
    
    return count;
}

int Rendering::Renderer::DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    auto& scene = *InArgs.Scene;
    
    InTarget.BeginWrite(-1, false);
    for (auto& entry : scene.Meshes.DeferredShaders)
    {
        ShaderResource* shaderResource = entry.second.Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        rlEnableShader(shader->id);
        SetShaderValues(InArgs, *shaderResource, InTarget, entry.first);
        
        RenderTarget::Slot bindOffset;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, bindOffset);

        rlLoadDrawQuad();
        
        RenderTarget::Slot unbindOffset;
        for (auto& b : InBuffers)
            if (b) b->Unbind(*shaderResource, unbindOffset);
        
        rlDisableShader();
    }
    InTarget.EndWrite();
    
    return static_cast<uint32>(scene.Meshes.DeferredShaders.size());
}

int Rendering::Renderer::DrawLuminProbes(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    auto probes = InArgs.Lumin->GetProbes(InArgs);
    CHECK_RETURN(probes.empty(), 0);

    LuminConfig luminConf = InArgs.Lumin->GetConfig();
    
    // Get shader
    auto res = luminConf.LightingShader;
    ShaderResource* shaderResource = res.Get().Get();
    CHECK_RETURN(!shaderResource, 0);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN(!shader, 0);

    const auto& debugRes = luminConf.DebugShader;
    ShaderResource* debugShaderResource = debugRes.Get().Get();
    CHECK_RETURN(!debugShaderResource, 0);
    const Shader* debugShader = debugShaderResource->Get();
    CHECK_RETURN(!debugShader, 0);

    auto model = luminConf.SphereModel.Get().Get();
    auto* modelRes = model->Get();
    CHECK_RETURN(!modelRes, 0);
    CHECK_RETURN(!modelRes->meshCount, 0);
    Mesh& mesh = modelRes->meshes[0];
    
    InTarget.BeginWrite(RL_BLEND_ADDITIVE, false);
    rlEnableShader(shader->id);
    SetShaderValues(InArgs, *shaderResource, InTarget);
    SetValue(*shaderResource, "ProbeDensity", &luminConf.Density.Get(), SHADER_UNIFORM_VEC3);
    for (auto& probe : probes)
    {
        // Also set probe position
        SetValue(*shaderResource, "ProbePosition", &probe->Pos, SHADER_UNIFORM_VEC3);

        RenderTarget::Slot bindOffset;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, bindOffset);
        probe->Target.Bind(*shaderResource, bindOffset);

        rlLoadDrawQuad();
        
        RenderTarget::Slot unbindOffset;
        for (auto& b : InBuffers)
            if (b) b->Unbind(*shaderResource, unbindOffset);    
        probe->Target.Unbind(*shaderResource, unbindOffset);
    }
    rlDisableShader();

    if (luminConf.Debug)
    {
        // Draw debug probes
        BeginMode3D(Utility::Ray::ConvertCamera(InArgs.Camera));
        rlDisableColorBlend();
        rlEnableDepthTest();
        rlEnableShader(debugShader->id);
        SetShaderValues(InArgs, *debugShaderResource, InTarget);
        SetValue(*debugShaderResource, "ProbeDensity", &luminConf.Density.Get(), SHADER_UNIFORM_VEC3);
        for (auto& probe : probes)
        {
            // Also set probe position
            SetValue(*debugShaderResource, "ProbePosition", &probe->Pos, SHADER_UNIFORM_VEC3);
            
            RenderTarget::Slot bindOffset;
            for (auto& b : InBuffers)
                if (b) b->Bind(*debugShaderResource, bindOffset);
            probe->Target.Bind(*debugShaderResource, bindOffset);
            
            RaylibRenderUtility::DrawInstances(mesh, *debugShader, { probe->Pos });
            
            RenderTarget::Slot unbindOffset;
            for (auto& b : InBuffers)
                if (b) b->Unbind(*debugShaderResource, unbindOffset);
            probe->Target.Unbind(*debugShaderResource, unbindOffset);
            
        }
        rlDisableShader();
        EndMode3D();
        rlDisableDepthTest();
    }

    InTarget.EndWrite();
    return probes.size();
}

int Rendering::Renderer::DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    CHECK_ASSERT(!InArgs.Context->LightsPtr, "Invalid lightptr");
    auto& lightMan = *InArgs.Context->LightsPtr;
    
    auto lights = lightMan.GetLights(InArgs);
    CHECK_RETURN(lights.empty(), 0);

    LightConfig conf = lightMan.Config;
    
    // Get shader
    auto res = conf.LightShader;
    ShaderResource* shaderResource = res.Get().Get();
    CHECK_RETURN(!shaderResource, 0);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN(!shader, 0);
    
    InTarget.BeginWrite(RL_BLEND_ADDITIVE, false);
    rlEnableShader(shader->id);
    SetShaderValues(InArgs, *shaderResource, InTarget);
    SetValue(*shaderResource, "UpdateFrequency", &conf.UpdateFrequency.Get(), SHADER_UNIFORM_FLOAT);

    for (auto& light : lights)
    {
        CHECK_ASSERT(!light, "Invalid light");
    
        SetValue(*shaderResource, "Position", &light->Data.Position, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Direction", &light->Data.Direction, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Color", &light->Data.Color, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Range", &light->Data.Range, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "ConeRadius", &light->Data.Radius, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "Intensity", &light->Data.Intensity, SHADER_UNIFORM_FLOAT);
        
        auto& cache = lightMan.Cache.at(light->ID);
        float time = static_cast<float>(cache.Timestamp);
        SetValue(*shaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "ShadowPosition", &cache.SamplePos, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "ShadowPositionPrev", &cache.PrevSamplePos, SHADER_UNIFORM_VEC3);

        RenderTarget::Slot bindOffset;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, bindOffset);

        if (light->Shadows && cache.Timestamp > 0.001f)
        {
            cache.Target.Curr().Bind(*shaderResource, bindOffset);
            cache.Target.Prev().Bind(*shaderResource, bindOffset, "Prev");
        }

        rlLoadDrawQuad();
        
        RenderTarget::Slot unbindOffset;
        for (auto& b : InBuffers)
            if (b) b->Unbind(*shaderResource, unbindOffset);

        if (light->Shadows && cache.Timestamp > 0.001f)
        {
            cache.Target.Curr().Unbind(*shaderResource, bindOffset);
            cache.Target.Prev().Unbind(*shaderResource, bindOffset, "Prev");
        }
    }
    rlDisableShader();

    InTarget.EndWrite();
    return lights.size();
}

int Rendering::Renderer::DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget)
{
    auto model = InArgs.Context->Config.DefaultCube.Get().Get();
    auto* modelRes = model->Get();
    CHECK_RETURN(!modelRes, 0);
    CHECK_RETURN(!modelRes->meshCount, 0);
    Mesh& mesh = modelRes->meshes[0];
    
    InTarget.BeginWrite(RL_BLEND_ALPHA);
    BeginMode3D(Utility::Ray::ConvertCamera(InArgs.Camera));
    rlDisableBackfaceCulling();
    int c = 0;
    for (auto& environment : InArgs.Scene->Environments)
    {
        MaterialResource* rm = environment.Skybox.Get();
        CHECK_CONTINUE(!rm);
        ShaderResource* shaderResource = rm->SurfaceShader.Get().Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);
        
        rlEnableShader(shader->id);
        SetShaderValues(InArgs, *shaderResource, InTarget);
        SetValue(*shaderResource, "Bounds", &environment.Shape, SHADER_UNIFORM_VEC3);
        RaylibRenderUtility::DrawInstances(mesh, *shader, { environment.Position });
        rlDisableShader();
        c++;
    }
    EndMode3D();
    InTarget.EndWrite();
    return c;
}

void Rendering::Renderer::DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend, bool InClear)
{
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    InTarget.BeginWrite(InBlend, InClear);
    rlEnableShader(shader->id);
    SetShaderValues(InArgs, *shaderResource, InTarget);
    
    RenderTarget::Slot bindOffset;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, bindOffset);
    rlLoadDrawQuad();

    RenderTarget::Slot unbindOffset;
    for (auto& b : InBuffers)
        if (b) b->Unbind(*shaderResource, unbindOffset);
    
    rlDisableShader();
    InTarget.EndWrite(); 
}

void Rendering::Renderer::DrawCubeFace(const RenderArgs& InArgs, const RenderTarget& InTarget, int InFaceIndex, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend, bool InClear)
{
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    InTarget.BeginWrite(InBlend, InClear, InFaceIndex);
    rlEnableShader(shader->id);
    SetShaderValues(InArgs, *shaderResource, InTarget);
    
    RenderTarget::Slot bindOffset;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, bindOffset);
    rlLoadDrawQuad();

    RenderTarget::Slot unbindOffset;
    for (auto& b : InBuffers)
        if (b) b->Unbind(*shaderResource, unbindOffset);
    
    rlDisableShader();
    InTarget.EndWrite();
}

int Rendering::Renderer::DrawDebug(const RenderArgs& InArgs)
{
    auto& scene = *InArgs.Scene;
    
    rlEnableColorBlend();
    
    BeginMode3D(Utility::Ray::ConvertCamera(InArgs.Camera));

    for (auto& shape : scene.DebugShapes)
    {
        switch (shape.Type)
        {
        case DebugShape::Type::SPHERE:
            DrawSphereWires(
                Utility::Ray::ConvertVec(shape.Pos),
                shape.Data.x,
                static_cast<int>(shape.Data.y),
                static_cast<int>(shape.Data.z),
                { shape.Color.r, shape.Color.g, shape.Color.b, shape.Color.a });
            break;
        case DebugShape::Type::BOX:
            DrawCubeWiresV(
                Utility::Ray::ConvertVec(shape.Pos),
                Utility::Ray::ConvertVec(shape.Data),
                { shape.Color.r, shape.Color.g, shape.Color.b, shape.Color.a });
            break;
        case DebugShape::Type::CAPSULE:
            const Vec3F dir = Mat4F(shape.Rot).Right() * shape.Data.y;
            const auto start = Utility::Ray::ConvertVec(shape.Pos + dir);
            const auto end = Utility::Ray::ConvertVec(shape.Pos - dir);
            DrawCapsuleWires(
                start,
                end,
                shape.Data.x,
                static_cast<int>(shape.Data.z),
                static_cast<int>(shape.Data.z) / 2,
                { shape.Color.r, shape.Color.g, shape.Color.b, shape.Color.a });
            break;
        }
    }

    for (auto& line : scene.DebugLines)
        DrawLine3D(
            Utility::Ray::ConvertVec(line.Start),
            Utility::Ray::ConvertVec(line.End),
            { line.Color.r, line.Color.g, line.Color.b, line.Color.a });

    EndMode3D();

    return static_cast<int>(scene.DebugShapes.size() + scene.DebugLines.size());
}

void Rendering::Renderer::Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer)
{
    BeginTextureMode(InTarget);
    
    // Flip and blip
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTarget.texture.width),
        -static_cast<float>(InTarget.texture.height)
    };

    CHECK_ASSERT(!InBuffer.GetTextures()[0].Tex, "Tex nullptr");
    DrawTextureRec(
        *InBuffer.GetTextures()[0].Tex,
        sourceRec,
        { 0, 0 }, 
        ::WHITE);
}

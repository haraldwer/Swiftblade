#include "Renderer.h"

#include "Context/Context.h"
#include "Core/Utility/RayUtility.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "State/Command.h"
#include "State/State.h"
#include "Viewport/Viewport.h"
#include "Resources/BakedTexture.h"

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const void* InValue, const int InType, const int InCount)
{
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set uniform")
    rlSetUniform(loc, InValue, InType, InCount);
}

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const Mat4F& InValue)
{
    const int loc = InShader.GetLocation(InName);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set matrix uniform")
    const Matrix m = Utility::Ray::ConvertMat(InValue);
    rlSetUniformMatrix(loc, m);
}

void Rendering::Renderer::SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const void* InValue, const int InType, const int InCount)
{
    const int loc = InShader.GetLocation(InLoc);
    CHECK_RETURN(loc < 0);
    PROFILE_GL_GPU("Set uniform")
    rlSetUniform(loc, InValue, InType, InCount);
}

void Rendering::Renderer::SetValue(const ShaderResource& InShader, const ShaderResource::DefaultLoc& InLoc, const Mat4F& InValue)
{
    const int loc = InShader.GetLocation(InLoc);
    CHECK_RETURN(loc < 0);
    const Matrix m = Utility::Ray::ConvertMat(InValue);
    rlSetUniformMatrix(loc, m);
}

void Rendering::Renderer::SetFrameShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget)
{
    PROFILE_GL();

    const auto& viewport = *InArgs.viewportPtr;
    auto& context = *InArgs.contextPtr;
    const auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    // Time
    const float time = static_cast<float>(context.timer.Ellapsed()); 
    SetValue(InShader, ShaderResource::DefaultLoc::TIME, &time, SHADER_UNIFORM_FLOAT);
    const float delta = static_cast<float>(viewport.delta); 
    SetValue(InShader, ShaderResource::DefaultLoc::DELTA, &delta, SHADER_UNIFORM_FLOAT);
    const Vec2I res = viewport.GetResolution();
    const Vec2F resf = { static_cast<float>(res.x), static_cast<float>(res.y) };
    SetValue(InShader, ShaderResource::DefaultLoc::RESOLUTION, &resf, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetPerspectiveShaderValues(const RenderArgs& InArgs, const Perspective& InPerspective, ShaderResource& InShader)
{
    PROFILE_GL();
    
    auto& viewport = *InArgs.viewportPtr;
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    const Vec2F res = viewport.GetResolution().To<float>();
    float w = InPerspective.targetRect.z > 0 ?
        static_cast<float>(InPerspective.targetRect.z) : res.x;
    float h = InPerspective.targetRect.w > 0 ?
        static_cast<float>(InPerspective.targetRect.w) : res.y;
    Vec4F rect = Vec4F{
        static_cast<float>(InPerspective.targetRect.x),
        static_cast<float>(InPerspective.targetRect.y),
        w, h };
    
    // Matrices
    Vec2F size = { rect.z, rect.w };
    Mat4F view = InPerspective.camera.GetViewMatrix();
    Mat4F proj = InPerspective.camera.GetProjectionMatrix(size);
    viewport.viewProj = Mat4F::GetInverse(view) * proj;
    
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ, Mat4F::Transpose(viewport.viewProj));
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ_PREV, Mat4F::Transpose(viewport.viewProjPrev));
    SetValue(InShader, ShaderResource::DefaultLoc::VIEW_PROJ_INV, Mat4F::Transpose(Mat4F::GetInverse(viewport.viewProj)));

    // Make resolution independent
    Vec4F refRect = InPerspective.referenceRect;
    rect.x /= res.x;
    rect.y /= res.y;
    rect.z /= res.x;
    rect.w /= res.y;
    refRect.x /= res.x;
    refRect.y /= res.y;
    refRect.z /= res.x;
    refRect.w /= res.y;
    
    SetValue(InShader, ShaderResource::DefaultLoc::RECT, &rect, SHADER_UNIFORM_VEC4);
    SetValue(InShader, ShaderResource::DefaultLoc::REF_RECT, &refRect, SHADER_UNIFORM_VEC4);

    // Camera and view
    SetValue(InShader, ShaderResource::DefaultLoc::CAMERA_POSITION, &InPerspective.camera.position, SHADER_UNIFORM_VEC3);
    Vec2F nearFar = { InPerspective.camera.near, InPerspective.camera.far };
    SetValue(InShader, ShaderResource::DefaultLoc::NEAR_FAR, &nearFar, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetCustomShaderValues(ShaderResource& InShader)
{
    // TODO
}

void Rendering::Renderer::BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    PROFILE_GL();
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    auto& config = InArgs.contextPtr->config;
    for (auto& entry : config.NoiseTextures.Get())
    {
        const int loc = InShader.GetLocation(entry.first);
        CHECK_CONTINUE(loc < 0);
        const auto noiseRes = entry.second.Get();
        CHECK_CONTINUE(!noiseRes);
        const auto texRes = noiseRes->Get().Get();
        CHECK_CONTINUE(!texRes);
        const auto tex = texRes->Get();
        CHECK_CONTINUE(!tex);
        
        InOutSlot++;
        TextureCommand cmd;
        cmd.shaderLoc = loc;
        cmd.id = tex->id;
        cmd.filter = RL_TEXTURE_FILTER_LINEAR;
        cmd.wrap = RL_TEXTURE_WRAP_REPEAT;
        rlState::current.Set(cmd, InOutSlot);
    }
}

int Rendering::Renderer::DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget)
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
        ShaderResource* shaderResource = rm->SurfaceShader.Get().Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        ShaderCommand shaderCmd;
        shaderCmd.locs = shader->locs;
        shaderCmd.id = shader->id;
        rlState::current.Set(shaderCmd);

        int id = 0;
        SetValue(*shaderResource, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
        SetFrameShaderValues(InArgs, *shaderResource, InTarget);
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
                SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
                c += DrawInstances(mesh, 1);
            }
            rlState::current.ResetMesh();
        }
    }
    return c;
}

Map<uint64, int> Rendering::Renderer::DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget)
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
            ShaderResource* resShader = resMat->SurfaceShader.Get().Get();
            CHECK_CONTINUE(!resShader);
            Shader* shader = resShader->Get();
            CHECK_CONTINUE(!shader);

            // Enable shader
            ShaderCommand shaderCmd;
            shaderCmd.id = shader->id;
            shaderCmd.locs = shader->locs;
            shaderCmd.backfaceCulling = !resMat->TwoSided;
            shaderCmd.depthTest = true;
            shaderCmd.depthMask = true;
            rlState::current.Set(shaderCmd);

            const int id = static_cast<int32>(entry.second.deferredID);
            SetValue(*resShader, ShaderResource::DefaultLoc::DEFERRED_ID, &id, SHADER_UNIFORM_INT);
            SetFrameShaderValues(InArgs, *resShader, InSceneTarget);

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
                        SetPerspectiveShaderValues(InArgs, perspective, *resShader);
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

void Rendering::Renderer::DrawQuad()
{
    rlState::current.ResetMesh();
    PROFILE_GL_GPU("Draw quad");
    rlLoadDrawQuad();
}


int Rendering::Renderer::DrawInstances(const Mesh& InMesh, int InCount)
{
    PROFILE_GL_GPU("Draw instances");
    if (InMesh.indices != nullptr)
        rlDrawVertexArrayElementsInstanced(0, InMesh.triangleCount * 3, nullptr, InCount);
    else
        rlDrawVertexArrayInstanced(0, InMesh.vertexCount, InCount);
    return InCount;
}

int Rendering::Renderer::DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_GL();
    
    auto brdf = InArgs.contextPtr->config.TexBRDF.Get().Get();
    if (brdf && !brdf->IsBaked())
        brdf->Bake();
    
    auto& scene = *InArgs.scenePtr;
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = true;
    rlState::current.Set(frameCmd);

    Map<uint32, ResShader> passes = scene.meshes.GetDeferredShaders();
    passes[0] = InArgs.contextPtr->config.DeferredSkyboxShader; // Inject skybox
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
        SetFrameShaderValues(InArgs, *shaderResource, InTarget);
        
        int texSlot = 0;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, texSlot);
        
        BindNoiseTextures(InArgs, *shaderResource, texSlot);

        if (brdf)
            brdf->Get().Bind(*shaderResource, texSlot);
        
        for (auto& perspective : InArgs.perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.rect = perspective.targetRect;
            rlState::current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
            DrawQuad();
        }
    }
    return static_cast<int>(passes.size());
}

int Rendering::Renderer::DrawLuminProbes(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.luminPtr, "Invalid luminptr");
    auto& lumin = *InArgs.luminPtr;
    
    auto probes = lumin.GetProbes(InArgs);
    CHECK_RETURN(probes.empty(), 0);

    LuminConfig conf = InArgs.luminPtr->config;
    
    // Get shader
    auto res = conf.LightingShader;
    ShaderResource* shaderResource = res.Get().Get();
    CHECK_RETURN(!shaderResource, 0);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN(!shader, 0);

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

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.blendMode = RL_BLEND_ADDITIVE;
    rlState::current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    
    int texSlot = 0;
    auto& t = lumin.GetProbeTarget();
    t.Bind(*shaderResource, texSlot, RL_TEXTURE_FILTER_LINEAR);

    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);

    // Collect
    int c = static_cast<int>(probes.size());
    Vector<float> timestamps;
    Vector<Vec3F> positions;
    Vector<Vec4F> rects;
    timestamps.reserve(c);
    positions.reserve(c);
    rects.reserve(c);
    for (auto& probe : probes)
    {
        timestamps.push_back(probe->timestamp);
        positions.push_back(probe->pos);
        rects.push_back(probe->rect);
    }

    float range = lumin.GetRange();
    SetValue(*shaderResource, "ProbeRange", &range, SHADER_UNIFORM_FLOAT);
    SetValue(*shaderResource, "ProbeCount", &c, SHADER_UNIFORM_INT);
    SetValue(*shaderResource, "Timestamp", timestamps.data(), SHADER_UNIFORM_FLOAT, c);
    SetValue(*shaderResource, "ProbePosition", positions.data(), SHADER_UNIFORM_VEC3, c);
    SetValue(*shaderResource, "ProbeRect", rects.data(), SHADER_UNIFORM_VEC4, c);
    
    for (auto& perspective : InArgs.perspectives)
    {
        PerspectiveCommand perspCmd;
        perspCmd.rect = perspective.targetRect;
        rlState::current.Set(perspCmd);
        SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
        DrawQuad(); // TODO: Replace with cube mesh
    }

    if (conf.Debug)
    {
        PROFILE_GL_NAMED("Lumin debug");
        ShaderCommand debugShaderCmd;
        debugShaderCmd.locs = debugShader->locs;
        debugShaderCmd.id = debugShader->id;
        debugShaderCmd.depthTest = true;
        debugShaderCmd.depthMask = true;
        rlState::current.Set(debugShaderCmd);
        
        SetFrameShaderValues(InArgs, *debugShaderResource, InTarget);
        SetValue(*debugShaderResource, "ProbeRange", &range, SHADER_UNIFORM_VEC3);

        int debugTexSlot = 0;
        t.Bind(*debugShaderResource, debugTexSlot);
        BindNoiseTextures(InArgs, *debugShaderResource, debugTexSlot);
        for (auto& b : InBuffers)
            if (b) b->Bind(*debugShaderResource, debugTexSlot);
        
        for (auto& probe : probes)
        {
            PROFILE_GL_NAMED("Lumin probe debug");
            
            SetValue(*debugShaderResource, "Timestamp", &probe->timestamp, SHADER_UNIFORM_FLOAT);
            SetValue(*debugShaderResource, "ProbePosition", &probe->pos, SHADER_UNIFORM_VEC3);
            SetValue(*debugShaderResource, "ProbeRect", &probe->rect, SHADER_UNIFORM_VEC4);
            
            MeshCommand cmd;
            cmd.vaoID = mesh.vaoId;
            if (rlState::current.Set(cmd, { probe->pos }))
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

    return static_cast<int>(probes.size());
}

int Rendering::Renderer::DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
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

void Rendering::Renderer::DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend, bool InClear)
{
    PROFILE_GL();
    
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.size = InTarget.Size();
    frameCmd.clearTarget = InClear;
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.blendMode = InBlend;
    rlState::current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    
    int texSlot = 0;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);
    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    
    for (auto& persp : InArgs.perspectives)
    {
        PerspectiveCommand perspCmd;
        perspCmd.rect = persp.targetRect;
        rlState::current.Set(perspCmd);
        SetPerspectiveShaderValues(InArgs, persp, *shaderResource);
        DrawQuad();
    }
}

int Rendering::Renderer::DrawDebug(const RenderArgs& InArgs)
{
    PROFILE_GL();
    
    auto& scene = *InArgs.scenePtr;
    rlState::current.Reset();

    BeginTextureMode(InArgs.viewportPtr->GetVirtualTarget());

    for (auto& persp : InArgs.perspectives)
    {
        BeginMode3D(Utility::Ray::ConvertCamera(persp.camera));

        const Vec2F res = InArgs.viewportPtr->GetResolution().To<float>();
        float w = persp.targetRect.z > 0 ?
            static_cast<float>(persp.targetRect.z) : res.x;
        float h = persp.targetRect.w > 0 ?
            static_cast<float>(persp.targetRect.w) : res.y;
        Vec4I rect = Vec4F({
            static_cast<float>(persp.targetRect.x),
            static_cast<float>(persp.targetRect.y),
            w, h }).To<int>();
        
        rlViewport(rect.x, rect.y, rect.z, rect.w);
        rlEnableSmoothLines();

        for (auto& shape : scene.debugShapes.GetCulled(InArgs.cullPoints))
        {
            switch (shape.type)
            {
            case DebugShape::Type::SPHERE:
                DrawSphereWires(
                    Utility::Ray::ConvertVec(shape.pos),
                    shape.data.x,
                    static_cast<int>(shape.data.y),
                    static_cast<int>(shape.data.z),
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            case DebugShape::Type::BOX:
                DrawCubeWiresV(
                    Utility::Ray::ConvertVec(shape.pos),
                    Utility::Ray::ConvertVec(shape.data),
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            case DebugShape::Type::CAPSULE:
                const Vec3F dir = Mat4F(shape.rot).Right() * shape.data.y;
                const auto start = Utility::Ray::ConvertVec(shape.pos + dir);
                const auto end = Utility::Ray::ConvertVec(shape.pos - dir);
                DrawCapsuleWires(
                    start,
                    end,
                    shape.data.x,
                    static_cast<int>(shape.data.z),
                    static_cast<int>(shape.data.z) / 2,
                    { shape.col.r, shape.col.g, shape.col.b, shape.col.a });
                break;
            }
        }

        for (auto& line : scene.debugLines.GetCulled(InArgs.cullPoints))
            DrawLine3D(
                Utility::Ray::ConvertVec(line.start),
                Utility::Ray::ConvertVec(line.end),
                { line.col.r, line.col.g, line.col.b, line.col.a });
        
        EndMode3D();
    }

    EndTextureMode();
    
    return static_cast<int>(scene.debugShapes.Count() + scene.debugLines.Count());
}

void Rendering::Renderer::Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer)
{
    PROFILE_GL();
    
    rlState::current.Reset();
    
    BeginTextureMode(InTarget);

    // Flip and blip
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTarget.texture.width),
        -static_cast<float>(InTarget.texture.height)
    };

    CHECK_ASSERT(!InBuffer.GetTextures()[0].tex, "Tex nullptr");
    DrawTextureRec(
        *InBuffer.GetTextures()[0].tex,
        sourceRec,
        { 0, 0 }, 
        ::WHITE);

    EndTextureMode();
}

bool Rendering::Renderer::Bake(const BakedTexture& InTex)
{
    rlState::current.Reset();
    
    ShaderResource* shaderResource = InTex.Shader.Get().Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource", false);
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader", false);

    FrameCommand frameCmd;
    frameCmd.fboID = InTex.target.GetFBO();
    frameCmd.size = InTex.target.Size();
    rlState::current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.locs = shader->locs;
    shaderCmd.id = shader->id;
    shaderCmd.blendMode = -1;
    rlState::current.Set(shaderCmd);

    Vec4F rect = Vec4F(0, 0, 1, 1);
    SetValue(*shaderResource, ShaderResource::DefaultLoc::RECT, &rect, SHADER_UNIFORM_VEC4);
    
    DrawQuad();

    rlState::current.Reset();
    
    return true;
}

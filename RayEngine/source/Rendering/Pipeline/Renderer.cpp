#include "Renderer.h"

#include "Context/Context.h"
#include "Core/Utility/RayUtility.h"
#include "RayRenderUtility.h"
#include "Scene/Scene.h"
#include "Viewport/Viewport.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "State/Command.h"
#include "State/State.h"

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const void* InValue, const int InType)
{
    const int loc = InShader.GetLocation(InName);
    if (loc >= 0)
        rlSetUniform(loc, InValue, InType, 1);
}

void Rendering::Renderer::SetValue(ShaderResource& InShader, const String& InName, const Mat4F& InValue)
{
    const int loc = InShader.GetLocation(InName);
    if (loc >= 0)
    {
        Matrix m = Utility::Ray::ConvertMat(InValue);
        rlSetUniformMatrix(loc, m);
    }
}

void Rendering::Renderer::SetFrameShaderValues(const RenderArgs& InArgs, ShaderResource& InShader, const RenderTarget& InSceneTarget)
{
    auto& viewport = *InArgs.Viewport;
    auto& context = *InArgs.Context;
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    // Time
    const float time = static_cast<float>(context.Timer.Ellapsed()); 
    SetValue(InShader, "Time", &time, SHADER_UNIFORM_FLOAT);
    const float delta = static_cast<float>(viewport.Delta); 
    SetValue(InShader, "Delta", &delta, SHADER_UNIFORM_FLOAT);
    const Vec2I res = viewport.GetResolution();
    const Vec2F resf = { static_cast<float>(res.x), static_cast<float>(res.y) };
    SetValue(InShader, "Resolution", &resf, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetPerspectiveShaderValues(const RenderArgs& InArgs, const Perspective& InPerspective, ShaderResource& InShader)
{
    auto& viewport = *InArgs.Viewport;
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    const Vec2I res = viewport.GetResolution();
    int w = InPerspective.TargetRect.z > 0 ? InPerspective.TargetRect.z : res.x;
    int h = InPerspective.TargetRect.w > 0 ? InPerspective.TargetRect.w : res.y;
    Vec4F rect = {
        static_cast<float>(InPerspective.TargetRect.x),
        static_cast<float>(InPerspective.TargetRect.y),
        static_cast<float>(w),
        static_cast<float>(h)
    };
    
    SetValue(InShader, "Rect", &rect, SHADER_UNIFORM_VEC4);
    
    // Matrices
    Vec2F size = { rect.z, rect.w };
    Mat4F view = InPerspective.Camera.GetViewMatrix();
    Mat4F proj = InPerspective.Camera.GetProjectionMatrix(size);
    viewport.ViewProj = Mat4F::Transpose(Mat4F::GetInverse(view) * proj);
    
    SetValue(InShader, "ViewProj", viewport.ViewProj);
    SetValue(InShader, "ViewProjPrev", viewport.ViewProjPrev);
    SetValue(InShader, "ViewProjInv", Mat4F::GetInverse(viewport.ViewProj));

    // Camera and view
    SetValue(InShader, "CameraPosition", &InPerspective.Camera.Position, SHADER_UNIFORM_VEC3);
    Vec2F nearFar = { InPerspective.Camera.Near, InPerspective.Camera.Far };
    SetValue(InShader, "NearFar", &nearFar, SHADER_UNIFORM_VEC2);
}

void Rendering::Renderer::SetCustomShaderValues(ShaderResource& InShader)
{
    // TODO
}

void Rendering::Renderer::BindNoiseTextures(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot)
{
    auto& config = InArgs.Context->Config;
    for (auto& entry : config.NoiseTextures.Get())
    {
        int loc = InShader.GetLocation(entry.first);
        CHECK_CONTINUE(loc < 0);
        auto noiseRes = entry.second.Get();
        CHECK_CONTINUE(!noiseRes);
        auto texRes = noiseRes->Get().Get();
        CHECK_CONTINUE(!texRes);
        auto tex = texRes->Get();
        CHECK_CONTINUE(!tex);
        
        InOutSlot++;
        TextureCommand cmd;
        cmd.ShaderLoc = loc;
        cmd.ID = tex->id;
        rlState::Current.Set(cmd, InOutSlot);
    }
}

Map<uint64, int> Rendering::Renderer::DrawScene(const RenderArgs& InArgs, RenderTarget& InSceneTarget)
{
    FrameCommand frameCmd;
    frameCmd.fboID = InSceneTarget.GetFBO();
    frameCmd.Size = InSceneTarget.Size();
    frameCmd.Clear = true;
    rlState::Current.Set(frameCmd);
    
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

        // Enable shader
        ShaderCommand shaderCmd;
        shaderCmd.ID = shader->id;
        shaderCmd.Locs = shader->locs;
        shaderCmd.BackfaceCulling = !resMat->TwoSided;
        shaderCmd.DepthTest = true;
        shaderCmd.DepthMask = true;
        rlState::Current.Set(shaderCmd);

        const int id = static_cast<int32>(entry.second.DeferredID);
        SetValue(*resShader, "DeferredID", &id, SHADER_UNIFORM_INT);
        SetFrameShaderValues(InArgs, *resShader, InSceneTarget);

        int texSlot = 0;
        BindNoiseTextures(InArgs, *resShader, texSlot);

        // Set values and textures from material or from model instance?
        SetCustomShaderValues(*resShader);

        // Draw every mesh
        for (int i = 0; i < meshCount; i++)
        {
            MeshCommand cmd;
            cmd.vaoID = meshes[i].vaoId;
            if (rlState::Current.Set(cmd, entry.second.Transforms))
            {
                // From every perspective
                for (auto& perspective : InArgs.Perspectives)
                {
                    PerspectiveCommand perspCmd;
                    perspCmd.Rect = perspective.TargetRect;
                    SetPerspectiveShaderValues(InArgs, perspective, *resShader);
                    rlState::Current.Set(perspCmd);
                    count[entry.first] += DrawInstances(meshes[i], static_cast<int>(entry.second.Transforms.size()));
                }
                rlState::Current.ResetMesh();
            }
        }
    }

    return count;
}

void Rendering::Renderer::DrawQuad()
{
    rlState::Current.ResetMesh();
    rlLoadDrawQuad();
}

int Rendering::Renderer::DrawInstances(const Mesh& InMesh, int InNum)
{
    if (InMesh.indices != nullptr)
        rlDrawVertexArrayElementsInstanced(0, InMesh.triangleCount * 3, nullptr, InNum);
    else
        rlDrawVertexArrayInstanced(0, InMesh.vertexCount, InNum);
    return InNum;
}

int Rendering::Renderer::DrawDeferredScene(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    auto& scene = *InArgs.Scene;

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.Size = InTarget.Size();
    rlState::Current.Set(frameCmd);
    
    for (auto& entry : scene.Meshes.DeferredShaders)
    {
        ShaderResource* shaderResource = entry.second.Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        ShaderCommand shaderCmd;
        shaderCmd.Locs = shader->locs;
        shaderCmd.ID = shader->id;
        rlState::Current.Set(shaderCmd);

        const int id = static_cast<int32>(entry.first);
        SetValue(*shaderResource, "DeferredID", &id, SHADER_UNIFORM_INT);
        SetFrameShaderValues(InArgs, *shaderResource, InTarget);
        
        int texSlot = 0;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, texSlot);
        BindNoiseTextures(InArgs, *shaderResource, texSlot);

        for (auto& perspective : InArgs.Perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.Rect = perspective.TargetRect;
            rlState::Current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
            DrawQuad();
        }
    }
    return static_cast<int>(scene.Meshes.DeferredShaders.size());
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

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.Size = InTarget.Size();
    rlState::Current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.Locs = shader->locs;
    shaderCmd.ID = shader->id;
    shaderCmd.BlendMode = RL_BLEND_ADDITIVE;
    rlState::Current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    SetValue(*shaderResource, "ProbeDensity", &luminConf.Density.Get(), SHADER_UNIFORM_VEC3);

    for (auto& probe : probes)
    {
        // Also set probe position
        SetValue(*shaderResource, "ProbePosition", &probe->Pos, SHADER_UNIFORM_VEC3);

        int texSlot = 0;
        for (auto& b : InBuffers)
            if (b) b->Bind(*shaderResource, texSlot);
        probe->Target.Bind(*shaderResource, texSlot);

        DrawQuad();
    }

    if (luminConf.Debug)
    {
        ShaderCommand debugShaderCmd;
        debugShaderCmd.Locs = debugShader->locs;
        debugShaderCmd.ID = debugShader->id;
        debugShaderCmd.DepthTest = true;
        debugShaderCmd.DepthMask = true;
        rlState::Current.Set(debugShaderCmd);
        
        SetFrameShaderValues(InArgs, *debugShaderResource, InTarget);
        SetValue(*debugShaderResource, "ProbeDensity", &luminConf.Density.Get(), SHADER_UNIFORM_VEC3);

        for (auto& probe : probes)
        {
            // Also set probe position
            SetValue(*debugShaderResource, "ProbePosition", &probe->Pos, SHADER_UNIFORM_VEC3);
            
            int texSlot = 0;
            for (auto& b : InBuffers)
                if (b) b->Bind(*debugShaderResource, texSlot);
            probe->Target.Bind(*debugShaderResource, texSlot);
            
            MeshCommand cmd;
            cmd.vaoID = mesh.vaoId;
            if (rlState::Current.Set(cmd, { probe->Pos }))
            {
                // Draw every perspective
                for (auto& perspective : InArgs.Perspectives)
                {
                    PerspectiveCommand perspCmd;
                    perspCmd.Rect = perspective.TargetRect;
                    rlState::Current.Set(perspCmd);
                    SetPerspectiveShaderValues(InArgs, perspective, *debugShaderResource);
                    DrawInstances(mesh, 1);
                }
                rlState::Current.ResetMesh();
            }
        }
    }

    return static_cast<int>(probes.size());
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

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.Size = InTarget.Size();
    rlState::Current.Set(frameCmd);
    
    ShaderCommand shaderCmd;
    shaderCmd.Locs = shader->locs;
    shaderCmd.ID = shader->id;
    shaderCmd.BlendMode = RL_BLEND_ALPHA;
    rlState::Current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    SetValue(*shaderResource, "UpdateFrequency", &conf.UpdateFrequency.Get(), SHADER_UNIFORM_FLOAT);

    int texSlot = 0;
    auto& t = lightMan.GetShadowTarget();
    t.Curr().Bind(*shaderResource, texSlot);
    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);
    
    for (auto& light : lights)
    {
        CHECK_ASSERT(!light, "Invalid light");
    
        SetValue(*shaderResource, "Position", &light->Data.Position, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Direction", &light->Data.Direction, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Color", &light->Data.Color, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Range", &light->Data.Range, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "ConeRadius", &light->Data.Radius, SHADER_UNIFORM_FLOAT);
        SetValue(*shaderResource, "Intensity", &light->Data.Intensity, SHADER_UNIFORM_FLOAT);

        auto find = lightMan.Cache.find(light->ID);
        LightData* shadowCache = find == lightMan.Cache.end() ? nullptr : &find->second;

        if (shadowCache && light->Shadows)
        {
            auto& cache = lightMan.Cache.at(light->ID);
            float time = static_cast<float>(cache.Timestamp);
            SetValue(*shaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
            SetValue(*shaderResource, "ShadowPosition", &cache.SamplePos, SHADER_UNIFORM_VEC3);
            SetValue(*shaderResource, "ShadowPositionPrev", &cache.PrevSamplePos, SHADER_UNIFORM_VEC3);
            SetValue(*shaderResource, "ShadowRect", &cache.Rect, SHADER_UNIFORM_VEC4);
        }
        else
        {
            float time = 0;
            SetValue(*shaderResource, "Timestamp", &time, SHADER_UNIFORM_FLOAT);
        }

        for (auto& perspective : InArgs.Perspectives)
        {
            PerspectiveCommand perspCmd;
            perspCmd.Rect = perspective.TargetRect;
            rlState::Current.Set(perspCmd);
            SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
            DrawQuad(); // TODO: Replace with cube mesh
        }
    }
    return static_cast<int>(lights.size());
}

int Rendering::Renderer::DrawSkyboxes(const RenderArgs& InArgs, const RenderTarget& InTarget)
{
    auto model = InArgs.Context->Config.DefaultCube.Get().Get();
    CHECK_RETURN(!model, 0);
    auto* modelRes = model->Get();
    CHECK_RETURN(!modelRes, 0);
    CHECK_RETURN(!modelRes->meshCount, 0);
    Mesh& mesh = modelRes->meshes[0];
    
    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.Size = InTarget.Size();
    frameCmd.Clear = true;
    rlState::Current.Set(frameCmd);
    
    int c = 0;
    for (auto& environment : InArgs.Scene->Environments)
    {
        MaterialResource* rm = environment.Skybox.Get();
        CHECK_CONTINUE(!rm);
        ShaderResource* shaderResource = rm->SurfaceShader.Get().Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        ShaderCommand shaderCmd;
        shaderCmd.Locs = shader->locs;
        shaderCmd.ID = shader->id;
        shaderCmd.BlendMode = RL_BLEND_ALPHA;
        rlState::Current.Set(shaderCmd);
        
        SetFrameShaderValues(InArgs, *shaderResource, InTarget);
        SetValue(*shaderResource, "Bounds", &environment.Shape, SHADER_UNIFORM_VEC3);
        SetValue(*shaderResource, "Position", &environment.Position, SHADER_UNIFORM_VEC3);

        int texSlot = 0;
        BindNoiseTextures(InArgs, *shaderResource, texSlot);

        MeshCommand cmd;
        cmd.vaoID = mesh.vaoId;
        if (rlState::Current.Set(cmd, { environment.Position }))
        {
            // Draw every perspective
            for (auto& perspective : InArgs.Perspectives)
            {
                PerspectiveCommand perspCmd;
                perspCmd.Rect = perspective.TargetRect;
                rlState::Current.Set(perspCmd);
                SetPerspectiveShaderValues(InArgs, perspective, *shaderResource);
                c += DrawInstances(mesh, 1);
            }
            rlState::Current.ResetMesh();
        }
    }
    return c;
}

void Rendering::Renderer::DrawFullscreen(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, int InBlend, bool InClear)
{
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    FrameCommand frameCmd;
    frameCmd.fboID = InTarget.GetFBO();
    frameCmd.Size = InTarget.Size();
    frameCmd.Clear = InClear;
    rlState::Current.Set(frameCmd);

    ShaderCommand shaderCmd;
    shaderCmd.Locs = shader->locs;
    shaderCmd.ID = shader->id;
    shaderCmd.BlendMode = InBlend;
    rlState::Current.Set(shaderCmd);
    
    SetFrameShaderValues(InArgs, *shaderResource, InTarget);
    
    int texSlot = 0;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, texSlot);
    BindNoiseTextures(InArgs, *shaderResource, texSlot);
    
    for (auto& persp : InArgs.Perspectives)
    {
        PerspectiveCommand perspCmd;
        perspCmd.Rect = persp.TargetRect;
        rlState::Current.Set(perspCmd);
        SetPerspectiveShaderValues(InArgs, persp, *shaderResource);
        DrawQuad();
    }
}

int Rendering::Renderer::DrawDebug(const RenderArgs& InArgs)
{
    auto& scene = *InArgs.Scene;
    if (scene.DebugShapes.empty())
        return 0;

    rlState::Current.Reset();

    for (auto& persp : InArgs.Perspectives)
    {
        BeginMode3D(Utility::Ray::ConvertCamera(persp.Camera));
        
        rlViewport(
            persp.TargetRect.x,
            persp.TargetRect.y,
            persp.TargetRect.z,
            persp.TargetRect.w);
        
        rlEnableSmoothLines();

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
    }

    return static_cast<int>(scene.DebugShapes.size() + scene.DebugLines.size());
}

void Rendering::Renderer::Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer)
{
    rlState::Current.Reset();
    
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

    EndTextureMode();
}

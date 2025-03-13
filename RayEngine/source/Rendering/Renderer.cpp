#include "Renderer.h"

#include "RayRenderUtility.h"
#include "Core/Utility/RayUtility.h"
#include "rlgl.h"

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

void Rendering::Renderer::SetShaderValues(ShaderResource& InShader, const RenderScene& InScene, const RenderTarget& InSceneTarget, uint32 InDeferredID) const
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    SetValue(InShader, "CameraPosition", &InScene.Cam.Position.data[0], SHADER_UNIFORM_VEC3);
    Vec2F nearFar = { InScene.Cam.Near, InScene.Cam.Far };
    SetValue(InShader, "NearFar", &nearFar.data[0], SHADER_UNIFORM_VEC2);
    const float time = static_cast<float>(InScene.Time); 
    SetValue(InShader, "Time", &time, SHADER_UNIFORM_FLOAT);
    const float delta = static_cast<float>(InScene.Delta); 
    SetValue(InShader, "Delta", &delta, SHADER_UNIFORM_FLOAT);
    SetValue(InShader, "Resolution", &InSceneTarget.Size().data[0], SHADER_UNIFORM_VEC2);
    const int id = static_cast<int32>(InDeferredID);
    SetValue(InShader, "DeferredID", &id, SHADER_UNIFORM_INT);
    SetValue(InShader, "WorldToScreen", Utility::Ray::ConvertMat(PendingMVP));
    SetValue(InShader, "WorldToPrevScreen", Utility::Ray::ConvertMat(PreviousMVP));
    SetValue(InShader, "ScreenToWorld", Utility::Ray::ConvertMat(Mat4F::GetInverse(PendingMVP)));
}

void Rendering::Renderer::SetCustomShaderValues(ShaderResource& InShader) const
{
}

Map<uint64, int> Rendering::Renderer::DrawScene(const RenderScene& InScene, RenderTarget& InSceneTarget)
{
    rlEnableDepthTest();
    
    InSceneTarget.BeginWrite();
    BeginMode3D(Utility::Ray::ConvertCamera(InScene.Cam));

    Mat4F view = Utility::Ray::ConvertBack(rlGetMatrixModelview());
    Mat4F proj = Utility::Ray::ConvertBack(rlGetMatrixProjection());
    PreviousMVP = PendingMVP;
    PendingMVP = view * proj;
    
    // Instanced rendering
    Map<uint64, int> count;
    for (auto& entry : InScene.Meshes.Entries)
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
        //resMat->Transparent ? rlDisableDepthMask() : rlEnableDepthMask();
        // rlEnableWireMode?
        // rlEnablePointMode?
        // rlEnableSmoothLines?

        // Enable shader
        rlEnableShader(shader->id);
        SetShaderValues(*resShader, InScene, InSceneTarget, entry.second.DeferredID);
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

int Rendering::Renderer::DrawDeferredScene(const RenderScene& InScene, const RenderTarget& InTarget,
                                           const Vector<RenderTarget*>& InBuffers) const
{
    InTarget.BeginWrite();
    for (auto& entry : InScene.Meshes.DeferredShaders)
    {
        ShaderResource* shaderResource = entry.second.Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);

        rlEnableShader(shader->id);
        SetShaderValues(*shaderResource, InScene, InTarget, entry.first);
        
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
    
    return static_cast<uint32>(InScene.Meshes.DeferredShaders.size());
}

void Rendering::Renderer::DrawFullscreen(const RenderScene& InScene, const RenderTarget& InTarget, const ResShader& InShader, const Vector<RenderTarget*>& InBuffers, const Vector<RenderTarget*>& InPrevBuffers, int InBlend, bool InClear) const
{
    ShaderResource* shaderResource = InShader.Get();
    CHECK_RETURN_LOG(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_RETURN_LOG(!shader, "Failed to get shader");

    InTarget.BeginWrite(InBlend, InClear);
    rlEnableShader(shader->id);
    SetShaderValues(*shaderResource, InScene, InTarget, 0);
    
    RenderTarget::Slot bindOffset;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, bindOffset);
    for (auto& b : InPrevBuffers)
        if (b) b->Bind(*shaderResource, bindOffset, "Prev");

    rlLoadDrawQuad();

    RenderTarget::Slot unbindOffset;
    for (auto& b : InBuffers)
        if (b) b->Unbind(*shaderResource, unbindOffset);
    for (auto& b : InPrevBuffers)
        if (b) b->Unbind(*shaderResource, unbindOffset, "Prev");
    
    rlDisableShader();
    InTarget.EndWrite(); 
}

int Rendering::Renderer::DrawDebug(const RenderScene& InScene)
{
    rlEnableColorBlend();
    
    BeginMode3D(Utility::Ray::ConvertCamera(InScene.Cam));

    for (auto& shape : InScene.DebugShapes)
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

    for (auto& line : InScene.DebugLines)
        DrawLine3D(
            Utility::Ray::ConvertVec(line.Start),
            Utility::Ray::ConvertVec(line.End),
            { line.Color.r, line.Color.g, line.Color.b, line.Color.a });

    EndMode3D();

    return static_cast<int>(InScene.DebugShapes.size() + InScene.DebugLines.size());
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

    CHECK_ASSERT(!InBuffer.GetBuffers()[0].Tex, "Tex nullptr");
    DrawTextureRec(
        *InBuffer.GetBuffers()[0].Tex,
        sourceRec,
        { 0, 0 },
        ::WHITE);
}

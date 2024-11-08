#include "SceneRenderer.h"

#include "Engine/Profiling/Profile.h"
#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "RaylibRenderUtility.h"
#include "Utility/RayUtility.h"

void Rendering::SceneRenderer::Init()
{
    SSAOShader = ResShader("Shaders/SH_SSAO.ps"); 
    QuantizeShader = ResShader("Shaders/SH_Quantize.ps");
}

void Rendering::SceneRenderer::Setup(const RenderTexture2D& InVirtualTarget)
{
    if (SceneTarget.TryBeginSetup(InVirtualTarget))
    {
        SceneTarget.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
        SceneTarget.CreateBuffer("TexScreenPosition", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
        SceneTarget.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
        SceneTarget.CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
        SceneTarget.CreateBuffer("TexDeferredData", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
        SceneTarget.EndSetup(InVirtualTarget);
    }

    if (FrameTarget.TryBeginSetup(InVirtualTarget))
    {
        FrameTarget.CreateBuffer("TexFrame", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
        FrameTarget.EndSetup(InVirtualTarget);
    }
    
    if (QuantizeTarget.TryBeginSetup(InVirtualTarget))
    {
        QuantizeTarget.CreateBuffer("TexQuantize", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
        QuantizeTarget.EndSetup(InVirtualTarget);
    }
    
    for (auto& target : SSAOTargets)
    {
        if (target.TryBeginSetup(InVirtualTarget))
        {
            target.CreateBuffer("TexAO", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
            target.EndSetup(InVirtualTarget);
        }
    }
}

void Rendering::SceneRenderer::Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget)
{
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Scene target composing with multiple targets
    // TODO: Customizable render pipeline

    rlEnableDepthTest();
    
    Setup(InVirtualTarget);    

    // Draw to SceneTarget
    DrawEntries(InScene, SceneTarget);
    
    rlDisableDepthTest();
    
    // Collect AO
    auto& prevSSAOTarget = SSAOTargets[CurrentSSAOTarget];
    CurrentSSAOTarget = (CurrentSSAOTarget + 1) % 2;
    auto& newSSAOTarget = SSAOTargets[CurrentSSAOTarget];
    DrawPostProcessing(InScene, newSSAOTarget, { &SceneTarget, &prevSSAOTarget }, SSAOShader);
    
    // Draw to FrameTarget, quantize
    DrawDeferredScene(InScene, FrameTarget, { &SceneTarget, &newSSAOTarget });
    DrawPostProcessing(InScene, QuantizeTarget, { &SceneTarget, &newSSAOTarget, &FrameTarget }, QuantizeShader);

    rlEnableDepthTest();
    
    BeginTextureMode(InVirtualTarget);
    Blip(InVirtualTarget, QuantizeTarget);
    
    DrawDebug(InScene);
}

Rendering::SceneRenderer::~SceneRenderer()
{
    // Unload targets
    SceneTarget.Unload();
}

void Rendering::SceneRenderer::DrawDebugWindow()
{
    ImGui::Text(("Meshes: " + std::to_string(MeshDrawCount)).c_str());
    ImGui::Text(("Debug shapes: " + std::to_string(DebugDrawCount)).c_str());
    ImGui::Checkbox("DebugDraw##SceneRenderer", &DebugDraw);

    auto func = [](const RenderTarget& InTarget, String InName)
    {
        if (ImGui::CollapsingHeader(InName.c_str()))
        {
            auto& buffs = InTarget.GetBuffers();
            for (auto& buff : buffs)
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x, vMax.y - vMin.y};
                const float mul = static_cast<float>(buff.Tex.width) / size.width;

                // Send to ImGui
                rlImGuiImageRect(
                    &buff.Tex,
                    static_cast<int>(size.x),
                    static_cast<int>(static_cast<float>(buff.Tex.height) / mul),
                    Rectangle{
                        0, 0,
                        static_cast<float>(buff.Tex.width),
                        static_cast<float>(-buff.Tex.height)
                    });
                ImGui::TableNextColumn();
            }
        }
    };
    func(SceneTarget, "Scene");
    func(SSAOTargets[CurrentSSAOTarget], "SSAO");
    func(FrameTarget, "Frame");
    func(QuantizeTarget, "Quantize");
}


void Rendering::SceneRenderer::SetShaderValues(ShaderResource& InShader, const RenderScene& InScene, const RenderTarget& InSceneTarget, uint32 InDeferredID)
{
    auto ptr = InShader.Get();
    CHECK_RETURN(!ptr);

    const int cameraPos = InShader.GetLocation("CameraPosition");
    if (cameraPos >= 0)
        rlSetUniform(cameraPos, InScene.Cam.Position.data, SHADER_UNIFORM_VEC3, 1);

    const int timePos =  InShader.GetLocation("Time");
    float time = static_cast<float>(InScene.Time);
    if (timePos >= 0)
        rlSetUniform(timePos, &time, SHADER_UNIFORM_FLOAT, 1);

    const int resolution =  InShader.GetLocation("Resolution");
    if (resolution >= 0)
        rlSetUniform(resolution, InSceneTarget.Size().data, SHADER_UNIFORM_VEC2, 1);

    const int idPos =  InShader.GetLocation("DeferredID");
    const int32 deferredID = static_cast<int32>(InDeferredID);
    if (idPos >= 0)
        rlSetUniform(idPos, &deferredID, SHADER_UNIFORM_INT, 1);
}

void Rendering::SceneRenderer::SetCustomShaderValues(ShaderResource& InShader)
{
}

void Rendering::SceneRenderer::DrawEntries(const RenderScene& InScene, const RenderTarget& InSceneTarget)
{
    PROFILE_SCOPE_BEGIN("DrawEntries")

    InSceneTarget.BeginWrite(); 
    BeginMode3D(Utility::Ray::ConvertCamera(InScene.Cam));

    // Instanced rendering
    MeshDrawCount = 0;
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

        // Enable shader
        rlEnableShader(shader->id);
        SetShaderValues(*resShader, InScene, InSceneTarget, entry.second.DeferredID);
        SetCustomShaderValues(*resShader);

        // Data has been prepared for this entry
        // Time to draw all the instances
        for (int i = 0; i < meshCount; i++)
        {
            MeshDrawCount += static_cast<int>(entry.second.Transforms.size());
            RaylibRenderUtility::DrawInstances(meshes[i], *shader, entry.second.Transforms, InScene.Cam.Position);
        }

        // Disable shader
        rlDisableShader();
    }
    rlEnableBackfaceCulling();
    EndMode3D();
    InSceneTarget.EndWrite();

    PROFILE_SCOPE_END()
}

void Rendering::SceneRenderer::DrawDeferredScene(const RenderScene& InScene, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers)
{
    PROFILE_SCOPE_BEGIN("DrawDeferredScene")

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
    
    PROFILE_SCOPE_END()
}

void Rendering::SceneRenderer::DrawPostProcessing(const RenderScene& InScene, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers, const ResShader& InShader)
{
    PROFILE_SCOPE_BEGIN("DrawPostProcessing")

    ShaderResource* shaderResource = InShader.Get();
    CHECK_ASSERT(!shaderResource, "Failed to find shader resource");
    const Shader* shader = shaderResource->Get();
    CHECK_ASSERT(!shader, "Failed to get shader");

    InTarget.BeginWrite();
    rlEnableShader(shader->id);
    SetShaderValues(*shaderResource, InScene, InTarget, 0);
    
    RenderTarget::Slot bindOffset;
    for (auto& b : InBuffers)
        if (b) b->Bind(*shaderResource, bindOffset);

    rlLoadDrawQuad();

    RenderTarget::Slot unbindOffset;
    for (auto& b : InBuffers)
        if (b) b->Unbind(*shaderResource, unbindOffset);
    
    rlDisableShader();
    InTarget.EndWrite(); 
    
    PROFILE_SCOPE_END()
}

void Rendering::SceneRenderer::DrawDebug(const RenderScene& InScene)
{
    PROFILE_SCOPE_BEGIN("DrawDebug")

    if (!DebugDraw)
    {
        DebugDrawCount = 0;
        PROFILE_SCOPE_END()
        return;
    }

    BeginMode3D(Utility::Ray::ConvertCamera(InScene.Cam));

    for (auto& shape : InScene.DebugShapes)
    {
        switch (shape.Type)
        {
        case DebugShapeInstance::Type::SPHERE:
            DrawSphereWires(
                Utility::Ray::ConvertVec(shape.Pos),
                shape.Data.x,
                static_cast<int>(shape.Data.y),
                static_cast<int>(shape.Data.z),
                shape.Color);
            break;
        case DebugShapeInstance::Type::BOX:
            DrawCubeWiresV(
                Utility::Ray::ConvertVec(shape.Pos),
                Utility::Ray::ConvertVec(shape.Data),
                shape.Color);
            break;
        case DebugShapeInstance::Type::CAPSULE:
            const Vec3F dir = Mat4F(shape.Rot).Right() * shape.Data.y;
            const auto start = Utility::Ray::ConvertVec(shape.Pos + dir);
            const auto end = Utility::Ray::ConvertVec(shape.Pos - dir);
            DrawCapsuleWires(
                start,
                end,
                shape.Data.x,
                static_cast<int>(shape.Data.z),
                static_cast<int>(shape.Data.z) / 2,
                shape.Color);
            break;
        }
    }

    for (auto& line : InScene.DebugLines)
        DrawLine3D(
            Utility::Ray::ConvertVec(line.Start),
            Utility::Ray::ConvertVec(line.End),
            line.Color);

    DebugDrawCount = InScene.DebugShapes.size() + InScene.DebugLines.size();

    EndMode3D();

    PROFILE_SCOPE_END()
}

void Rendering::SceneRenderer::Blip(const RenderTexture2D& InTarget, const RenderTarget& InBuffer)
{
    // Flip and blip
    const Rectangle sourceRec = {
        0.0f, 0.0f,
        static_cast<float>(InTarget.texture.width),
        -static_cast<float>(InTarget.texture.height)
    };
    
    DrawTextureRec(
        InBuffer.GetBuffers()[0].Tex,
        sourceRec,
        { 0, 0 },
        WHITE);
}

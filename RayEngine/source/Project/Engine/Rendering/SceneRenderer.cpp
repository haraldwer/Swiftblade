#include "SceneRenderer.h"

#include "Engine/Profiling/Profile.h"
#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "RaylibRenderUtility.h"
#include "Utility/RayUtility.h"
#include "Utility/Time/Time.h"

void Rendering::SceneRenderer::Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget)
{
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Scene target composing with multiple targets
    // TODO: Customizable render pipeline
    
    SceneTarget.TrySetup(InVirtualTarget);
    
    // Draw to SceneTarget
    DrawEntries(InScene, SceneTarget);
    
    BeginTextureMode(InVirtualTarget);
    DrawDeferredScene(InScene, SceneTarget);
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
    
    for (auto& buff : SceneTarget.GetBuffers())
    {
        if (ImGui::CollapsingHeader(buff.Name.c_str()))
        {
            // Adjust size
            const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
            const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
            const Vec2F size = { vMax.x - vMin.x, vMax.y - vMin.y };
            const float mul = static_cast<float>(buff.Tex.width) / size.width;
            
            // Send to ImGui
            rlImGuiImageRect(
                &buff.Tex,
                static_cast<int>(size.x),
                static_cast<int>(static_cast<float>(buff.Tex.height) / mul),
                Rectangle{ 0,0,
                    static_cast<float>(buff.Tex.width),
                    static_cast<float>(-buff.Tex.height)
                });
        }
    }
}


void Rendering::SceneRenderer::SetShaderValues(const Shader* InShader, const RenderScene& InScene, const SceneRenderTarget& InSceneTarget)
{
    CHECK_RETURN(!InShader);
    
    // Camera
    const int cameraPos = GetShaderLocation(*InShader, "CameraPosition");
    SetShaderValue(*InShader, cameraPos, InScene.Cam.Position.data, SHADER_UNIFORM_VEC3);

    // Time
    const int timePos = GetShaderLocation(*InShader, "Time");
    float time = static_cast<float>(InScene.Time);
    SetShaderValue(*InShader, timePos, &time, SHADER_UNIFORM_FLOAT);

    // Resolution
    const int resolution = GetShaderLocation(*InShader, "Resolution");
    SetShaderValue(*InShader, resolution, InSceneTarget.Size().data, SHADER_UNIFORM_VEC2);
}

void Rendering::SceneRenderer::SetCustomShaderValues(const Shader* InShader) // Floats and vectors
{
    CHECK_RETURN(!InShader);
    //for (auto& val : values)
    //{
    //    const int pos = GetShaderLocation(*InShader, val.Name);
    //    SetShaderValue(*InShader, pos, val.Data, SHADER_UNIFORM_FLOAT);
    //}
}

void Rendering::SceneRenderer::DrawEntries(const RenderScene& InScene, const SceneRenderTarget& InSceneTarget)
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

        const Shader* shader = nullptr; 
        if (const auto resMat = entry.second.Material.Get())
        {
            if (const auto resShader = resMat->SurfaceShader.Get().Get())
                shader =resShader->Get();
            resMat->TwoSided ?
                rlDisableBackfaceCulling() :
                rlEnableBackfaceCulling();
        }

        CHECK_CONTINUE(!meshes);
        CHECK_CONTINUE(meshCount == 0);
        CHECK_CONTINUE(!shader);
        CHECK_CONTINUE(entry.second.Transforms.empty());

        // Enable shader
        rlEnableShader(shader->id);
        SetShaderValues(shader, InScene, InSceneTarget);
        SetCustomShaderValues(shader);
        
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

void Rendering::SceneRenderer::DrawDeferredScene(const RenderScene& InScene, const SceneRenderTarget& InSceneTarget)
{
    PROFILE_SCOPE_BEGIN("DrawDeferredScene")
    
    BeginMode3D(Utility::Ray::ConvertCamera(InScene.Cam));
    rlDisableDepthTest();
    rlDisableColorBlend();

    // TODO: For every shader 
    for (auto& entry : InScene.Meshes.DeferredShaders)
    {
        // Set shader
        const auto shaderResource = entry.second.Get();
        CHECK_CONTINUE(!shaderResource);
        const Shader* shader = shaderResource->Get();
        CHECK_CONTINUE(!shader);
        
        rlEnableShader(shader->id);
        SetShaderValues(shader, InScene, InSceneTarget);

        // Bind textures
        InSceneTarget.Bind(*shader, 0);
        
        // Draw fullscreen quad
        rlLoadDrawQuad();
        rlDisableShader();
    }

    // Reset
    rlEnableColorBlend();
    rlEnableDepthTest();
    EndMode3D();

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

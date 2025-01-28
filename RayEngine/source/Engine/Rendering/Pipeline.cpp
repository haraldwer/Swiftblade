#include "Pipeline.h"

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Utility/RayUtility.h"

void Rendering::Pipeline::Init()
{
    SSAOShader = ResShader("Shaders/PostProcessing/SH_SSAO.ps"); 
    QuantizeShader = ResShader("Shaders/PostProcessing/SH_Quantize.ps");
    FXAAShader = ResShader("Shaders/PostProcessing/SH_FXAA.ps");
    
    FireShader = ResShader("Shaders/PostProcessing/SH_Fire.ps");
    FireBlipShader = ResShader("Shaders/PostProcessing/SH_FireBlip.ps");
}

void Rendering::Pipeline::Setup(const RenderTexture2D& InVirtualTarget)
{
    if (SceneTarget.TryBeginSetup(InVirtualTarget))
    {
        SceneTarget.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
        SceneTarget.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
        SceneTarget.CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
        SceneTarget.CreateBuffer("TexVelocity", PIXELFORMAT_UNCOMPRESSED_R8G8B8);
        SceneTarget.CreateBuffer("TexDeferredData", PIXELFORMAT_UNCOMPRESSED_R32G32B32);
        SceneTarget.EndSetup(InVirtualTarget);
    }

    for (auto& target : SSAOTargets.All())
        target.Setup(InVirtualTarget, "TexAO", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    for (auto& target : FireTargets.All())
        target.Setup(InVirtualTarget, "TexFire", PIXELFORMAT_UNCOMPRESSED_R8G8B8A8);
    FrameTarget.Setup(InVirtualTarget, "TexFrame", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    QuantizeTarget.Setup(InVirtualTarget, "TexQuantize", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
}

void Rendering::Pipeline::Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget)
{
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Customizable render pipeline

    Setup(InVirtualTarget);

    // Draw to SceneTarget
    MeshDrawCount = Renderer.DrawScene(InScene, SceneTarget);

    // Propagate fire
    FireTargets.Iterate();
    Renderer.DrawFullscreen(InScene, FireTargets.Curr(), FireShader, { &SceneTarget, &FireTargets.Prev() });
    
    // Collect AO
    SSAOTargets.Iterate();
    Renderer.DrawFullscreen(InScene, SSAOTargets.Curr(), SSAOShader, { &SceneTarget, &SSAOTargets.Prev() });
    
    // Draw to FrameTarget, quantize
    Renderer.DrawDeferredScene(InScene, FrameTarget, { &SceneTarget, &SSAOTargets.Curr() });
    Renderer.DrawFullscreen(InScene, FrameTarget, FireBlipShader, { &SceneTarget, &FireTargets.Curr() }, {}, -1, false);
    Renderer.DrawFullscreen(InScene, QuantizeTarget, QuantizeShader, { &SceneTarget, &SSAOTargets.Curr(), &FrameTarget });
    Renderer.DrawFullscreen(InScene, FrameTarget, FXAAShader, { &QuantizeTarget });

    // Blip FXAA to final target (should maybe skip FXAA target completely?)
    // Also handles upscaling?
    Renderer.Blip(InVirtualTarget, FrameTarget);

    DebugDrawCount = 0;
    if (DebugDraw)
        DebugDrawCount = Renderer.DrawDebug(InScene);
}

Rendering::Pipeline::~Pipeline()
{
    // Unload targets
    SceneTarget.Unload();
    for (auto& t : SSAOTargets.All())
        t.Unload();
    FrameTarget.Unload();
    QuantizeTarget.Unload();
}

void Rendering::Pipeline::DrawDebugWindow()
{
    ImGui::Text(("Meshes: " + std::to_string(MeshDrawCount)).c_str());
    ImGui::Text(("Debug shapes: " + std::to_string(DebugDrawCount)).c_str());
    ImGui::Checkbox("DebugDraw##SceneRenderer", &DebugDraw);

    auto func = [](const RenderTarget& InTarget, const String& InName)
    {
        if (ImGui::CollapsingHeader(InName.c_str()))
        {
            for (auto& buff : InTarget.GetBuffers())
            {
                // Adjust size
                const ImVec2 vMin = ImGui::GetWindowContentRegionMin();
                const ImVec2 vMax = ImGui::GetWindowContentRegionMax();
                const Vec2F size = {vMax.x - vMin.x - 0.1f, vMax.y - vMin.y - 0.1f};
                const float mul = static_cast<float>(buff.Tex.width) / size.width;

                // Send to ImGui
                SetTextureWrap(buff.Tex, TEXTURE_WRAP_REPEAT);
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
    func(SSAOTargets.Curr(), "SSAO");
    func(FireTargets.Curr(), "Fire");
    func(FrameTarget, "Frame");
    func(QuantizeTarget, "Quantize");
}

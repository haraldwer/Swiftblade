#include "Pipeline.h"

#include "ImGui/imgui.h"
#include "ImGui/rlImGui.h"
#include "Utility/RayUtility.h"

void Rendering::Pipeline::Init()
{
    SSAOShader = ResShader("Shaders/PostProcessing/SH_SSAO.ps"); 
    QuantizeShader = ResShader("Shaders/PostProcessing/SH_Quantize.ps");
    FXAAShader = ResShader("Shaders/PostProcessing/SH_FXAA.ps");
    
    FireShader = ResShader("Shaders/DeferredProcessing/SH_Fire.ds");
}

void Rendering::Pipeline::Setup(const RenderTexture2D& InVirtualTarget)
{
    for (auto& target : SceneTargets.All())
    {
        if (target.TryBeginSetup(InVirtualTarget))
        {
            target.CreateBuffer("TexPosition", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
            target.CreateBuffer("TexNormal", PIXELFORMAT_UNCOMPRESSED_R16G16B16);
            target.CreateBuffer("TexColor", PIXELFORMAT_UNCOMPRESSED_R4G4B4A4);
            target.CreateBuffer("TexVelocity", PIXELFORMAT_UNCOMPRESSED_R32G32B32);
            target.CreateBuffer("TexDeferredData", PIXELFORMAT_UNCOMPRESSED_R32G32B32A32);
            target.EndSetup(InVirtualTarget);
        }
    }
    
    for (auto& target : SSAOTargets.All())
        target.Setup(InVirtualTarget, "TexAO", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    FrameTarget.Setup(InVirtualTarget, "TexFrame", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    QuantizeTarget.Setup(InVirtualTarget, "TexQuantize", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
    FXAATarget.Setup(InVirtualTarget, "TexFXAA", PIXELFORMAT_UNCOMPRESSED_R16G16B16A16);
}

void Rendering::Pipeline::Render(const RenderScene& InScene, const RenderTexture2D& InVirtualTarget)
{
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Scene target composing with multiple targets
    // TODO: Customizable render pipeline

    Setup(InVirtualTarget);

    // Draw to SceneTarget
    SceneTargets.Iterate(); 
    MeshDrawCount = Renderer.DrawScene(InScene, SceneTargets);

    // Propagate fire
    // Draw to current deferred scene
    // Needs "current" and previous deferred scene
    SceneTargets.Iterate();
    Renderer.DrawFullscreen(InScene, SceneTargets.Curr(), FireShader, { &SceneTargets.Prev(1) }, { &SceneTargets.Prev(2) });
    
    // Collect AO
    SSAOTargets.Iterate();
    Renderer.DrawFullscreen(InScene, SSAOTargets.Curr(), SSAOShader, { &SceneTargets.Curr(), &SSAOTargets.Prev() });
    
    // Draw to FrameTarget, quantize
    Renderer.DrawDeferredScene(InScene, FrameTarget, SceneTargets, { &SSAOTargets.Curr() });
    Renderer.DrawFullscreen(InScene, QuantizeTarget, QuantizeShader, { &SceneTargets.Curr(), &SSAOTargets.Curr(), &FrameTarget });
    Renderer.DrawFullscreen(InScene, FXAATarget, FXAAShader, { &QuantizeTarget });

    // Blip FXAA to final target (should maybe skip FXAA target completely?)
    Renderer.Blip(InVirtualTarget, FXAATarget);

    DebugDrawCount = 0;
    if (DebugDraw)
        DebugDrawCount = Renderer.DrawDebug(InScene);
}

Rendering::Pipeline::~Pipeline()
{
    // Unload targets
    for (auto& t : SceneTargets.All())
        t.Unload();
    for (auto& t : SSAOTargets.All())
        t.Unload();
    FrameTarget.Unload();
    QuantizeTarget.Unload();
    FXAATarget.Unload();
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
    func(SceneTargets.Curr(), "Scene");
    func(SceneTargets.Prev(), "Scene prev");
    func(SceneTargets.Prev(2), "Scene prev 2");
    func(SSAOTargets.Curr(), "SSAO");
    func(FrameTarget, "Frame");
    func(QuantizeTarget, "Quantize");
    func(FXAATarget, "FXAA");
}

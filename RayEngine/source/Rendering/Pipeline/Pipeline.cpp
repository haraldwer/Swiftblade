#include "Pipeline.h"

#include "Context/Context.h"
#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::Pipeline::Render(const RenderArgs& InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    // TODO: Customizable render pipeline

    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& fireTargets = InArgs.Viewport->Targets.FireTargets;
    auto& ssaoTargets = InArgs.Viewport->Targets.SSAOTargets;
    auto& quantizeTarget = InArgs.Viewport->Targets.QuantizeTarget;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    auto& virtualTarget = InArgs.Viewport->VirtualTarget;

    auto& FireShader = InArgs.Context->FireShader;
    auto& SSAOShader = InArgs.Context->SSAOShader;
    auto& FireBlipShader = InArgs.Context->FireBlipShader;
    auto& QuantizeShader = InArgs.Context->QuantizeShader;
    auto& FXAAShader = InArgs.Context->FXAAShader;

    Stats stats;
    
    // Draw to SceneTarget
    stats.MeshDrawCount = Renderer::DrawScene(InArgs, sceneTarget);

    // Propagate fire
    fireTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, fireTargets.Curr(), FireShader, { &sceneTarget, &fireTargets.Prev() });
    
    // Collect AO
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), SSAOShader, { &sceneTarget, &ssaoTargets.Prev() });
    
    // Draw to FrameTarget, quantize
    stats.DeferredDrawCount = Renderer::DrawDeferredScene(InArgs, frameTarget, { &sceneTarget, &ssaoTargets.Curr() });
    Renderer::DrawFullscreen(InArgs, frameTarget, FireBlipShader, { &sceneTarget, &fireTargets.Curr() }, {}, -1, false);
    Renderer::DrawFullscreen(InArgs, quantizeTarget, QuantizeShader, { &sceneTarget, &ssaoTargets.Curr(), &frameTarget });
    Renderer::DrawFullscreen(InArgs, frameTarget, FXAAShader, { &quantizeTarget });

    // Blip FXAA to final target (should maybe skip FXAA target completely?)
    // Also handles upscaling?
    Renderer::Blip(*virtualTarget, frameTarget);
    
    stats.DebugDrawCount = Renderer::DrawDebug(InArgs);
    return stats;
}

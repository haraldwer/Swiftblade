#include "Pipeline.h"

#include "Context/Context.h"
#include "Lumin/Lumin.h"
#include "Scene/Scene.h"
#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::Pipeline::Render(RenderArgs InArgs)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");

    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    
    Stats stats;
    
    if (InArgs.Lumin == nullptr)
        InArgs.Lumin = InArgs.Context->LuminPtr;
    if (InArgs.Lumin && InArgs.Context->Config.Lumin)
        stats += InArgs.Lumin->UpdateProbes(InArgs);

    stats += RenderScene(InArgs);
    stats += RenderFire(InArgs);
    stats += RenderAO(InArgs);
    stats += RenderSkybox(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLumin(InArgs);
    stats += RenderFX(InArgs);
    stats += Blip(InArgs);
    stats += RenderDebug(InArgs); 
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderScene(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    stats.MeshDrawCount = Renderer::DrawScene(InArgs, sceneTarget);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLumin(const RenderArgs& InArgs)
{
    CHECK_RETURN(!InArgs.Context->Config.Lumin, {})
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    stats.Probes += Renderer::DrawLuminProbes(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDeferred(const RenderArgs& InArgs)
{
    Stats stats;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& ssaoTargets = InArgs.Viewport->Targets.SSAOTargets;
    stats.DeferredDrawCount = Renderer::DrawDeferredScene(InArgs, frameTarget, { &sceneTarget, &ssaoTargets.Curr() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderSkybox(const RenderArgs& InArgs)
{
    Stats stats;
    stats.Skyboxes += Renderer::DrawSkyboxes(InArgs, InArgs.Viewport->Targets.FrameTarget);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderFire(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& fireTargets = InArgs.Viewport->Targets.FireTargets;
    auto& FireShader = InArgs.Context->FireShader;
    fireTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, fireTargets.Curr(), FireShader, { &sceneTarget, &fireTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ApplyFire(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    auto& fireTargets = InArgs.Viewport->Targets.FireTargets;
    auto& FireBlipShader = InArgs.Context->FireBlipShader;
    Renderer::DrawFullscreen(InArgs, frameTarget, FireBlipShader, { &sceneTarget, &fireTargets.Curr() }, -1, false);
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderAO(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& ssaoTargets = InArgs.Viewport->Targets.SSAOTargets;
    auto& SSAOShader = InArgs.Context->SSAOShader;
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), SSAOShader, { &sceneTarget, &ssaoTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderFX(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTarget;
    auto& ssaoTargets = InArgs.Viewport->Targets.SSAOTargets;
    auto& quantizeTarget = InArgs.Viewport->Targets.QuantizeTarget;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    auto& QuantizeShader = InArgs.Context->QuantizeShader;
    auto& FXAAShader = InArgs.Context->FXAAShader;
    Renderer::DrawFullscreen(InArgs, quantizeTarget, QuantizeShader, { &sceneTarget, &ssaoTargets.Curr(), &frameTarget });
    Renderer::DrawFullscreen(InArgs, frameTarget, FXAAShader, { &quantizeTarget });
    stats.FullscreenPasses += 2;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::Blip(const RenderArgs& InArgs)
{
    Stats stats;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTarget;
    auto& virtualTarget = InArgs.Viewport->VirtualTarget;
    Renderer::Blip(*virtualTarget, frameTarget);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDebug(const RenderArgs& InArgs)
{
    Stats stats;
    stats.DebugDrawCount = Renderer::DrawDebug(InArgs);
    return stats;
}

RenderTarget& Rendering::Pipeline::GetFrameTarget(const RenderArgs& InArgs)
{
    return InArgs.Viewport->Targets.FrameTarget;
}

RenderTarget& Rendering::Pipeline::GetSceneTarget(const RenderArgs& InArgs)
{
    return InArgs.Viewport->Targets.SceneTarget;
}

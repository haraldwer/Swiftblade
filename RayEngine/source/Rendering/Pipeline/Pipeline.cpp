#include "Pipeline.h"

#include "rlgl.h"
#include "Context/Context.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "Scene/Scene.h"
#include "State/State.h"
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
    if (InArgs.Lights == nullptr)
        InArgs.Lights = InArgs.Context->LightsPtr;
    
    if (InArgs.Lumin && InArgs.Context->Config.Lumin)
        stats += InArgs.Lumin->Update(InArgs);
    if (InArgs.Lights)
        stats += InArgs.Lights->Update(InArgs); 
    
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs);
    stats += ProcessScene(InArgs);
    stats += RenderFire(InArgs);
    stats += RenderAO(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);
    stats += ApplyFire(InArgs);
    stats += RenderFX(InArgs);
    stats += Blip(InArgs);
    stats += RenderDebug(InArgs);
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderScene(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets;
    stats.MeshDrawCount = Renderer::DrawScene(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ProcessScene(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTargets = InArgs.Viewport->Targets.SceneTargets;
    auto& shader = InArgs.Context->Config.ProcessSceneShader;
    sceneTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, sceneTargets.Curr(), shader, { &sceneTargets.Prev() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderFire(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& fireTargets = InArgs.Viewport->Targets.FireTargets;
    auto& FireShader = InArgs.Context->Config.FireShader;
    fireTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, fireTargets.Curr(), FireShader, { &sceneTarget, &fireTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderAO(const RenderArgs& InArgs)
{
    if (!InArgs.Context->Config.SSAO)
        return {};
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.Viewport->Targets.AOTargets;
    auto& SSAOShader = InArgs.Context->Config.SSAOShader;
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), SSAOShader, { &sceneTarget, &ssaoTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderSkybox(const RenderArgs& InArgs)
{
    Stats stats;
    stats.Skyboxes += Renderer::DrawSkyboxes(InArgs, InArgs.Viewport->Targets.FrameTargets.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDeferred(const RenderArgs& InArgs)
{
    Stats stats;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTargets.Curr();
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.Viewport->Targets.AOTargets;
    stats.DeferredDrawCount = Renderer::DrawDeferredScene(InArgs, frameTarget, { &sceneTarget, &ssaoTargets.Curr() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLights(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.Viewport->Targets.FrameTargets.Curr();
    stats.Lights += Renderer::DrawLights(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLumin(const RenderArgs& InArgs)
{
    CHECK_RETURN(!InArgs.Context->Config.Lumin, {})
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.Viewport->Targets.FrameTargets.Curr();
    stats.Probes += Renderer::DrawLuminProbes(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ApplyFire(const RenderArgs& InArgs)
{
    Stats stats;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.Viewport->Targets.FrameTargets.Curr();
    auto& fireTargets = InArgs.Viewport->Targets.FireTargets;
    auto& FireBlipShader = InArgs.Context->Config.FireBlipShader;
    Renderer::DrawFullscreen(InArgs, frameTarget, FireBlipShader, { &sceneTarget, &fireTargets.Curr() }, -1, false);
    stats.FullscreenPasses++;
    return stats;
}


Rendering::Pipeline::Stats Rendering::Pipeline::RenderFX(const RenderArgs& InArgs)
{
    Stats stats;
    auto& conf = InArgs.Context->Config;
    auto& frameTargets = InArgs.Viewport->Targets.FrameTargets;
    auto& sceneTarget = InArgs.Viewport->Targets.SceneTargets.Curr();
    if (conf.Quantize)
    {
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.QuantizeShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.MotionBlur)
    {
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.MotionBlurShader, { &sceneTarget, &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.Distort)
    {
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.DistortShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.FXAA)
    {
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FXAAShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.Tonemapping)
    {
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.TonemappingShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::Blip(const RenderArgs& InArgs)
{
    Stats stats;
    auto& frameTarget = InArgs.Viewport->Targets.FrameTargets.Curr();
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

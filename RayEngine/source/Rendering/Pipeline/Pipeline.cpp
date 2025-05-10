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
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.ContextPtr, "Invalid context");

    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    
    Stats stats;
    
    if (InArgs.LuminPtr == nullptr)
        InArgs.LuminPtr = InArgs.ContextPtr->LuminPtr;
    if (InArgs.LightsPtr == nullptr)
        InArgs.LightsPtr = InArgs.ContextPtr->LightsPtr;
    
    if (InArgs.LuminPtr && InArgs.ContextPtr->Config.Lumin)
        stats += InArgs.LuminPtr->Update(InArgs);
    if (InArgs.LightsPtr)
        stats += InArgs.LightsPtr->Update(InArgs); 
    
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

Rendering::Pipeline::Stats Rendering::Pipeline::RenderSkybox(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets;
    stats.Skyboxes += Renderer::DrawSkyboxes(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets;
    stats.MeshDrawCount = Renderer::DrawScene(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ProcessScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTargets = InArgs.ViewportPtr->Targets.SceneTargets;
    auto& shader = InArgs.ContextPtr->Config.ProcessSceneShader;
    sceneTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, sceneTargets.Curr(), shader, { &sceneTargets.Prev() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderFire(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& fireTargets = InArgs.ViewportPtr->Targets.FireTargets;
    auto& FireShader = InArgs.ContextPtr->Config.FireShader;
    fireTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, fireTargets.Curr(), FireShader, { &sceneTarget, &fireTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderAO(const RenderArgs& InArgs)
{
    if (!InArgs.ContextPtr->Config.SSAO)
        return {};
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.ViewportPtr->Targets.AOTargets;
    auto& SSAOShader = InArgs.ContextPtr->Config.SSAOShader;
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), SSAOShader, { &sceneTarget, &ssaoTargets.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDeferred(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& frameTarget = InArgs.ViewportPtr->Targets.FrameTargets.Curr();
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.ViewportPtr->Targets.AOTargets;
    stats.DeferredDrawCount = Renderer::DrawDeferredScene(InArgs, frameTarget, { &sceneTarget, &ssaoTargets.Curr() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.ViewportPtr->Targets.FrameTargets.Curr();
    stats.Lights += Renderer::DrawLights(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLumin(const RenderArgs& InArgs)
{
    CHECK_RETURN(!InArgs.ContextPtr->Config.Lumin, {})
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.ViewportPtr->Targets.FrameTargets.Curr();
    stats.Probes += Renderer::DrawLuminProbes(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ApplyFire(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    auto& frameTarget = InArgs.ViewportPtr->Targets.FrameTargets.Curr();
    auto& fireTargets = InArgs.ViewportPtr->Targets.FireTargets;
    auto& FireBlipShader = InArgs.ContextPtr->Config.FireBlipShader;
    Renderer::DrawFullscreen(InArgs, frameTarget, FireBlipShader, { &sceneTarget, &fireTargets.Curr() }, -1, false);
    stats.FullscreenPasses++;
    return stats;
}


Rendering::Pipeline::Stats Rendering::Pipeline::RenderFX(const RenderArgs& InArgs)
{
    Stats stats;
    auto& conf = InArgs.ContextPtr->Config;
    auto& frameTargets = InArgs.ViewportPtr->Targets.FrameTargets;
    auto& sceneTarget = InArgs.ViewportPtr->Targets.SceneTargets.Curr();
    if (conf.Quantize)
    {
        PROFILE_GL_NAMED("Quantize");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.QuantizeShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.MotionBlur)
    {
        PROFILE_GL_NAMED("MotionBlur");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.MotionBlurShader, { &sceneTarget, &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.Distort)
    {
        PROFILE_GL_NAMED("Distort");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.DistortShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.FXAA)
    {
        PROFILE_GL_NAMED("FXAA");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FXAAShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    if (conf.Tonemapping)
    {
        PROFILE_GL_NAMED("Tonemapping");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.TonemappingShader, { &frameTargets.Prev() });
        stats.FullscreenPasses++;
    }
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::Blip(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& frameTarget = InArgs.ViewportPtr->Targets.FrameTargets.Curr();
    auto& virtualTarget = InArgs.ViewportPtr->VirtualTarget;
    Renderer::Blip(*virtualTarget, frameTarget);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDebug(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    stats.DebugDrawCount = Renderer::DrawDebug(InArgs);
    return stats;
}

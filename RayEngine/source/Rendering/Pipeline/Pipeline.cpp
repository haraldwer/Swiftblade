#include "Pipeline.h"

#include "Context/Context.h"
#include "Lights/Lights.h"
#include "Lumin/Lumin.h"
#include "Scene/Scene.h"
#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::Pipeline::Render(RenderArgs InArgs)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");

    // TODO: Sub-tick camera movement
    // TODO: Depth sorting
    
    Stats stats;
    
    if (InArgs.luminPtr == nullptr)
        InArgs.luminPtr = InArgs.contextPtr->LuminPtr;
    if (InArgs.lightsPtr == nullptr)
        InArgs.lightsPtr = InArgs.contextPtr->LightsPtr;
    
    if (InArgs.luminPtr && InArgs.contextPtr->Config.Lumin)
        stats += InArgs.luminPtr->Update(InArgs);
    if (InArgs.lightsPtr)
        stats += InArgs.lightsPtr->Update(InArgs); 
    
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
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets;
    stats.skyboxes += Renderer::DrawSkyboxes(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets;
    stats.meshDrawCount = Renderer::DrawScene(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ProcessScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTargets = InArgs.viewportPtr->targets.SceneTargets;
    auto& shader = InArgs.contextPtr->Config.ProcessSceneShader;
    sceneTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, sceneTargets.Curr(), shader, { &sceneTargets.Prev() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderFire(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    auto& fireTargets = InArgs.viewportPtr->targets.FireTargets;
    auto& fireShader = InArgs.contextPtr->Config.FireShader;
    fireTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, fireTargets.Curr(), fireShader, { &sceneTarget, &fireTargets.Prev() });
    stats.fullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderAO(const RenderArgs& InArgs)
{
    if (!InArgs.contextPtr->Config.SSAO)
        return {};
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.viewportPtr->targets.AOTargets;
    auto& ssaoShader = InArgs.contextPtr->Config.SSAOShader;
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), ssaoShader, { &sceneTarget, &ssaoTargets.Prev() });
    stats.fullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDeferred(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    const auto& frameTarget = InArgs.viewportPtr->targets.FrameTargets.Curr();
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    auto& ssaoTargets = InArgs.viewportPtr->targets.AOTargets;
    stats.deferredDrawCount = Renderer::DrawDeferredScene(InArgs, frameTarget, { &sceneTarget, &ssaoTargets.Curr() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    const auto& frameTarget = InArgs.viewportPtr->targets.FrameTargets.Curr();
    stats.lights += Renderer::DrawLights(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLumin(const RenderArgs& InArgs)
{
    CHECK_RETURN(!InArgs.contextPtr->Config.Lumin, {})
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    const auto& frameTarget = InArgs.viewportPtr->targets.FrameTargets.Curr();
    stats.probes += Renderer::DrawLuminProbes(InArgs, frameTarget, { &sceneTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ApplyFire(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    const auto& frameTarget = InArgs.viewportPtr->targets.FrameTargets.Curr();
    auto& fireTargets = InArgs.viewportPtr->targets.FireTargets;
    auto& fireBlipShader = InArgs.contextPtr->Config.FireBlipShader;
    Renderer::DrawFullscreen(InArgs, frameTarget, fireBlipShader, { &sceneTarget, &fireTargets.Curr() }, -1, false);
    stats.fullscreenPasses++;
    return stats;
}


Rendering::Pipeline::Stats Rendering::Pipeline::RenderFX(const RenderArgs& InArgs)
{
    Stats stats;
    auto& conf = InArgs.contextPtr->Config;
    auto& frameTargets = InArgs.viewportPtr->targets.FrameTargets;
    auto& sceneTarget = InArgs.viewportPtr->targets.SceneTargets.Curr();
    if (conf.Quantize)
    {
        PROFILE_GL_NAMED("Quantize");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.QuantizeShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.MotionBlur)
    {
        PROFILE_GL_NAMED("MotionBlur");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.MotionBlurShader, { &sceneTarget, &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.Distort)
    {
        PROFILE_GL_NAMED("Distort");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.DistortShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FXAA)
    {
        PROFILE_GL_NAMED("FXAA");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FXAAShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.Tonemapping)
    {
        PROFILE_GL_NAMED("Tonemapping");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.TonemappingShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::Blip(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    const auto& frameTarget = InArgs.viewportPtr->targets.FrameTargets.Curr();
    const auto& virtualTarget = InArgs.viewportPtr->virtualTarget;
    Renderer::Blip(*virtualTarget, frameTarget);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDebug(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    stats.debugDrawCount = Renderer::DrawDebug(InArgs);
    return stats;
}

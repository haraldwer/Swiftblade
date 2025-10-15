#include "Pipeline.h"

#include "DeferredRenderer.h"
#include "../../../cmake-build-web-release/_deps/raylib-src/src/rlgl.h"
#include "Lights/LightsRenderer.h"
#include "Lumin/LuminRenderer.h"
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
        InArgs.luminPtr = InArgs.contextPtr->luminPtr;
    if (InArgs.lightsPtr == nullptr)
        InArgs.lightsPtr = InArgs.contextPtr->lightsPtr;
    
    //if (InArgs.luminPtr)
    //    stats += InArgs.luminPtr->Update(InArgs);
    if (InArgs.lightsPtr)
        stats += InArgs.lightsPtr->Update(InArgs); 

    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs); 
    stats += ProcessScene(InArgs);
    stats += RenderAO(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderSurfaces(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);
    stats += RenderDebug(InArgs);
    stats += RenderSceneFX(InArgs);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderPost(RenderArgs InArgs)
{
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    Stats stats;
    stats += RenderPostFX(InArgs);
    stats += Blip(InArgs);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderCustom(RenderArgs InArgs, std::function<void()> InFunc)
{
    Stats stats;
    stats.fullscreenPasses = Renderer::DrawCustom(InArgs, InFunc);
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderSkybox(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets;
    stats.skyboxes += DeferredRenderer::DrawSkyboxes(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets;
    stats.meshDrawCount = DeferredRenderer::DrawScene(InArgs, sceneTarget.Curr());
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::ProcessScene(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTargets = InArgs.viewportPtr->targets.sceneTargets;

    if (InArgs.contextPtr->config.FX.Get().Bump)
    {
        auto& bumpShader = InArgs.contextPtr->config.FX.Get().BumpShader;
        sceneTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, sceneTargets.Curr(), bumpShader, { &sceneTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (InArgs.contextPtr->config.FX.Get().Parallax)
    {
        auto& pomShader = InArgs.contextPtr->config.FX.Get().POMShader;
        sceneTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, sceneTargets.Curr(), pomShader, { &sceneTargets.Prev() });
        stats.fullscreenPasses++;
    }
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderAO(const RenderArgs& InArgs)
{
    if (!InArgs.contextPtr->config.FX.Get().SSAO)
        return {};
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    auto& ssaoTargets = InArgs.viewportPtr->targets.aoTargets;
    auto& ssaoShader = InArgs.contextPtr->config.FX.Get().SSAOShader;
    ssaoTargets.Iterate();
    Renderer::DrawFullscreen(InArgs, ssaoTargets.Curr(), ssaoShader, { &sceneTarget, &ssaoTargets.Prev() });
    stats.fullscreenPasses++;
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderDeferred(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    auto& surfaceTarget = InArgs.viewportPtr->targets.surfaceTarget;
    auto& ssaoTarget = InArgs.viewportPtr->targets.aoTargets.Curr();
    stats.deferredDrawCount = DeferredRenderer::DrawDeferredScene(InArgs, surfaceTarget, { &sceneTarget, &ssaoTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderSurfaces(const RenderArgs &InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& frameTarget = InArgs.viewportPtr->targets.frameTargets.Curr();
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    auto& surfaceTarget = InArgs.viewportPtr->targets.surfaceTarget;
    stats.fullscreenPasses += DeferredRenderer::DrawSurfaces(InArgs, frameTarget, { &sceneTarget, &surfaceTarget });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLights(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    auto& surfaceTarget = InArgs.viewportPtr->targets.surfaceTarget;
    auto& ssaoTargets = InArgs.viewportPtr->targets.aoTargets;
    const auto& frameTarget = InArgs.viewportPtr->targets.frameTargets.Curr();
    stats.lights += LightsRenderer::DrawLights(InArgs, frameTarget, { &sceneTarget, &surfaceTarget, &ssaoTargets.Curr() });
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::RenderLumin(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    const auto& frameTarget = InArgs.viewportPtr->targets.frameTargets.Curr();
    stats.probes += LuminRenderer::DrawLuminProbesDebug(InArgs, frameTarget, { &sceneTarget });
    return stats;
}


Rendering::Pipeline::Stats Rendering::Pipeline::RenderSceneFX(const RenderArgs &InArgs)
{
    Stats stats;
    auto& conf = InArgs.contextPtr->config;
    auto& frameTargets = InArgs.viewportPtr->targets.frameTargets;
    auto& sceneTarget = InArgs.viewportPtr->targets.sceneTargets.Curr();
    if (conf.FX.Get().MotionBlur)
    {
        PROFILE_GL_NAMED("MotionBlur");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().MotionBlurShader, { &sceneTarget, &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FX.Get().Bloom)
    {
        PROFILE_GL_NAMED("Bloom");
        auto& bloomTargets = InArgs.viewportPtr->targets.bloomTargets;
        Renderer::DrawBloom(InArgs, bloomTargets, frameTargets);
    }
    if (conf.FX.Get().Tonemapping)
    {
        PROFILE_GL_NAMED("Tonemapping");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().TonemappingShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FX.Get().Distort)
    {
        PROFILE_GL_NAMED("Distort");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().DistortShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    return stats;
}
Rendering::Pipeline::Stats Rendering::Pipeline::RenderPostFX(const RenderArgs &InArgs)
{
    Stats stats;
    auto& conf = InArgs.contextPtr->config;
    auto& frameTargets = InArgs.viewportPtr->targets.frameTargets;
    if (conf.FX.Get().Quantize)
    {
        PROFILE_GL_NAMED("Quantize");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().QuantizeShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FX.Get().FXAA)
    {
        PROFILE_GL_NAMED("FXAA");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().FXAAShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FX.Get().ChromaticAberration)
    {
        PROFILE_GL_NAMED("Chromatic Aberration");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().CAShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    if (conf.FX.Get().Grain)
    {
        PROFILE_GL_NAMED("Grain");
        frameTargets.Iterate();
        Renderer::DrawFullscreen(InArgs, frameTargets.Curr(), conf.FX.Get().GrainShader, { &frameTargets.Prev() });
        stats.fullscreenPasses++;
    }
    return stats;
}

Rendering::Pipeline::Stats Rendering::Pipeline::Blip(const RenderArgs& InArgs)
{
    PROFILE_GL();
    Stats stats;
    const auto& frameTarget = InArgs.viewportPtr->targets.frameTargets.Curr();
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

#include "LuminPipeline.h"

#include "TextureTargets/SwapTarget.h"
#include "Viewport/Viewport.h"
#include "rlgl.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);

    auto frame = InArgs.viewportPtr->GetTargets().frameTargets.Curr();
    auto scene = InArgs.viewportPtr->GetTargets().sceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.fullscreenPasses++;
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderFallbackProbe(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderDeferred(InArgs);

    auto frame = InArgs.viewportPtr->GetTargets().frameTargets.Curr();
    auto scene = InArgs.viewportPtr->GetTargets().sceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.fullscreenPasses++;
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::LuminPipeline::LerpProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget, const RenderTarget& InLerpTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    Stats stats;
    Renderer::DrawFullscreen(InArgs, InLerpTarget, InShader, { &InTarget }, RL_BLEND_ALPHA, false);
    stats.fullscreenPasses++;
    return stats;
}

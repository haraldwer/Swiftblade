#include "LuminPipeline.h"

#include "TextureTargets/SwapTarget.h"
#include "Viewport/Viewport.h"
#include "rlgl.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.ContextPtr, "Invalid context");
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);

    auto frame = InArgs.ViewportPtr->GetTargets().FrameTargets.Curr();
    auto scene = InArgs.ViewportPtr->GetTargets().SceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.FullscreenPasses++;
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::LuminPipeline::LerpProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget, SwapTarget& InLerpTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.ContextPtr, "Invalid context");
    
    Stats stats;
    InLerpTarget.Iterate();
    Renderer::DrawFullscreen(InArgs, InLerpTarget.Curr(), InShader, { &InTarget, &InLerpTarget.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

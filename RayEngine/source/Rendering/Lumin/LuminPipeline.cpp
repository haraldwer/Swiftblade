#include "LuminPipeline.h"

#include "TextureTargets/SwapTarget.h"
#include "Viewport/Viewport.h"
#include "rlgl.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);

    auto frame = InArgs.Viewport->GetTargets().FrameTargets.Curr();
    auto scene = InArgs.Viewport->GetTargets().SceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.FullscreenPasses++;
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::LuminPipeline::LerpProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget, SwapTarget& InLerpTarget)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    InLerpTarget.Iterate();
    Renderer::DrawFullscreen(InArgs, InLerpTarget.Curr(), InShader, { &InTarget, &InLerpTarget.Prev() });
    stats.FullscreenPasses++;
    return stats;
}

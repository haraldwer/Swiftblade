#include "LuminPipeline.h"

#include "TextureTargets/SwapTarget.h"
#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    stats += RenderScene(InArgs);
    stats += RenderSkybox(InArgs);
    stats += RenderDeferred(InArgs);
    stats += RenderLights(InArgs);
    stats += RenderLumin(InArgs);

    auto frame = InArgs.Viewport->GetTargets().FrameTarget;
    auto scene = InArgs.Viewport->GetTargets().SceneTarget;
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.FullscreenPasses++;
    
    return stats;
}

#include "LuminPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbeFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, bool InClear)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    stats += RenderScene(InArgs);
    //stats += RenderLumin(InArgs);
    stats += RenderSkybox(InArgs);
    stats += RenderDeferred(InArgs);

    auto& targets = InArgs.Viewport->GetTargets();
    auto frame = targets.FrameTarget;
    auto scene = targets.SceneTarget;
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, InClear);
    return stats;
}

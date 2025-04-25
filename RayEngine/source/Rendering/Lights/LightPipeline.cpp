#include "LightPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadows(const RenderArgs& InArgs, const ResShader& InShader, SwapTarget& InTarget)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    // TODO: Skip all shading
    
    Stats stats;
    stats += RenderScene(InArgs); 
    stats += RenderSkybox(InArgs);
    stats += RenderDeferred(InArgs);

    InTarget.Iterate();
    auto& frameTarget = InArgs.Viewport->GetTargets().FrameTarget;
    auto& sceneTarget = InArgs.Viewport->GetTargets().SceneTarget;
    Renderer::DrawFullscreen(InArgs, InTarget.Curr(), InShader, { &frameTarget, &sceneTarget, &InTarget.Prev() });
    stats.FullscreenPasses++;
    
    return stats;
}

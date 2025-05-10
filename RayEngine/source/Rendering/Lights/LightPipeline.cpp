#include "LightPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadows(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.ScenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.ViewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.ContextPtr, "Invalid context");
    
    // TODO: Skip all shading
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs); 
    stats += RenderDeferred(InArgs);

    auto& frame = InArgs.ViewportPtr->GetTargets().FrameTargets.Curr();
    auto& scene = InArgs.ViewportPtr->GetTargets().SceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.FullscreenPasses++;
    
    return stats;
}

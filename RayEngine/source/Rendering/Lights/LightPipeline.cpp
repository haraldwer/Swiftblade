#include "LightPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadows(const RenderArgs& InArgs, const ResShader& InShader, const RenderTarget& InTarget)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    // TODO: Skip all shading
    
    Stats stats;
    stats += RenderSkybox(InArgs);
    stats += RenderScene(InArgs); 
    stats += RenderDeferred(InArgs);

    auto& frame = InArgs.viewportPtr->GetTargets().FrameTargets.Curr();
    auto& scene = InArgs.viewportPtr->GetTargets().SceneTargets.Curr();
    Renderer::DrawFullscreen(InArgs, InTarget, InShader, { &frame, &scene }, -1, false);
    stats.fullscreenPasses++;
    
    return stats;
}

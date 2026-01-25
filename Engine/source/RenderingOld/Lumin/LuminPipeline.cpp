#include "LuminPipeline.h"

#include "LuminRenderer.h"
#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbes(const RenderArgs &InArgs, bool InFallback)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InArgs.scenePtr, "Invalid scene");
    CHECK_ASSERT(!InArgs.viewportPtr, "Invalid viewport");
    CHECK_ASSERT(!InArgs.contextPtr, "Invalid context");
    
    Stats stats;
    
    if (InFallback)
    {
        stats += RenderSkybox(InArgs);
        stats += RenderDeferred(InArgs);
        stats += RenderSurfaces(InArgs);
    }
    else
    {
        stats += RenderSkybox(InArgs);
        stats += RenderScene(InArgs);
        stats += RenderParticles(InArgs);
        stats += RenderDeferred(InArgs);
        stats += RenderSurfaces(InArgs);
        stats += RenderLights(InArgs);
    }
    
    return stats;
}

Rendering::Pipeline::Stats Rendering::LuminPipeline::CollectSH(const RenderArgs& InArgs, const LuminChunkFrameData& InChunkData)
{
    PROFILE_GL();
    
    CHECK_ASSERT(!InChunkData.targets03, "Invalid target 0-3");
    CHECK_ASSERT(!InChunkData.targets48, "Invalid target 4-8");
    
    auto& sceneTarget = InArgs.viewportPtr->GetTargets().sceneTargets.Curr();
    auto& frameTarget = InArgs.viewportPtr->GetTargets().frameTargets.Curr();
    
    Stats stats;
    stats.fullscreenPasses += LuminRenderer::CollectSHCoefficients(InArgs, *InChunkData.targets03, 0, 3, { &sceneTarget, &frameTarget });
    stats.fullscreenPasses += LuminRenderer::CollectSHCoefficients(InArgs, *InChunkData.targets48, 4, 8, { &sceneTarget, &frameTarget });
    return stats;
}

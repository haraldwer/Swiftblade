#include "LightPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadows(const RenderArgs& InArgs, const ResShader& InShader)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    // TODO: Skip all shading
    
    Stats stats;
    stats += RenderScene(InArgs); 
    stats += RenderDeferred(InArgs);
    
    return stats;
}

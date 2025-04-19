#include "LightPipeline.h"

#include "Viewport/Viewport.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadowFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, int InFaceIndex)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    // TODO: Skip all shading
    
    Stats stats;
    stats += RenderScene(InArgs); 
    //stats += RenderDeferred(InArgs);

    auto scene = InArgs.Viewport->GetTargets().SceneTarget;
    Renderer::DrawCubeFace(InArgs, InTarget, InFaceIndex, InShader, { &scene }, -1, true);
    return stats;
}

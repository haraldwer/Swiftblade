#include "LightPipeline.h"

Rendering::Pipeline::Stats Rendering::LightPipeline::RenderShadowFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, int InFaceIndex)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    stats += RenderScene(InArgs); // TODO: Skip all shading
    //stats += RenderDeferred(InArgs); // TODO: Skip all shading

    auto scene = GetSceneTarget(InArgs);
    Renderer::DrawCubeFace(InArgs, InTarget, InFaceIndex, InShader, { &scene }, -1, true);
    return stats;
}

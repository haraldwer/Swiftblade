#include "LuminPipeline.h"

Rendering::Pipeline::Stats Rendering::LuminPipeline::RenderProbeFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const int InFaceIndex, const ResShader& InShader)
{
    CHECK_ASSERT(!InArgs.Scene, "Invalid scene");
    CHECK_ASSERT(!InArgs.Viewport, "Invalid viewport");
    CHECK_ASSERT(!InArgs.Context, "Invalid context");
    
    Stats stats;
    stats += RenderScene(InArgs);
    stats += RenderLumin(InArgs);
    stats += RenderDeferred(InArgs);

    auto frame = GetFrameTarget(InArgs);
    auto scene = GetSceneTarget(InArgs);
    Renderer::DrawCubeFace(InArgs, InTarget, InFaceIndex, InShader, { &frame, &scene });
    return stats;
}

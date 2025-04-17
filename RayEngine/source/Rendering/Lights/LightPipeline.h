#pragma once
#include "Pipeline/Pipeline.h"

namespace Rendering
{
    class LightPipeline : public Pipeline
    {
    public:
        Stats RenderShadowFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, int InFaceIndex);
    };
}

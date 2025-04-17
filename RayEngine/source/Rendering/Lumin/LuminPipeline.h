#pragma once
#include "Pipeline/Pipeline.h"

namespace Rendering
{
    class LuminPipeline : public Pipeline
    {
    public:
        Stats RenderProbeFace(const RenderArgs& InArgs, const RenderTarget& InTarget, const ResShader& InShader, bool InClear);
    };
}

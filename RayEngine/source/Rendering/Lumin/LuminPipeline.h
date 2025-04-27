#pragma once
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    class SwapTarget;

    class LuminPipeline : public Pipeline
    {
    public:
        Stats RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget);
    };
}

#pragma once
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    class SwapTarget;

    class LuminPipeline : public Pipeline
    {
    public:
        static Stats RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget);
        static Stats LerpProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget, SwapTarget& InLerpTarget);
    };
}

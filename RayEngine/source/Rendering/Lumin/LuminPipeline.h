#pragma once
#include "Rendering/Pipeline/Pipeline.h"
#include "TextureTargets/SwapTarget.h"

namespace Rendering
{

    class LuminPipeline : public Pipeline
    {
    public:
        static Stats RenderProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget);
        static Stats RenderFallbackProbe(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget);
        static Stats LerpProbes(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InFrame, SwapTarget& InTarget);
    };
}

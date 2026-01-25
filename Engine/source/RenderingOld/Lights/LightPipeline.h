#pragma once
#include "RenderingOld/Pipeline/Pipeline.h"

namespace Rendering
{
    class LightPipeline : public Pipeline
    {
    public:
        static Stats RenderShadows(const RenderArgs& InArgs, const ResShader& InShader, const RenderTarget& InTarget);
    };
}

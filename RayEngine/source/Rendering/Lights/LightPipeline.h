#pragma once
#include "Pipeline/Pipeline.h"

namespace Rendering
{
    class LightPipeline : public Pipeline
    {
    public:
        Stats RenderShadows(const RenderArgs& InArgs, const ResShader& InShader);
    };
}

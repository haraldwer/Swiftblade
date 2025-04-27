#pragma once
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    class LightPipeline : public Pipeline
    {
    public:
        Stats RenderShadows(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget);
    };
}

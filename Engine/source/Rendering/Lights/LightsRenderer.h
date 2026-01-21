#pragma once

#include "Pipeline/Renderer.h"

namespace Rendering
{
    class LightsRenderer : public Renderer
    {
    public:
        static int DrawLights(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
    };
}

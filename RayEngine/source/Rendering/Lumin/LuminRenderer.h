#pragma once

#include "Pipeline/Renderer.h"

namespace Rendering
{
    class LuminRenderer : public Renderer
    {
    public:
        static void ApplyLumin(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        static void BindBRDF(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        static int DrawLuminProbesDebug(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
    };
}

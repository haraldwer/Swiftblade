#pragma once

#include "Lumin.h"
#include "Pipeline/Renderer.h"

namespace Rendering
{
    class LuminTargets;

    class LuminRenderer : public Renderer
    {
    public:
        static int DrawLumin(const RenderArgs& InArgs, LuminTargets& InTarget, const Vector<RenderTarget*> &InBuffers);
        static void BindBRDF(const RenderArgs& InArgs, ShaderResource& InShader, int& InOutSlot);
        static int CollectSHCoefficients(const RenderArgs& InArgs, RenderTarget& InTarget, int InStartCoeff, int InEndCoeff, const Vector<RenderTarget*>& InBuffers);
        static int DrawLuminProbesDebug(const RenderArgs& InArgs, const RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers);
        
    private:
        static int DrawChunks(const RenderArgs& InArgs, const ResShader& InShader, RenderTarget& InTarget, const Vector<RenderTarget*>& InBuffers, const LuminData& InData);
    };
}

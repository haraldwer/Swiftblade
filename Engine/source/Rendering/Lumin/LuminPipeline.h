#pragma once
#include "LuminChunk.h"
#include "Rendering/Pipeline/Pipeline.h"

namespace Rendering
{
    struct LuminChunkFrameData;

    class LuminPipeline : public Pipeline
    {
    public:
        static Stats RenderProbes(const RenderArgs& InArgs, bool InFallback);
        static Stats CollectSH(const RenderArgs& InArgs, const LuminChunkFrameData& InChunkData);
    };
}

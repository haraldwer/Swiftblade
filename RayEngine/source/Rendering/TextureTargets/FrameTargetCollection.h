#pragma once
#include "SwapTarget.h"

namespace Rendering
{
    struct FrameTargetCollection
    {
        void Init(const RenderTexture& InTarget);
        void Deinit();

        OrderedMap<String, Vector<RenderTarget::TargetTex>> GetNamed();

        RenderTarget SceneTarget;
        SwapTarget AOTargets = SwapTarget();
        SwapTarget GITargets = SwapTarget();
        SwapTarget FireTargets = SwapTarget();
        RenderTarget FrameTarget;
        RenderTarget QuantizeTarget;
    };
}

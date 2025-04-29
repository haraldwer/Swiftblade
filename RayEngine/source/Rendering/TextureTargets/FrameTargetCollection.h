#pragma once
#include "SwapTarget.h"

namespace Rendering
{
    struct FrameTargetCollection
    {
        void Init(const RenderTexture& InTarget);
        void Deinit();

        OrderedMap<String, Vector<RenderTarget::TargetTex>> GetNamed();

        SwapTarget SceneTargets;
        SwapTarget AOTargets;
        SwapTarget FireTargets;
        SwapTarget FrameTargets;
    };
}

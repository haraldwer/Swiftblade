#pragma once
#include "SwapTarget.h"

namespace Rendering
{
    struct FrameTargetCollection
    {
        void Init(const RenderTexture& InTarget);
        void Deinit();

        Map<String, Vector<RenderTarget::TargetTex>> GetTargets();

        RenderTarget SceneTarget;
        SwapTarget SSAOTargets = SwapTarget(2);
        SwapTarget FireTargets = SwapTarget(2);
        RenderTarget FrameTarget;
        RenderTarget QuantizeTarget;
    };
}

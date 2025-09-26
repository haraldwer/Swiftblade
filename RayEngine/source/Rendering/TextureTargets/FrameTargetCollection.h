#pragma once
#include "SwapTarget.h"

namespace Rendering
{
    struct FXConfig;

    struct FrameTargetCollection
    {
        void Init(const RenderTexture& InTarget, const FXConfig& InFX);
        void Deinit();

        OrderedMap<String, Vector<RenderTarget::TargetTex>> GetNamed();

        SwapTarget sceneTargets = {};
        RenderTarget surfaceTarget = {};
        SwapTarget frameTargets = {};

        // Extras, depends on fx
        SwapTarget aoTargets = {};
        SwapTarget bloomTargets = {};
    };
}

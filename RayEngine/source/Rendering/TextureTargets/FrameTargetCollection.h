#pragma once
#include "SwapTarget.h"
#include "Lumin/LuminTargets.h"

namespace Rendering
{
    struct FXConfig;

    struct FrameTargetCollection
    {
        void Init(const RenderTexture& InTarget, const FXConfig& InFX, bool InCubemap = false);
        void Deinit();

        OrderedMap<String, Vector<RenderTarget::TargetTex>> GetNamed();

        SwapTarget sceneTargets = {};
        RenderTarget surfaceTarget = {};
        SwapTarget frameTargets = {};

        // Extras, depends on fx
        SwapTarget aoTargets = {};
        SwapTarget bloomTargets = {};
        LuminTargets luminTargets = {};
    };
}

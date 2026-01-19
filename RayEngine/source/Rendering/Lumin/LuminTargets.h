#pragma once
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    struct FXConfig;
    
    class LuminTargets
    {
    public:
        void Init(const RenderTexture& InTarget, const float InIrrScale, const float InRScale);
        void Deinit();
        
        RenderTarget irradianceTarget;
        RenderTarget radianceTarget;
        RenderTarget irradianceFrameTarget;
        RenderTarget radianceFrameTarget;
    };
}

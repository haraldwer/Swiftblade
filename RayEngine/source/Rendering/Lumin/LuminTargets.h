#pragma once
#include "TextureTargets/RenderTarget.h"

namespace Rendering
{
    struct FXConfig;
    
    class LuminTargets
    {
    public:
        void Init(const Vec2I& InRes, float InIrrScale, float InRScale);
        void Deinit();
        
        RenderTarget irradianceTarget;
        RenderTarget radianceTarget;
        RenderTarget irradianceFrameTarget;
        RenderTarget radianceFrameTarget;
    };
}

#pragma once
#include "RenderTarget.h"

namespace Rendering
{
    struct FrameTargets
    {
        void Init(const Vec2I& InSize);
        void Deinit();
        
        RenderTarget frame;
        RenderTarget depth;
    };
}

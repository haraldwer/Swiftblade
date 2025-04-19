#pragma once

#include "RenderTarget.h"

namespace Rendering
{
    class SwapTarget
    {
    public:
        SwapTarget(int InNum = 2)
        {
            CHECK_ASSERT(InNum < 2, "Too few targets");
            Targets.resize(InNum);
        }
        
        RenderTarget& Curr() { return Targets[Current]; }
        RenderTarget& Prev(int InOffset = 1) { return Targets[(Current + Targets.size() - InOffset) % Targets.size()]; }
        void Iterate() { Current = (Current + 1) % Targets.size(); }
        Vector<RenderTarget>& All() { return Targets; }
    private:
        Vector<RenderTarget> Targets;
        int Current = 0;
    };
}

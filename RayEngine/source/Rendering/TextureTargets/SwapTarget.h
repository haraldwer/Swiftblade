#pragma once

#include "RenderTarget.h"

namespace Rendering
{
    class SwapTarget
    {
    public:
        SwapTarget(const int InNum = 2)
        {
            CHECK_ASSERT(InNum < 2, "Too few targets");
            targets.resize(InNum);
        }
        
        RenderTarget& Curr() { return targets[current]; }
        RenderTarget& Prev(const int InOffset = 1) { return targets[(current + targets.size() - InOffset) % targets.size()]; }
        void Iterate() { current = (current + 1) % static_cast<int>(targets.size()); }
        Vector<RenderTarget>& All() { return targets; }
    private:
        Vector<RenderTarget> targets = {};
        int current = 0;
    };
}

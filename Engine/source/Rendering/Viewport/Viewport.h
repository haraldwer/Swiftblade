#pragma once

#include "Targets/FrameTargets.h"

namespace Rendering
{
    class Viewport
    {
    public:
        void Init(const Vec2I& InSize);
        void Deinit();

        void Resize(const Vec2I& InSize);
        Vec2I Size() const { return size; }
        FrameTargets& GetTargets() { return targets; }

    private:
        FrameTargets targets;
        Vec2I size;
    };
}

#pragma once

#include "Targets/RenderTarget.h"

namespace Rendering
{
    class Viewport
    {
    public:
        void Init(const Vec2I& InSize);
        void Deinit();

        void Resize(const Vec2I& InResolution);
        Vec2I Resolution() const { return resolution; }

    private:
        
        RenderTarget frameTarget;
        Vec2I resolution;
        
        // This class represents a "view" into the world
        // It contains all required targets
        // And can be used to draw the scene
    };
}

#pragma once

#include "BaseConfig.h"

namespace Rendering
{
    struct Config : BaseConfig<Config>
    {
        PROPERTY_D(int, Width, 1600);
        PROPERTY_D(int, Height, 900);
        PROPERTY_D(int, RenderSize, 0);
        PROPERTY_D(int, TargetFPS, 300);
        PROPERTY_D(bool, Fullscreen, false);
        PROPERTY_D(bool, VSync, false);
        PROPERTY_D(bool, MSAA, false);

        String Name() const override { return "Rendering"; };
    };
}

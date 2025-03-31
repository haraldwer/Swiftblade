#pragma once

namespace Rendering
{
    struct WindowConfig : PropertyOwner<WindowConfig>
    {
        PROPERTY_D(int, TargetFPS, 300);
        PROPERTY_D(bool, Fullscreen, false);
        PROPERTY_D(bool, VSync, false);
        PROPERTY_D(bool, MSAA, false);
        PROPERTY_D(int, Width, 1600);
        PROPERTY_D(int, Height, 900);
    };
}

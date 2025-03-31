#pragma once

namespace Rendering
{
    struct ViewportConfig : PropertyOwner<ViewportConfig>
    {
        PROPERTY_D(int, Width, 1600);
        PROPERTY_D(int, Height, 900);
        PROPERTY_D(int, RenderSize, 0);
    };
}

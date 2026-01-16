#pragma once

namespace Rendering
{
    struct ViewportConfig : PropertyOwner<ViewportConfig>
    {
        PROPERTY_D(int, Width, 0);
        PROPERTY_D(int, Height, 0);
        PROPERTY_D(int, RenderSize, 0);
        PROPERTY_D(bool, Cubemap, 0);
    };
}

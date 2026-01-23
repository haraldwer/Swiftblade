#pragma once

namespace Rendering
{
    struct WindowConfig : PropertyOwner<WindowConfig>
    {
        PROPERTY_D(Vec2I, Resolution, Vec2I(1600, 900));
        PROPERTY_D(String, Title, "Swiftblade");
    };
}

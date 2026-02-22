#pragma once

#include "Core/Property/PropertyOwner.h"

namespace Rendering
{
    struct WindowConfig : PropertyOwner<WindowConfig>
    {
        PROPERTY_D(Vec2I, Resolution, Vec2I(500, 900));
        PROPERTY_D(String, Title, "Swiftblade");
    };
}

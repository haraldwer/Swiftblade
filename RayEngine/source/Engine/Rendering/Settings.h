#pragma once
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

namespace Rendering
{
    struct Settings : PropertyOwner<Settings>
    {
        PROPERTY_P(int, Width, 1280);
        PROPERTY_P(int, Height, 720);
        PROPERTY_P(int, TargetFPS, 60);
        PROPERTY_P(bool, Fullscreen, false);
        PROPERTY_P(bool, VSync, false);
        PROPERTY_P(bool, MSAA, false);
    };
}

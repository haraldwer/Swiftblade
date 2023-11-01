#pragma once
#include "Engine/Property/Property.h"
#include "Engine/Property/PropertyOwner.h"

namespace Rendering
{
    struct Settings : PropertyOwner
    {
        PROPERTY_P(int, Width, 100);
        PROPERTY_P(int, Height, 100);
        PROPERTY_P(int, TargetFPS, 60);
        PROPERTY_P(bool, Fullscreen, false);
        
    };
}

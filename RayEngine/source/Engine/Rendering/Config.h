#pragma once
#include "..\Property\Property.h"
#include "Engine/Property/PropertyOwner.h"

namespace Rendering
{
    struct Config : PropertyOwner<Config>
    {
        PROPERTY_D(int, Width, 1280);
        PROPERTY_D(int, Height, 720);
        PROPERTY_D(int, TargetFPS, 60);
        PROPERTY_D(bool, Fullscreen, false);
        PROPERTY_D(bool, VSync, false);
        PROPERTY_D(bool, MSAA, false);

        inline static const String Path = "Configs/C_Rendering.json";
        void LoadConfig() { Load(Path); }
        void SaveConfig() const { Save(Path); }
    };
}
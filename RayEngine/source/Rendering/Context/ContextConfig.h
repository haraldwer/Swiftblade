#pragma once

#include "FXConfig.h"
#include "Lights/LightConfig.h"
#include "Lumin/LuminConfig.h"
#include "Resources/Model.h"
#include "Resources/NoiseTexture.h"

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY(Vector<String>, GlobalDefines)
        PROPERTY(StringMap<ResNoiseTex>, NoiseTextures)
        PROPERTY_D(ResModel, DefaultCube, "Defaults/M_Cube.obj");

        PROPERTY(LightConfig, Lights);
        PROPERTY(LuminConfig, Lumin);
        PROPERTY(FXConfig, FX);
    };
}

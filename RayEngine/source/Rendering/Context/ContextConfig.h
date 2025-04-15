#pragma once

#include "Rendering/Resources/NoiseTexture.h"
#include "Resources/Model.h"

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY(Vector<String>, GlobalDefines);
        PROPERTY(StringMap<ResNoiseTex>, NoiseTextures);
        PROPERTY_D(ResModel, DefaultCube, "Defaults/M_Cube.obj");
        PROPERTY_D(bool, Lumin, true);
    };
}

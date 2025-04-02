#pragma once

#include "Rendering/Resources/NoiseTexture.h"

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY(Vector<String>, GlobalDefines);
        PROPERTY(StringMap<ResNoiseTex>, NoiseTextures);
        PROPERTY_D(bool, Lumin, true);
    };
}

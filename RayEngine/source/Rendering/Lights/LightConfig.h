#pragma once

#include "Resources/Shader.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct LightConfig : PropertyOwner<LightConfig>
    {
        PROPERTY(ViewportConfig, Viewport);
        
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lights/SH_Collect.ps");
        PROPERTY_D(ResShader, LightShader, "Shaders/Lights/SH_Light.ps");

        PROPERTY_D(float, UpdateFrequency, 1.0f);
        PROPERTY_D(int, MaxShadowRenders, 0);
        PROPERTY_D(int, MaxShadows, 4);
        PROPERTY_D(int, MaxLights, 10);
    };
}
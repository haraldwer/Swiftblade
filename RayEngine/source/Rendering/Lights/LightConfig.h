#pragma once

#include "Resources/Shader.h"
#include "Resources/Model.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct LightConfig : PropertyOwner<LightConfig>
    {
        PROPERTY(ViewportConfig, Viewport);
        
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lights/SH_LightCollect.ps");
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lights/SH_LightDebug.fs");
        PROPERTY_D(ResShader, LightShader, "Shaders/Lights/SH_Light.ps");
        PROPERTY_D(ResModel, CubeModel, "Defaults/M_Cube.obj");

        PROPERTY_D(float, UpdateFrequency, 1.0f);
        PROPERTY_D(int, MaxShadowRenders, 0);
        PROPERTY_D(int, MaxShadows, 4);
        PROPERTY_D(int, MaxLights, 10);

        PROPERTY_D(bool, Debug, true);
    };
}
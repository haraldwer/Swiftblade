#pragma once

#include "Rendering/Resources/NoiseTexture.h"
#include "Resources/Model.h"
#include "Lights/LightConfig.h"

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY(Vector<String>, GlobalDefines);
        PROPERTY(StringMap<ResNoiseTex>, NoiseTextures);
        PROPERTY_D(ResModel, DefaultCube, "Defaults/M_Cube.obj");

        PROPERTY(LightConfig, Lights); // TODO: Move

        PROPERTY_D(ResShader, SSAOShader, "Shaders/PostProcessing/SH_SSAO.ps"); 
        PROPERTY_D(ResShader, QuantizeShader, "Shaders/PostProcessing/SH_Quantize.ps");
        PROPERTY_D(ResShader, FXAAShader, "Shaders/PostProcessing/SH_FXAA.ps");
        PROPERTY_D(ResShader, FireShader, "Shaders/PostProcessing/SH_Fire.ps");
        PROPERTY_D(ResShader, FireBlipShader, "Shaders/PostProcessing/SH_FireBlip.ps");
        
        PROPERTY_D(bool, Lumin, true);
    };
}

#pragma once

#include "Resources/Shader.h"

namespace Rendering
{
    struct FXConfig : PropertyOwner<FXConfig>
    {
        PROPERTY_D(bool, SSAO, false);
        PROPERTY_D(ResShader, SSAOShader, "Shaders/PostProcessing/SH_SSAO.ps");
        
        PROPERTY_D(bool, MotionBlur, false);
        PROPERTY_D(ResShader, MotionBlurShader, "Shaders/PostProcessing/SH_MotionBlur.ps");
        
        PROPERTY_D(bool, Quantize, false);
        PROPERTY_D(ResShader, QuantizeShader, "Shaders/PostProcessing/SH_Quantize.ps");

        PROPERTY_D(bool, FXAA, false);
        PROPERTY_D(ResShader, FXAAShader, "Shaders/PostProcessing/SH_FXAA.ps");

        PROPERTY_D(bool, Tonemapping, false);
        PROPERTY_D(ResShader, TonemappingShader, "Shaders/PostProcessing/SH_Tonemapping.ps");

        PROPERTY_D(bool, Distort, false);
        PROPERTY_D(ResShader, DistortShader, "Shaders/PostProcessing/SH_Distort.ps");

        PROPERTY_D(bool, Parallax, false);
        PROPERTY_D(ResShader, POMShader, "Shaders/PostProcessing/SH_POM.ps");

        PROPERTY_D(bool, Bump, false);
        PROPERTY_D(ResShader, BumpShader, "Shaders/PostProcessing/SH_Bump.ps");
    };
}

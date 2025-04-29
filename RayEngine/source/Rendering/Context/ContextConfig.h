#pragma once

#include "Lights/LightConfig.h"
#include "Rendering/Resources/NoiseTexture.h"
#include "Resources/Model.h"

namespace Rendering
{
    struct ContextConfig : PropertyOwner<ContextConfig>
    {
        PROPERTY(Vector<String>, GlobalDefines)
        PROPERTY(StringMap<ResNoiseTex>, NoiseTextures)
        PROPERTY_D(ResModel, DefaultCube, "Defaults/M_Cube.obj");

        PROPERTY(LightConfig, Lights); // TODO: Move

        PROPERTY_D(ResShader, DeferredSkyboxShader, "Shaders/SH_Default.ds");
        PROPERTY_D(ResShader, SSAOShader, "Shaders/PostProcessing/SH_SSAO.ps"); 
        PROPERTY_D(ResShader, MotionBlurShader, "Shaders/PostProcessing/SH_MotionBlur.ps");
        PROPERTY_D(ResShader, QuantizeShader, "Shaders/PostProcessing/SH_Quantize.ps");
        PROPERTY_D(ResShader, FXAAShader, "Shaders/PostProcessing/SH_FXAA.ps");
        PROPERTY_D(ResShader, TonemappingShader, "Shaders/PostProcessing/SH_Tonemapping.ps");
        PROPERTY_D(ResShader, DistortShader, "Shaders/PostProcessing/SH_Distort.ps");
        PROPERTY_D(ResShader, FireShader, "Shaders/PostProcessing/SH_Fire.ps");
        PROPERTY_D(ResShader, FireBlipShader, "Shaders/PostProcessing/SH_FireBlip.ps");
        PROPERTY_D(ResShader, ProcessSceneShader, "Shaders/SH_ProcessScene.ps");
        
        PROPERTY_D(bool, SSAO, true);
        PROPERTY_D(bool, MotionBlur, true);
        PROPERTY_D(bool, Quantize, true);
        PROPERTY_D(bool, FXAA, true);
        PROPERTY_D(bool, Tonemapping, true);
        PROPERTY_D(bool, Distort, true);
        PROPERTY_D(bool, Lumin, true);
    };
}

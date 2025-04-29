#pragma once

#include "Context/ContextConfig.h"
#include "Rendering/Resources/Shader.h"
#include "Resources/Model.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct LuminConfig : PropertyOwner<LuminConfig>
    {
        PROPERTY(ContextConfig, Context);
        PROPERTY(ViewportConfig, Viewport);
        
        PROPERTY_D(ResShader, LightingShader, "Shaders/Lumin/SH_Lumin.ps");
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lumin/SH_LuminCollect.ps");
        PROPERTY_D(ResShader, LerpShader, "Shaders/Lumin/SH_LuminLerp.ps");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        
        PROPERTY_D(Vec3F, Density, Vec3F(1.0f, 1.0f, 1.0f) * 0.11f);
        PROPERTY_D(float, RangeMultiplier, 1.5f);
        PROPERTY_D(float, CullMultiplier, 0.5f);
        PROPERTY_D(float, UpdateFrequency, 0.0f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxProbeRenders, 1);
        PROPERTY_D(int, MaxProbes, 15);
        PROPERTY_D(int, AtlasPadding, 10);
        
        PROPERTY_D(bool, Debug, false);
    };
}
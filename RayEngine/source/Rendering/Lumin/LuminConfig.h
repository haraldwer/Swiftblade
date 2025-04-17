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
        PROPERTY_D(ResShader, ProbeShader, "Shaders/Lumin/SH_LuminProbe.ps");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        
        PROPERTY_D(Vec3F, Density, Vec3F(0.75f, 1.0f, 0.75f) * 0.1f);
        PROPERTY_D(float, CloseCullDistance, 10.0f);
        PROPERTY_D(float, UpdateFrequency, 1.0f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxProbeRenders, 1);
        PROPERTY_D(float, ProbeFar, 50.0f);
        PROPERTY_D(int, MaxProbes, 30);
        
        PROPERTY_D(bool, Debug, false);
    };
}
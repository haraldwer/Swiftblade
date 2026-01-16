#pragma once

#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/BakedTexture.h"
#include "Resources/Model.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct LuminConfig : PropertyOwner<LuminConfig>
    {
        // Shaders
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lumin/SH_LuminCollect.ps");
        PROPERTY_D(ResShader, LerpShader, "Shaders/Lumin/SH_LuminLerp.ps");
        PROPERTY_D(ResBakedTex, TexBRDF, "Defaults/BT_BRDF.json"); // Move to material for custom BRDFs

        // SH probe grid
        PROPERTY_D(Vec3F, ProbeSize, Vec3F(1.0f));
        PROPERTY_D(int, SHResolution, 4096);
        
        // Probe rendering
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY_D(float, Far, 50.0f);
        PROPERTY_D(float, Near, 0.01f);

        // Probe update rules
        PROPERTY_D(int, FrameRenders, 1);
        
        // Debugging
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        PROPERTY_D(bool, Debug, false);
        PROPERTY_D(bool, Enabled, true);
    };
}
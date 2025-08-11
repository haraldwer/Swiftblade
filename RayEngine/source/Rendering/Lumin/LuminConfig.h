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

        // Shaders
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lumin/SH_LuminCollect.ps");
        PROPERTY_D(ResShader, LerpShader, "Shaders/Lumin/SH_LuminLerp.ps");
        PROPERTY_D(ResBakedTex, TexBRDF, "Defaults/BT_BRDF.json");

        // Probe locations and layering
        PROPERTY_D(Vec3F, BaseDensity, Vec3F(0.15f));
        PROPERTY_D(float, LayerScale, 0.5f);

        // Has to match SH_LumnSample.si!!
        PROPERTY_D(int, Layers, 1);
        PROPERTY_D(int, MaxLayerCount, 256 - 64 - 1); // Total = LayerCount * Layers (+ fallback)
        PROPERTY_D(int, AtlasPadding, 64); // For reusing nearby probes
        PROPERTY_D(int, MaxGridCount, 512); // For mapping grid -> probe

        // Probe update rules
        PROPERTY_D(float, UpdateFrequency, 0.0f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxFrameRenders, 1);
        PROPERTY_D(bool, ResetCache, false);
        
        // Debugging
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        PROPERTY_D(bool, Debug, false);
    };
}
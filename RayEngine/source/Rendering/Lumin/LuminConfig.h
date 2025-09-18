#pragma once

#include "Rendering/Resources/Shader.h"
#include "Rendering/Resources/BakedTexture.h"
#include "Resources/Model.h"
#include "Viewport/ViewportConfig.h"

namespace Rendering
{
    struct LuminConfig : PropertyOwner<LuminConfig>
    {
        PROPERTY(ViewportConfig, Viewport);

        // Shaders
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lumin/SH_LuminCollect.ps");
        PROPERTY_D(ResShader, LerpShader, "Shaders/Lumin/SH_LuminLerp.ps");
        PROPERTY_D(ResBakedTex, TexBRDF, "Defaults/BT_BRDF.json");

        // Probe locations and layering
        PROPERTY_D(Vec3F, BaseDensity, Vec3F(0.2f));
        PROPERTY_D(Vec3F, Offset, Vec3F(0.5f));
        PROPERTY_D(float, LayerScale, 0.5f);
        PROPERTY_D(float, Far, 25.0f);
        PROPERTY_D(float, Near, 0.01f);

        // Has to match SH_LumnSample.si!!
        PROPERTY_D(int, Layers, 1);
        PROPERTY_D(int, MaxLayerCount, 18 * 18 * 1 - 64 - 1); // LayerCount * Layers - padding - fallback 
        PROPERTY_D(int, AtlasPadding, 64); // For reusing nearby probes
        PROPERTY_D(int, MaxGridCount, 512); // For mapping grid -> probe

        // Probe update rules
        PROPERTY_D(float, UpdateFrequency, 0.0f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxFrameRenders, 1);
        PROPERTY_D(bool, SceneFallback, false);
        PROPERTY_D(bool, InvalidateFallback, false);
        PROPERTY_D(bool, InvalidateCache, false);
        
        // Debugging
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        PROPERTY_D(bool, Debug, false);
        PROPERTY_D(bool, Enabled, true);
    };
}
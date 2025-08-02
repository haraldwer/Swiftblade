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
        PROPERTY_D(Vec3F, BaseDensity, Vec3F(0.1f));
        PROPERTY_D(float, LayerScale, 0.5f);
        PROPERTY_D(int, Layers, 3);
        PROPERTY_D(int, MaxLayerProbes, 15); // Total = MaxLayer * Layers
        PROPERTY_D(int, AtlasPadding, 4); // For reusing nearby probes


        // Grid of probes
        // Select some probes
        // Can we assume grid?
        // Is every position valid?
        // What if not?
        // Fewer well-placed probes is better
        // Looping is not going to work
        // Selection should be a grid around camera
        // What about angle? Very efficient but a lot of math
        //
        

        // Probe update rules
        PROPERTY_D(float, UpdateFrequency, 0.1f);
        PROPERTY_D(int, Iterations, 0);
        PROPERTY_D(int, MaxFrameRenders, 1);
        
        // Debugging
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        PROPERTY_D(bool, Debug, false);
    };
}
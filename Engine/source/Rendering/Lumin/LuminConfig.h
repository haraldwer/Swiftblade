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
        PROPERTY_D(ResShader, RadianceShader, "Shaders/Lumin/SH_LuminRadiance.ps")
        PROPERTY_D(ResShader, IrradianceShader, "Shaders/Lumin/SH_LuminIrradiance.ps")
        PROPERTY_D(ResShader, CollectShader, "Shaders/Lumin/SH_LuminAccumulate.ps")
        PROPERTY_D(ResBakedTex, TexBRDF, "Defaults/BT_BRDF.json"); // Move to material for custom BRDFs

        // SH probe grid
        PROPERTY_D(Vec3F, CellSize, Vec3F(1.0f));
        PROPERTY_D(int, ChunkAxisCells, 32);
        
        // Probe rendering
        PROPERTY(ViewportConfig, Viewport);
        PROPERTY_D(float, Far, 50.0f);
        PROPERTY_D(float, Near, 0.01f);

        // Debugging
        PROPERTY_D(ResShader, DebugShader, "Shaders/Lumin/SH_LuminDebug.fs");
        PROPERTY_D(ResModel, SphereModel, "Defaults/M_Sphere.obj");
        PROPERTY_D(bool, Debug, false);
        PROPERTY_D(bool, Enabled, true);
    };
}
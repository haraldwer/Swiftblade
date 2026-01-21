#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Texture.h"

namespace Rendering
{
    struct EnvironmentInstance
    {
        ResRM skybox;
        ResTexture tonemap;
        Vec3F shape;
        Vec3F position;

        // Bounds?
        // Other FX parameters?? 
    };
}

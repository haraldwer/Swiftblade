#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Texture.h"

struct EnvironmentInstance
{
    ResRM Skybox;
    ResTexture Tonemap;
    Vec3F Shape;
    Vec3F Position;

    // Bounds?
    // Other FX parameters?? 
};

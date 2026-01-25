#pragma once

#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"
#include "Scene/Culling/VisibilityMask.h"

namespace Rendering
{
    struct MeshInstance
    {
        ResModel model;
        ResRM material;
        Mat4F transform;
        uint8 mask = static_cast<uint8>(VisibilityMask::ALL);
    };
}

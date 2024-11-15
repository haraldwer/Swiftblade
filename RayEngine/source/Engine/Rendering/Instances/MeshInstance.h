#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"

struct MeshInstance
{
    ResModel Model;
    ResRM Material;
    Mat4F Transform;
};

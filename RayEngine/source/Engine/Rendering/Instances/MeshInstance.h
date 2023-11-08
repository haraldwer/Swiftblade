#pragma once
#include "Engine/Rendering/Resources/Material.h"
#include "Engine/Rendering/Resources/Model.h"

struct MeshInstance
{
    ResModel Model;
    ResRM Material;
    Mat4F Transform;
};

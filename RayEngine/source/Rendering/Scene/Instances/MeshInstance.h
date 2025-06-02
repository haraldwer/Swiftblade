#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"

struct MeshInstance
{
    ResModel model;
    ResRM material;
    Mat4F transform;
    float extent = 1.0f;
};

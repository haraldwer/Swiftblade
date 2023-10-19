#pragma once
#include "raylib.h"

struct MeshInstance
{
    // Mesh resource
    // Material resource
    Mesh Mesh;
    Material Material;
    Matrix Transform;
};

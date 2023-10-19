#pragma once

#include <vector>

#include "raylib.h"
#include "Instances/MeshInstance.h"

class RenderScene
{

public:

    // - Logic interface - //
    void SetCamera(const Camera& InCamera) { Camera = InCamera; }
    const Camera& GetCamera() const { return Camera; }
    void AddMesh(const MeshInstance& InMesh) { Meshes.push_back(InMesh); }

    // - Render interface - // 
    void Render();

private:

    Camera Camera = {};
    std::vector<MeshInstance> Meshes;
    
};

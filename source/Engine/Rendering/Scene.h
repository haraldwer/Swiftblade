#pragma once

#include "Instances/CameraInstance.h"
#include "Instances/MeshInstance.h"

namespace Rendering
{
    class Scene
    {
    protected:
        CameraInstance Cam = {};
        Vector<MeshInstance> Meshes;
    };

    class LogicScene : public Scene
    {
    public:
        void SetCamera(const CameraInstance& InCamera) { Cam = InCamera; }
        void AddMesh(const MeshInstance& InMesh) { Meshes.push_back(InMesh); }
    };

    class RenderScene : public Scene
    {
    public:
        void Render();
    };
}


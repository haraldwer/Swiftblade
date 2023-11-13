#pragma once

#include "Debug/DebugShape.h"
#include "Instances/CameraInstance.h"
#include "Instances/MeshInstance.h"

namespace Rendering
{
    class Scene
    {
    protected:
        CameraInstance Cam = {};
        Vector<MeshInstance> Meshes;
        Vector<DebugShape> DebugShapes; 
    };

    class LogicScene : public Scene
    {
    public:
        void SetCamera(const CameraInstance& InCamera) { Cam = InCamera; }
        const CameraInstance& GetCamera() const { return Cam; }
        void AddMesh(const MeshInstance& InMesh) { Meshes.push_back(InMesh); }
        void AddDebugShape(const DebugShape& InShape) { DebugShapes.push_back(InShape); }
    };

    class RenderScene : public Scene
    {
    public:
        void Render();
    };
}


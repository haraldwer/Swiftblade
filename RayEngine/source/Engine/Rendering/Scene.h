#pragma once

#include "Debug/DebugShape.h"
#include "Instances/CameraInstance.h"
#include "Instances/MeshInstance.h"

namespace Rendering
{
    struct MeshCollection
    {
        void AddMesh(const MeshInstance& InInstance);
        void AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms);

        struct Entry {
            Vector<Matrix> Transforms;
            ResModel Model;
            ResRM Material;
            bool Initialized = false;
        };
        Map<uint64, Entry> Entries;
        
    private:
        Entry& GetEntry(const MeshInstance& InInstance);
    };
    
    class Scene
    {
    protected:
        CameraInstance Cam = {};
        MeshCollection Meshes;
        Vector<DebugShape> DebugShapes;
    };

    class LogicScene : public Scene
    {
    public:
        void SetCamera(const CameraInstance& InCamera) { Cam = InCamera; }
        const CameraInstance& GetCamera() const { return Cam; }
        void AddMesh(const MeshInstance& InMesh) { Meshes.AddMesh(InMesh); }
        void AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms) { Meshes.AddMeshes(InMesh, InTransforms); }
        void AddDebugShape(const DebugShape& InShape) { DebugShapes.push_back(InShape); }
    };

    class RenderScene : public Scene
    {
    public:
        void Render();
    };
}


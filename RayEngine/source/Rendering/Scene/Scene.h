#pragma once

#include "Culling/Frustum.h"
#include "Instances/CameraInstance.h"
#include "Instances/DebugShape.h"
#include "Instances/MeshInstance.h"

namespace Rendering
{
    struct MeshCollection
    {
        void AddMesh(const MeshInstance& InInstance);
        void AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms);

        struct Entry {
            Vector<Mat4F> Transforms;
            ResModel Model;
            ResRM Material;
            uint32 DeferredID = 0;
            bool Initialized = false;
        };
        Map<uint64, Entry> Entries;
        Map<uint32, ResShader> DeferredShaders;
        
    private:
        Entry& GetEntry(const MeshInstance& InInstance);
    };
    
    class Scene
    {
        friend class Renderer; 
    public:
        void SetCamera(const CameraInstance& InCamera);
        const CameraInstance& GetCamera() const { return Cam; }
        
        void AddMesh(const MeshInstance& InMesh);
        void AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd);
        void AddDebugShape(const DebugShape& InShape);
        void AddDebugLine(const DebugLine& InLine);

        void Clear();
        uint32 Count() const;

    private:
        CameraInstance Cam = {};
        MeshCollection Meshes;
        Vector<DebugShape> DebugShapes;
        Vector<DebugLine> DebugLines;
        Frustum Frustum;
    };
}


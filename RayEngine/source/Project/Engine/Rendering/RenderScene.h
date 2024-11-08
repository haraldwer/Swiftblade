#pragma once

#include "Culling/Frustum.h"
#include "Debug/DebugShapeInstance.h"
#include "Instances/CameraInstance.h"
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
    
    class RenderScene
    {
        friend class SceneRenderer; 
    public:
        void SetCamera(const CameraInstance& InCamera);
        void SetTime(double InTime);
        const CameraInstance& GetCamera() const { return Cam; }
        
        void AddMesh(const MeshInstance& InMesh);
        void AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd);
        void AddDebugShape(const DebugShapeInstance& InShape);
        void AddDebugLine(const DebugLineInstance& InLine);

        void Clear();
        
    private:
        CameraInstance Cam = {};
        MeshCollection Meshes;
        Vector<DebugShapeInstance> DebugShapes;
        Vector<DebugLineInstance> DebugLines;
        Frustum Frustum;
        double Time = 0.0f; 
        
    };
}


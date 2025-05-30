#pragma once

#include "Culling/Frustum.h"
#include "Instances/CameraInstance.h"
#include "Instances/DebugShape.h"
#include "Instances/EnvironmentInstance.h"
#include "Instances/LightInstance.h"
#include "Instances/MeshInstance.h"
#include "Utility/Collections/SpatialContainer.h"

namespace Rendering
{
    struct MeshCollection
    {
        void AddMesh(const MeshInstance& InInstance);
        void AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms);
        
        struct Entry {
            Utility::SpatialContainer<Mat4F> Transforms;
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
        friend class Lumin;
        friend class Lights;
    public:
        void SetCamera(const CameraInstance& InCamera);
        const CameraInstance& GetCamera() const { return MainCamera; }
        
        void AddEnvironment(const EnvironmentInstance& InEnvironment);
        void AddMesh(const MeshInstance& InMesh);
        void AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd);
        void AddLight(const LightInstance& InLight);
        void AddDebugShape(const DebugShape& InShape);
        void AddDebugLine(const DebugLine& InLine);

        void Clear();
        void Build();
        uint32 Count() const;

    private:
        CameraInstance MainCamera = {};
        Vector<EnvironmentInstance> Environments;
        MeshCollection Meshes;
        Utility::SpatialContainer<LightInstance> Lights;
        Utility::SpatialContainer<DebugShape> DebugShapes;
        Utility::SpatialContainer<DebugLine> DebugLines;
    };
}


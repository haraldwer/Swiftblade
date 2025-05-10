#include "Scene.h"

using namespace Rendering;

void Scene::Clear()
{
    Meshes = {};
    Environments = {};
    Lights.Clear();
    DebugShapes.Clear();
    DebugLines.Clear();
}

void Scene::Build()
{
    for (auto& e : Meshes.Entries)
        e.second.Transforms.Build();
    Lights.Build();
    DebugShapes.Build();
    DebugLines.Build();
}

uint32 Scene::Count() const
{
    uint32 c = 0;
    for (auto& e : Meshes.Entries)
        c += e.second.Transforms.Count();
    return c;
}

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.Transforms.Insert(InInstance.Transform, {
        .Position = InInstance.Transform.GetPosition(),
        .Extent = InInstance.Extent
    });
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    for (auto& t : InTransforms)
    {
        entry.Transforms.Insert(t, {
            .Position = InInstance.Transform.GetPosition(),
            .Extent = InInstance.Extent
        });
    }
}

MeshCollection::Entry& MeshCollection::GetEntry(const MeshInstance& InInstance)
{
    union Union
    {
        struct Res
        {
            uint32 MaterialHash;
            uint32 ModelHash;
        } Resource;
        uint64 Key = 0;
    } hash;

    // Create hash
    auto mat = InInstance.Material.Get();
    if (mat)
        hash.Resource.MaterialHash = mat->Hash();
    if (auto model = InInstance.Model.Get())
        hash.Resource.ModelHash = model->Hash();

    // Add entry
    Entry& entry = Entries[hash.Key];
    if (!entry.Initialized)
    {
        entry.Material = InInstance.Material;
        entry.Model = InInstance.Model;
        entry.Initialized = true;
        
        // Add to deferred
        // Deferred is separate because multiple meshes might share the same material hash
        if (mat)
        {
            entry.DeferredID = mat->DeferredHash();
            if (!DeferredShaders.contains(entry.DeferredID) && entry.DeferredID != 0 && mat)
                DeferredShaders[entry.DeferredID] = mat->DeferredShader.Get();
        }
    }
    
    return entry;
}

void Scene::SetCamera(const CameraInstance& InCamera)
{
    MainCamera = InCamera;
}

void Scene::AddEnvironment(const EnvironmentInstance& InEnvironment)
{
    Environments.push_back(InEnvironment);
}

void Scene::AddMesh(const MeshInstance& InMesh)
{ 
    Meshes.AddMesh(InMesh);
}

void Scene::AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd)
{
    Meshes.AddMeshes(InMesh, InTransforms);
}

void Scene::AddLight(const LightInstance& InLight)
{
    Lights.Insert(InLight, {
        .Position = InLight.Data.Position,
        .Extent = InLight.Data.Range
    }); 
}

void Scene::AddDebugShape(const DebugShape& InShape)
{
    DebugShapes.Insert(InShape,{
        .Position = InShape.Pos,
        .Extent = InShape.Data.r
    });
}

void Scene::AddDebugLine(const DebugLine& InLine)
{
    const Vec3F diff = (InLine.End - InLine.Start)/2; 
    DebugLines.Insert(InLine,{
        .Position = diff,
        .Extent = diff.Length()
    });
}

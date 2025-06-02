#include "Scene.h"

using namespace Rendering;

void Scene::Clear()
{
    PROFILE();
    meshes = {};
    environments = {};
    lights.Clear();
    debugShapes.Clear();
    debugLines.Clear();
}

void Scene::Build()
{
    for (auto& e : meshes.entries)
        e.second.transforms.Build();
    lights.Build();
    debugShapes.Build();
    debugLines.Build();
}

uint32 Scene::Count() const
{
    uint32 c = 0;
    for (auto& e : meshes.entries)
        c += e.second.transforms.Count();
    return c;
}

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.transforms.Insert(InInstance.transform, {
        .position = InInstance.transform.GetPosition(),
        .extent = InInstance.extent
    });
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    for (auto& t : InTransforms)
    {
        entry.transforms.Insert(t, {
            .position = InInstance.transform.GetPosition(),
            .extent = InInstance.extent
        });
    }
}

MeshCollection::Entry& MeshCollection::GetEntry(const MeshInstance& InInstance)
{
    union Union
    {
        struct Res
        {
            uint32 materialHash;
            uint32 modelHash;
        } resource;
        uint64 key = 0;
    } hash;

    // Create hash
    const auto mat = InInstance.material.Get();
    if (mat)
        hash.resource.materialHash = mat->Hash();
    if (const auto model = InInstance.model.Get())
        hash.resource.modelHash = model->Hash();

    // Add entry
    Entry& entry = entries[hash.key];
    if (!entry.initialized)
    {
        entry.material = InInstance.material;
        entry.model = InInstance.model;
        entry.initialized = true;
        
        // Add to deferred
        // Deferred is separate because multiple meshes might share the same material hash
        if (mat)
        {
            entry.deferredID = mat->DeferredHash();
            if (!deferredShaders.contains(entry.deferredID) && entry.deferredID != 0 && mat)
                deferredShaders[entry.deferredID] = mat->DeferredShader.Get();
        }
    }
    
    return entry;
}

void Scene::SetCamera(const CameraInstance& InCamera)
{
    mainCamera = InCamera;
}

void Scene::AddEnvironment(const EnvironmentInstance& InEnvironment)
{
    environments.push_back(InEnvironment);
}

void Scene::AddMesh(const MeshInstance& InMesh)
{ 
    meshes.AddMesh(InMesh);
}

void Scene::AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd)
{
    meshes.AddMeshes(InMesh, InTransforms);
}

void Scene::AddLight(const LightInstance& InLight)
{
    lights.Insert(InLight, {
        .position = InLight.data.position,
        .extent = InLight.data.range
    }); 
}

void Scene::AddDebugShape(const DebugShape& InShape)
{
    debugShapes.Insert(InShape,{
        .position = InShape.pos,
        .extent = InShape.data.r
    });
}

void Scene::AddDebugLine(const DebugLine& InLine)
{
    const Vec3F diff = (InLine.end - InLine.start)/2; 
    debugLines.Insert(InLine,{
        .position = diff,
        .extent = diff.Length()
    });
}

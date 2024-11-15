#include "RenderScene.h"

#include "Utility/Hash.h"

using namespace Rendering;

void RenderScene::BeginFrame()
{
    double newTime = Utility::Time::Get().Total();
    Delta = newTime - Time; 
    Time = newTime;
}

void RenderScene::Clear()
{
    Cam = {};
    Meshes = {};
    DebugShapes = {};
    DebugLines = {};
}

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.Transforms.push_back(InInstance.Transform);
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    entry.Transforms.reserve(entry.Transforms.size() + InTransforms.size());
    entry.Transforms.insert(entry.Transforms.end(), InTransforms.begin(), InTransforms.end());
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

void RenderScene::SetCamera(const CameraInstance& InCamera)
{
    Cam = InCamera;
    Frustum.ConstructFrustum(InCamera);
}

void RenderScene::AddMesh(const MeshInstance& InMesh)
{
    const Vec3F scale = InMesh.Transform.GetScale();
    const float maxScale = Utility::Math::Max(Utility::Math::Max(scale.x, scale.y), scale.z);
    if (Frustum.CheckSphere(InMesh.Transform.GetPosition(), maxScale))
        Meshes.AddMesh(InMesh);
}

void RenderScene::AddMeshes(const MeshInstance& InMesh, const Vector<Mat4F>& InTransforms, const Vec3F& InBoxStart, const Vec3F& InBoxEnd)
{
    if (InBoxStart != Vec3F::Zero() || InBoxEnd != Vec3F::Zero())
    {
        const Vec3F halfDiff = (InBoxEnd - InBoxStart) * 0.5f;
        const Vec3F center = InBoxStart + halfDiff;
        if (!Frustum.CheckBox(center.x, center.y, center.z, halfDiff.x, halfDiff.y, halfDiff.z))
            return;
    }
    Meshes.AddMeshes(InMesh, InTransforms);
}

void RenderScene::AddDebugShape(const DebugShapeInstance& InShape)
{
    if (Frustum.CheckPoint(InShape.Pos))
        DebugShapes.push_back(InShape);
}

void RenderScene::AddDebugLine(const DebugLineInstance& InLine)
{
    const Vec3F diff = InLine.End - InLine.Start; 
    if (Frustum.CheckCube(InLine.Start + diff * 0.5f, diff.Length()))
        DebugLines.push_back(InLine);
}

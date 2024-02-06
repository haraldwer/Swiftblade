#include "RenderScene.h"

#include "Utility/Hash.h"

using namespace Rendering;

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
    const String matID = InInstance.Material.Identifier();
    const String modelID = InInstance.Model.Identifier();

    union Union
    {
        struct Res
        {
            uint32 Material;
            uint32 Model;
        } Resource;
        uint64 Key = 0;
    } hash;
    
    hash.Resource.Material = Utility::Hash(matID); 
    hash.Resource.Model = Utility::Hash(modelID);
    
    Entry& entry = Entries[hash.Key];
    if (!entry.Initialized)
    {
        entry.Material = InInstance.Material;
        entry.Model = InInstance.Model;
        entry.Initialized = true; 
    }
    return entry;
}

void RenderScene::SetCamera(const CameraInstance& InCamera, bool InUpdateFrustum)
{
    Cam = InCamera;
    if (InUpdateFrustum)
        Frustum.ConstructFrustum(InCamera);
}

void RenderScene::AddMesh(const MeshInstance& InMesh)
{
    const Vec3F scale = InMesh.Transform.GetScale();
    const float maxScale = MAX(MAX(scale.x, scale.y), scale.z);
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

void RenderScene::Clear()
{
    Cam = {};
    Meshes = {};
    DebugShapes = {};
    DebugLines = {};
}

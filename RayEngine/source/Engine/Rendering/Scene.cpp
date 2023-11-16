#include "Scene.h"

#include "Utility/Hash.h"
#include "Utility/RayUtility.h"

using namespace Rendering;

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.Transforms.push_back(Utility::Ray::ConvertMat(InInstance.Transform));
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    entry.Transforms.reserve(entry.Transforms.size() + InTransforms.size());
    for (const Mat4F& trans : InTransforms)
        entry.Transforms.push_back(Utility::Ray::ConvertMat(trans));
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

void RenderScene::Render()
{
    BeginMode3D(Utility::Ray::ConvertCamera(Cam));
    
    // TODO: Sub-tick camera movement
    // TODO: Depth sorting

    // Instanced rendering
    for (auto& entry : Meshes.Entries)
    {
        const Mesh* meshes = nullptr;
        int32 meshCount = 0;
        if (const auto resModel = entry.second.Model.Get())
        {
            if (const auto rlModel = resModel->Get())
            {
                if (rlModel->meshCount > 0)
                {
                    meshes = rlModel->meshes;
                    meshCount = rlModel->meshCount;
                }
            }
        }

        const Material* material = nullptr;
        if (const auto resMat = entry.second.Material.Get())
            if (const auto rlMat = resMat->Get())
                material = rlMat;

        const Matrix* transforms = entry.second.Transforms.data();
        const int tCount = static_cast<int>(entry.second.Transforms.size());

        CHECK_CONTINUE(!meshes);
        CHECK_CONTINUE(meshCount == 0);
        CHECK_CONTINUE(!material);
        CHECK_CONTINUE(!transforms);
        CHECK_CONTINUE(tCount == 0);
        
        for (int i = 0; i < meshCount; i++)
            DrawMeshInstanced(meshes[i], *material, transforms, tCount);
    }
    
    DrawGrid(10, 10.0f);

    for (auto& shape : DebugShapes)
    {
        switch (shape.Type)
        {
        case DebugShape::Type::SPHERE:
            DrawSphereWires(
                Utility::Ray::ConvertVec(shape.Pos),
                shape.Data.x,
                static_cast<int>(shape.Data.y),
                static_cast<int>(shape.Data.z),
                shape.Color);
            break;
        case DebugShape::Type::BOX:
            DrawCubeWiresV(
                Utility::Ray::ConvertVec(shape.Pos),
                Utility::Ray::ConvertVec(shape.Data),
                shape.Color);
            break;
        case DebugShape::Type::CAPSULE:
            const Vec3F dir = Mat4F(shape.Rot).Right() * shape.Data.y;
            const auto start = Utility::Ray::ConvertVec(shape.Pos + dir);
            const auto end = Utility::Ray::ConvertVec(shape.Pos - dir);
            DrawCapsuleWires(
                start,
                end,
                shape.Data.x,
                static_cast<int>(shape.Data.z),
                static_cast<int>(shape.Data.z) / 2,
                shape.Color);
            break;
        }
    }

    EndMode3D();
}

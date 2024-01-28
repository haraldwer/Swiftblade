#include "RenderScene.h"

#include "Utility/Hash.h"

using namespace Rendering;

void MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    entry.Transforms.push_back(InInstance.Transform);
    // TODO: Culling
}

void MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);
    entry.Transforms.reserve(entry.Transforms.size() + InTransforms.size());
    entry.Transforms.insert(entry.Transforms.end(), InTransforms.begin(), InTransforms.end());
    // TODO: Culling
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

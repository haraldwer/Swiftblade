#include "Mesh.h"

#include "Instance/Instance.h"
#include "Rendering/Scene/Instances/MeshInstance.h"
#include "Transform.h"

using namespace ECS;

void SysMesh::SystemFrame()
{
    diff.Clear();

    for (const auto& id : translation)
    {
        Mesh& m = GetInternal(id.second);
        const Transform& t = Get<Transform>(id.first);
        const Mat4F world = t.World();
        const uint64 hash = MeshInstance::GenHash(m.Model, m.Material);
        
        if (world == m.worldCache && hash == m.hashCache)
        {
            diff.Touch(hash);
        }
        else
        {
            diff.Modify(hash);
            m.worldCache = world;
            m.hashCache = hash;
            hashToEntity[hash].insert(id.second);
        }
    }

    auto& rs = Engine::Instance::Get().GetRenderScene();

    // Remove persistence for unused hashes
    for (uint64 hash : diff.GetRemoved())
    {
        rs.Meshes().ClearPersistence(hash);
        hashToPersistence.erase(hash);
        hashToEntity.erase(hash);
    }
    
    // For every modified hash
    for (uint64 hash : diff.GetModified())
    {
        // Maybe generate persistence
        if (!hashToPersistence.contains(hash))
            hashToPersistence[hash] = MeshInstance::GenPersistentID();
        uint32 persistenceID = hashToPersistence.at(hash);
        
        // For every entity now using that hash
        for (EntityID id : hashToEntity.at(hash))
        {
            Mesh& m = GetInternal(id);
            rs.Meshes().AddMesh({
                .model= m.Model,
                .material= m.Material,
                .transform= m.worldCache,
                .hash= hash,
                .persistentID= persistenceID
            });
        }
    }
}
#include "Mesh.h"

#include "Transform.h"
#include "Instance/Instance.h"

using namespace ECS;

void SysMesh::SystemInit()
{
    persistentID = 0;// TODO:
}

void SysMesh::SystemFrame()
{
    bool isEditor = Engine::Instance::Get().IsEditor();
    
    for (const auto& id : ComponentMap())
    {
        Mesh& m = GetInternal(id.first);
        CHECK_CONTINUE(isEditor && !m.EditorVisible);
        CHECK_CONTINUE(!isEditor && !m.GameVisible);
        
        const Transform* t = TryGet<Transform>(id.second);
        CHECK_CONTINUE(!t);
        
        const Mat4F world = t->World();
        const uint64 hash = 0; // TODO: Rendering::MeshInstance::GenHash(m.Model, m.Material);
        CHECK_CONTINUE(hash == 0);
        
        if (world == m.worldCache && hash == m.hashCache)
        {
            diff.Touch(hash);
        }
        else
        {
            diff.Modify(hash);
            m.worldCache = world;
            m.hashCache = hash;
            hashToComponent[hash].insert(id.first);
        }
    }

    // TODO:
    /*
    Rendering::Scene &rs = Engine::Instance::Get().GetRenderScene();

    // Remove persistence for unused hashes
    for (uint64 hash : diff.GetRemoved())
    {
        rs.Meshes().Remove(hash, persistentID);
        hashToComponent.erase(hash);
    }
    
    // For every modified hash (including where entities were removed)
    for (uint64 hash : diff.GetModified())
    {
        rs.Meshes().Remove(hash, persistentID);
        
        // For every entity now using that hash
        for (ComponentID id : hashToComponent.at(hash))
        {
            Mesh& m = GetInternal(id);
            rs.Meshes().Add({
                .model= m.Model,
                .material= m.Material,
                .transform= m.worldCache,
                .hash= hash,
                .mask= m.Mask
            }, persistentID);
        }
    }
    */
    
    // Begin diff for next frame
    diff.Begin();
}

void SysMesh::Deinit(const EntityID InID, Mesh &InComponent)
{
    // Include in diff
    if (InComponent.hashCache != 0)
    {
        diff.Modify(InComponent.hashCache);
        const ComponentID id = Translate(InID);
        hashToComponent.at(InComponent.hashCache).erase(id);
    }
    
    System::Deinit(InID, InComponent);
}

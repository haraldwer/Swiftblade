#include "MeshCollection.h"

void Rendering::MeshCollection::ClearAll()
{
    entries.clear();
    deferredShaders.clear(); 
}

void Rendering::MeshCollection::ClearNonPersistent()
{
    for (auto& persistence : entries)
    {
        CHECK_CONTINUE(persistence.first != 0)
        persistence.second.clear();
    }
}

void Rendering::MeshCollection::Build()
{
    for (auto& p : entries)
    {
        for (auto& e : p.second)
        {
            if (e.second.transforms.IsBuilt())
            {
                e.second.justRebuilt = false;
                continue;
            }
            e.second.transforms.Build();
            e.second.justRebuilt = true;
        }
    }
}

void Rendering::MeshCollection::Add(const MeshInstance& InInstance, const uint32 InPersistentID)
{
    Entry &entry = GetEntry(InInstance, InPersistentID);
    CHECK_ASSERT(entry.transforms.IsBuilt(), "Clear before adding");
    entry.transforms.Insert(InInstance.transform, {
        .position = InInstance.transform.GetPosition(),
        .extent = InInstance.extent
    });
}

void Rendering::MeshCollection::Add(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms, const uint32 InPersistentID)
{
    Entry& entry = GetEntry(InInstance, InPersistentID);
    CHECK_ASSERT(entry.transforms.IsBuilt(), "Clear before adding")
    for (auto& t : InTransforms)
        entry.transforms.Insert(t, {
            .position = t.GetPosition(),
            .extent = InInstance.extent
        });
}

void Rendering::MeshCollection::Remove(const uint64 InHash, const uint32 InPersistenceID)
{
    CHECK_ASSERT(InPersistenceID == 0, "Cannot remove non-persistent objects");
    if (entries.contains(InPersistenceID))
    {
        auto& persistence = entries.at(InPersistenceID);
        if (persistence.contains(InHash))
            persistence.erase(InHash);
    }
}

Rendering::MeshCollection::Entry& Rendering::MeshCollection::GetEntry(const MeshInstance& InInstance, const uint32 InPersistent)
{
    CHECK_ASSERT(InInstance.hash == 0, "Invalid hash");
    
    // Get / add entry
    auto& persistence = entries[InPersistent];
    Entry& entry = persistence[InInstance.hash];
    if (!entry.initialized)
    {
        entry.material = InInstance.material;
        entry.model = InInstance.model;
        entry.initialized = true;
        
        // Add to deferred
        // Deferred is separate because multiple meshes might share the same material hash
        if (auto mat = entry.material.Get())
        {
            entry.deferredID = mat->DeferredHash();
            if (!deferredShaders.contains(entry.deferredID) && entry.deferredID != 0)
                deferredShaders[entry.deferredID] = mat->DeferredShader.Get();
        }
    }
    
    return entry;
}

#include "MeshCollection.h"

void Rendering::MeshCollection::Clear()
{
    Set<uint64> toBeRemoved; 
    for (auto& e : entries)
    {
        e.second.justRebuilt = false;
        if (e.second.persistentID == 0 || // No cache
            e.second.persistentID != e.second.prevPersistentID) // Cache changed
            toBeRemoved.insert(e.first);
    }
    for (auto& e : toBeRemoved)
        entries.erase(e);
}

void Rendering::MeshCollection::Build()
{
    for (auto& e : entries)
    {
        // Different from prev and not zero
        if (e.second.persistentID != e.second.prevPersistentID || e.second.persistentID == 0)
        {
            e.second.prevPersistentID = e.second.persistentID;
            e.second.transforms.Build();
            e.second.justRebuilt = true;
        }
    }
}

bool Rendering::MeshCollection::UpdatePersistence(Entry& InEntry, const MeshInstance& InInstance)
{
    // Persistent and equal
    if (InEntry.persistentID != 0 &&
        InEntry.persistentID == InInstance.persistentID &&
        InEntry.persistentID == InEntry.prevPersistentID)
        return true;

    // New persistent ID
    if (InEntry.persistentID != InInstance.persistentID)
        InEntry.persistentID = InInstance.persistentID;

    return false;
}

void Rendering::MeshCollection::AddMesh(const MeshInstance& InInstance)
{
    auto& entry = GetEntry(InInstance);
    
    if (UpdatePersistence(entry, InInstance))
        return;
    
    entry.transforms.Insert(InInstance.transform, {
        .position = InInstance.transform.GetPosition(),
        .extent = InInstance.extent
    });
}

void Rendering::MeshCollection::AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms)
{
    Entry& entry = GetEntry(InInstance);

    if (UpdatePersistence(entry, InInstance))
        return;
    
    for (auto& t : InTransforms)
    {
        entry.transforms.Insert(t, {
            .position = t.GetPosition(),
            .extent = InInstance.extent
        });
    }
}

void Rendering::MeshCollection::ClearPersistence(uint64 InHash)
{
    if (entries.contains(InHash))
        entries.at(InHash).persistentID = 0;
}

Rendering::MeshCollection::Entry& Rendering::MeshCollection::GetEntry(const MeshInstance& InInstance)
{
    CHECK_ASSERT(InInstance.hash == 0, "Invalid hash");
    
    // Get / add entry
    Entry& entry = entries[InInstance.hash];
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
            if (!deferredShaders.contains(entry.deferredID) && entry.deferredID != 0 && mat)
                deferredShaders[entry.deferredID] = mat->DeferredShader.Get();
        }
    }
    
    return entry;
}

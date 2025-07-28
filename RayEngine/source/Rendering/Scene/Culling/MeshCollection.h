#pragma once

#include "Scene/Instances/MeshInstance.h"
#include "Utility/Collections/SplitContainer.h"

namespace Rendering
{
    struct MeshCollection
    {
        void ClearAll();
        void ClearNonPersistent();
        void Build();
        
        void Add(const MeshInstance& InInstance, uint32 InPersistentID = 0);
        void Add(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms, uint32 InPersistentID = 0);
        void Remove(uint64 InHash, uint32 InPersistenceID);
        
        Map<uint32, ResShader> GetDeferredShaders() const { return deferredShaders; }

        struct Entry
        {
            Utility::SplitContainer<Mat4F> transforms;
            
            ResModel model;
            ResRM material;
            uint8 mask = 0;
            uint32 deferredID = 0;
            bool initialized = false;
            bool justRebuilt = false;
        };
        
        const Map<uint32, Map<uint64, Entry>>& GetEntries() const { return entries; }

    private:

        //  persistence -> hash -> entry
        Map<uint32, Map<uint64, Entry>> entries;
        Map<uint32, ResShader> deferredShaders;
        
        Entry& GetEntry(const MeshInstance& InInstance, uint32 InPersistent);
    };
}
    
#pragma once

#include "Scene/Instances/MeshInstance.h"
#include "Utility/Collections/SplitContainer.h"

namespace Rendering
{
    struct MeshCollection
    {
        void Clear();
        void Build();
        
        void AddMesh(const MeshInstance& InInstance);
        void AddMeshes(const MeshInstance& InInstance, const Vector<Mat4F>& InTransforms);
        void ClearPersistence(uint64 InHash);
        Map<uint32, ResShader> GetDeferredShaders() const { return deferredShaders; }

        struct Entry
        {
            Utility::SplitContainer<Mat4F> transforms;
            ResModel model;
            ResRM material;
            uint32 deferredID = 0;
            bool initialized = false;

            uint32 persistentID = 0;
            uint32 prevPersistentID = 0;
            bool justRebuilt = false;
        };
        
        const Map<uint64, Entry>& GetEntries() const { return entries; }

    private:
        
        Map<uint64, Entry> entries;
        Map<uint32, ResShader> deferredShaders;
        
        Entry& GetEntry(const MeshInstance& InInstance);
        static bool UpdatePersistence(Entry& InEntry, const MeshInstance& InInstance);
    };
}
    
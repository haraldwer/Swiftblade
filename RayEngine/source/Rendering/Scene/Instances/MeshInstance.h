#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"

struct MeshInstance
{
    ResModel model;
    ResRM material;
    Mat4F transform;
    uint64 hash;
    float extent = 1.0f;

    inline static uint32 PERSISTENT_ID = 0;
    static uint32 GenPersistentID()
    {
        PERSISTENT_ID = Utility::Math::Max(static_cast<uint32>(1), PERSISTENT_ID + 1);
        return PERSISTENT_ID;
    }

    static uint64 GenHash(ResModel& InModel, ResRM& InMat)
    {
        union Union
        {
            struct Res
            {
                uint32 materialHash;
                uint32 modelHash;
            } resource;
            uint64 key = 0;
        } hash;
            
        if (const auto mat = InMat.Get())
            hash.resource.materialHash = mat->Hash();
        if (const auto m = InModel.Get())
            hash.resource.modelHash = m->Hash();
        return hash.key;
    }
};

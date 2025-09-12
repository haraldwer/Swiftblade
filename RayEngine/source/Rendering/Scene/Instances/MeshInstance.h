#pragma once
#include "Rendering/Resources/Material.h"
#include "Rendering/Resources/Model.h"

namespace Rendering
{
    enum class MeshMask : uint8
    {
        NONE = 0,
        DEFAULT = 1 << 0,
        SHADOWS = 1 << 1,
        LUMIN = 1 << 2,
        ALL = 255,
    };

    struct MeshInstance
    {
        ResModel model;
        ResRM material;
        Mat4F transform;
        uint64 hash;
        uint8 mask = static_cast<uint8>(MeshMask::ALL);
        float extent = 1.0f;

        inline static uint32 PERSISTENT_ID = 0;
        static uint32 GenPersistentID()
        {
            PERSISTENT_ID = Utility::Math::Max(static_cast<uint32>(1), PERSISTENT_ID + 1);
            return PERSISTENT_ID;
        }

        static uint64 GenHash(const ResModel& InModel, const ResRM& InMat)
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
}

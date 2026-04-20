#pragma once

#include "CubeVolumeCoord.h"

namespace ECS
{
    struct CubeVolumeData
    {
        Map<VolumeCoordKey, VolumeDataType> data; // Each coordinate has a block type
        void Serialize(SerializeObj& InOutObj) const;
        bool Deserialize(const DeserializeObj& InObj);
    };
}


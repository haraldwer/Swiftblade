#pragma once
#include "Math/Coord.h"

namespace ECS
{
    struct CubeVolume;
    
    typedef uint32 VolumeCoordKey;
    typedef int8 VolumeCoordValue;
    typedef uint8 VolumeDataType;
    typedef Utility::Coord<VolumeCoordKey, VolumeCoordValue> VolumeCoord;
}

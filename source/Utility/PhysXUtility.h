#pragma once

#define PX_PUBLIC_RELEASE 1
#define PX_PHYSX_STATIC_LIB
#define PX_DEBUG 1
#define PX_NVTX 0
#define PX_CHECKED 1
#define PX_SUPPORT_PVD 1
#define PX_SUPPORT_OMNI_PVD 1

#include "physx/PxPhysicsAPI.h"

namespace Utility
{
    namespace PhysX
    {
        inline physx::PxMat44T<float> ConvertMat(Mat4F InMat)
        {
            return physx::PxMat44T<float>({InMat.data});
        }
    }
}

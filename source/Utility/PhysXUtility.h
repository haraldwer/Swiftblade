#pragma once

#define PX_PUBLIC_RELEASE 1
#define PX_PHYSX_STATIC_LIB
#define PX_NVTX 0
#define PX_SUPPORT_PVD 1
#define PX_SUPPORT_OMNI_PVD 1

#include "physx/PxPhysicsAPI.h"

namespace Utility
{
    namespace PhysX
    {
        inline physx::PxVec3 ConvertVec(const Vec3F& InVec)
        {
            return { InVec.x, InVec.y, InVec.z };
        }

        inline physx::PxQuat ConvertQuat(const QuatF& InQuat)
        {
            return { InQuat.x, InQuat.y, InQuat.z, InQuat.w };
        }

        inline Vec3F ConvertVec(const physx::PxVec3& InVec)
        {
            return { InVec.x, InVec.y, InVec.z };
        }

        inline QuatF ConvertQuat(const physx::PxQuat& InQuat)
        {
            return { InQuat.x, InQuat.y, InQuat.z, InQuat.w };
        }
    }
}

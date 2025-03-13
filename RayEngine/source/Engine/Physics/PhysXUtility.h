#pragma once

#include "foundation/PxVec3.h"
#include "foundation/PxQuat.h"

namespace Utility
{
    namespace PhysX
    {
        inline physx::PxVec3T<float> ConvertVec(const Vec3F& InVec)
        {
            return { InVec.x, InVec.y, InVec.z };
        }

        inline physx::PxQuatT<float> ConvertQuat(const QuatF& InQuat)
        {
            return { InQuat.x, InQuat.y, InQuat.z, InQuat.w };
        }

        inline Vec3F ConvertVec(const physx::PxVec3T<float>& InVec)
        {
            return Vec3F(InVec.x, InVec.y, InVec.z);
        }

        inline QuatF ConvertQuat(const physx::PxQuatT<float>& InQuat)
        {
            return QuatF(InQuat.w, InQuat.x, InQuat.y, InQuat.z);
        }
    }
}

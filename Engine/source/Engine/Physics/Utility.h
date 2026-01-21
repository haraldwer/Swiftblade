#pragma once

#include "reactphysics3d/mathematics/Transform.h"

namespace Physics
{
    inline reactphysics3d::Vector3 GetVec(const Vec3F& InVec)
    {
        return { InVec.x, InVec.y, InVec.z };
    }

    inline Vec3F GetVec(const reactphysics3d::Vector3& InVec)
    {
        return { InVec.x, InVec.y, InVec.z };
    }

    inline reactphysics3d::Quaternion GetQuat(const QuatF& InVec)
    {
        return { InVec.x, InVec.y, InVec.z, InVec.w };
    }

    inline QuatF GetQuat(const reactphysics3d::Quaternion& InVec)
    {
        return { InVec.w, InVec.x, InVec.y, InVec.z };
    }
    
    inline reactphysics3d::Transform GetTrans(const Mat4F& InTransform)
    {
        return { GetVec(InTransform.GetPosition()), GetQuat(InTransform.GetRotation()) };
    }
    
    inline Mat4F GetTrans(const reactphysics3d::Transform& InTransform)
    {
        return { GetVec(InTransform.getPosition()), GetQuat(InTransform.getOrientation()), Vec3F::One() };
    }
}

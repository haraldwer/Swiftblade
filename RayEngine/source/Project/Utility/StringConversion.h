#pragma once
#include "Math/AngleConversion.h"

namespace Utility
{
    template <class T>
    String ToString(const Math::Vector3<T>& InVec)
    {
        return "x:" + std::to_string(InVec.x) + " y:" + std::to_string(InVec.y) + " z:" + std::to_string(InVec.z); 
    }

    template <class T>
    String ToString(const Math::Quaternion<T>& InQuat)
    {
        Math::Vector3<T> vec = InQuat.Euler() * Math::RadiansToDegrees(1.0f); 
        return "x:" + std::to_string(vec.x) + " y:" + std::to_string(vec.y) + " z:" + std::to_string(vec.z); 
    }
}

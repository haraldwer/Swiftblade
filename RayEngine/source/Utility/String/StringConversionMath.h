#pragma once

#include "StringConversion.h"
#include "Math/Matrix/Matrix.hpp"
#include "Math/Quaternion.hpp"
#include "Math/Vector/Vector.hpp"

namespace Utility
{
    template <class T>
    String ToStr(const Math::Quaternion<T>& InQuat)
    {
        Math::Vector3<T> vec = InQuat.Euler() * Math::RadiansToDegrees(1.0f); 
        return "x:" + ToStr(vec.x) + " y:" + ToStr(vec.y) + " z:" + ToStr(vec.z);
    }

    template <class T, int A>
    String ToStr(const Math::Vector4<T, A>& InValue)
    {
        return ToStr(InValue.x) + " " + ToStr(InValue.y) + " " + ToStr(InValue.z) + " " + ToStr(InValue.w);
    }

    template <class T, int A>
    String ToStr(const Math::Vector3<T, A>& InValue)
    {
        return ToStr(InValue.x) + " " + ToStr(InValue.y) + " " + ToStr(InValue.z);
    }
        
    template <class T, int A>
    String ToStr(const Math::Vector2<T, A>& InValue)
    {
        return ToStr(InValue.x) + " " + ToStr(InValue.y);
    }

    template <class T>
    String ToStr(const Math::Matrix3x3<T>& InValue)
    {
        return ToStr(InValue.right) + " " + ToStr(InValue.up) + " " + ToStr(InValue.forward);
    }
        
    template <class T>
    String ToStr(const Math::Matrix4x4<T>& InValue)
    {
        Math::Vector3<T> p = InValue.GetPosition();
        Math::Vector3<T> s = InValue.GetScale();
        Math::Vector3<T> r = InValue.GetEuler();
        return "\nP: " + ToStr(p) + "\nS: " + ToStr(s) + "\nR: " + ToStr(r);
    }
}

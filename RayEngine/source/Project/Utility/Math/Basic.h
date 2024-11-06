#pragma once

namespace Utility
{
    namespace Math
    {
        template<class T>
        T Min(const T InFirst, const T InSecond)
        {
            return InFirst < InSecond ? InFirst : InSecond;
        }

        template<class T>
        T Max(const T InFirst, const T InSecond)
        {
            return InFirst > InSecond ? InFirst : InSecond;
        }

        template<class T>
        T Clamp(const T InValue, const T InMin, const T InMax)
        {
            if (InMin < InMax)
                return Min(Max(InValue, InMin), InMax);
            return Min(Max(InValue, InMax), InMin);
        }

        template<class T>
        T Saturate(const T InValue)
        {
            return Min(Max(InValue, static_cast<T>(0.0)), static_cast<T>(1.0));
        }


    }
}

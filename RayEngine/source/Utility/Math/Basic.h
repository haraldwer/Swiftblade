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

        template <class T>
        T DegreesToRadians(T InDeg)
        {
            return InDeg * (static_cast<T>(PI) / static_cast<T>(180));
        }

        template <class T>
        T RadiansToDegrees(T InRad)
        {
            return InRad * (static_cast<T>(180) / static_cast<T>(PI));
        }

        template <class T>
        T NextPow2(T InValue)
        {
            return pow(static_cast<T>(2), ceil(log(InValue)/log(static_cast<T>(2))));
        }
        
        inline uint32 NextPow2(uint32 InValue)
        {
            InValue--;
            InValue |= InValue >> 1;
            InValue |= InValue >> 2;
            InValue |= InValue >> 4;
            InValue |= InValue >> 8;
            InValue |= InValue >> 16;
            InValue++;
            return InValue;
        }
    }
}

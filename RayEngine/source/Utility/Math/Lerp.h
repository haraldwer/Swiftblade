#pragma once

#include "Basic.h"

namespace Utility
{
    namespace Math
    {
        template<class T, class F>
        T Lerp(const T& InA, const T& InB, const F InT)
        {
            return InA + (InB - InA) * Saturate(InT);
        }

        template<class T, class F>
        T LerpDelta(const T& InA, const T& InB, const F InSpeed, const F InDelta)
        {
            F lerp = 1 - pow(1 - InSpeed * 3.0f, InDelta * 300);
            return Lerp(InA, InB, lerp);
        }

        template<class T>
        T SmoothStep(T InT)
        {
            return InT * InT * InT * (InT * (6.0f * InT - 15.0f) + 10.0f);
        }
        
        template<class T, class F>
        T SmoothLerp(const T& InA, const T& InB, const F InT)
        {
            return Lerp(InA, InB, SmoothStep(InT));
        }
    }
}

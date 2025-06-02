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
    }
}

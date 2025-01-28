#pragma once

#include "Basic.h"

namespace Utility
{
    namespace Math
    {
        template<class T, class F>
        T Lerp(const T& a, const T& b, const F t)
        {
            return a + (b - a) * Saturate(t);
        }
    }
}

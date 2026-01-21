#pragma once

#include <cassert>
#include <cmath>

namespace Utility
{
    namespace Math
    {
        template<typename DataType>
        DataType SquareRoot(DataType InInputValue)
        {
            assert(false && "DataType has no supported square root.");
        }

        template<>
        inline float SquareRoot<float>(const float InInputValue)
        {
            return sqrtf(InInputValue);
        }

        template<>
        inline double SquareRoot<double>(const double InInputValue)
        {
            return sqrt(InInputValue);
        }

        template<>
        inline int SquareRoot<int>(const int InInputValue)
        {
            if (InInputValue == 0 || InInputValue == 1)
                return InInputValue;

            int start = 1;
            int end = InInputValue;
            int result = 0;

            while (start <= end)
            {
                const int middle = (start + end) / 2;
                if (middle * middle == InInputValue)
                    return middle;

                if (middle * middle < InInputValue)
                {
                    start = middle + 1;
                    result = middle;
                }
                else 
                {
                    end = middle - 1;
                }
            }

            return result;
        }

    }
}

#pragma once

#include <cassert>
#include <cmath>

namespace Utility
{
    namespace Math
    {
        template<typename DataType>
        DataType SquareRoot(DataType input_value)
        {
            assert(false && "DataType has no supported square root.");
        }

        template<>
        inline float SquareRoot<float>(float input_value)
        {
            return sqrtf(input_value);
        }

        template<>
        inline double SquareRoot<double>(double input_value)
        {
            return sqrt(input_value);
        }

        template<>
        inline int SquareRoot<int>(int input_value)
        {
            if (input_value == 0 || input_value == 1)
                return input_value;

            int start = 1;
            int end = input_value;
            int result = 0;

            while (start <= end)
            {
                const int middle = (start + end) / 2;
                if (middle * middle == input_value)
                    return middle;

                if (middle * middle < input_value)
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

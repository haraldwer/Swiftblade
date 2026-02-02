#pragma once
#include <chrono>

namespace Utility
{
    typedef std::chrono::high_resolution_clock::time_point Timepoint;
    
    inline Timepoint Now()
    {
        return std::chrono::high_resolution_clock::now();
    }
}

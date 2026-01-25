#pragma once
#include <chrono>

namespace Utility
{
    typedef std::chrono::steady_clock::time_point Timepoint;
    
    inline Timepoint Now()
    {
        return std::chrono::steady_clock::now();
    }
}

#pragma once
#include <chrono>

namespace Utility
{
    typedef std::chrono::steady_clock::time_point Timepoint; 
    
    class Timer
    {
    public:
        Timer();
        double Ellapsed() const;
        
    private:
        Timepoint Start;
    };
}

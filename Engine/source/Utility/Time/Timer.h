#pragma once

#include "Timepoint.h"

namespace Utility
{
    class Timer
    {
    public:
        Timer();
        double Ellapsed() const;
        
    private:
        Timepoint start;
    };
}

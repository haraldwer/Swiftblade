#pragma once
#include <chrono>

namespace Utility
{
#ifdef _MSC_VER
    typedef std::chrono::steady_clock::time_point Timepoint;
#elifdef __EMSCRIPTEN__ 
    typedef std::chrono::steady_clock::time_point Timepoint;
#else
    typedef std::chrono::system_clock::time_point Timepoint;
#endif
}

#include "Timer.h"

Utility::Timer::Timer()
{
    Start = std::chrono::high_resolution_clock::now();
}

double Utility::Timer::Ellapsed() const
{
    const auto current = std::chrono::high_resolution_clock::now();
    return static_cast<double>((current - Start).count()) / 1000000000.0; // Convert to sec
}

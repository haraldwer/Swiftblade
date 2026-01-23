#include "Timer.h"

Utility::Timer::Timer()
{
    start = std::chrono::steady_clock::now();
}

double Utility::Timer::Ellapsed() const
{
    const auto current = std::chrono::steady_clock::now();
    return static_cast<double>((current - start).count()); // Convert to sec
}

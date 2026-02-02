#include "Timer.h"

Utility::Timer::Timer()
{
    start = Now();
}

double Utility::Timer::Ellapsed() const
{
    const auto current = Now();
    std::chrono::nanoseconds duration = current - start;
    return static_cast<double>(duration.count()) / 1e9;
}

#pragma once

namespace Utility
{
    inline String ToStr(const std::time_t& InTimePoint, const String& InFormat = "%Y-%m-%d %H:%M:%S")
    {
        std::stringstream ss;
        ss << std::put_time(std::localtime(&InTimePoint), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline String ToStr(const std::chrono::system_clock::time_point& InTimePoint, const String& InFormat = "%Y-%m-%d %H:%M:%S")
    {
        return ToStr(std::chrono::system_clock::to_time_t(InTimePoint), InFormat);
    }

    inline String ToStr(const Timepoint& InTimePoint, const String& InFormat = "%Y-%m-%d %H:%M:%S")
    {
        auto duration = std::chrono::duration_cast<std::chrono::system_clock::duration>(InTimePoint - std::chrono::steady_clock::now());
        auto t = std::chrono::system_clock::to_time_t(std::chrono::system_clock::now() + duration);
        return ToStr(t, InFormat);
    }
    
    template <class Rep, class Period>
    String ToStr(const std::chrono::duration<Rep, Period>& InDuration)
    {
        auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(InDuration).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(InDuration).count() % 1000;

        std::stringstream ss;
        ss << '('
           << std::setfill('0')
           << std::setw(2) << total_seconds / 60 << ':'      // Minutes
           << std::setw(2) << total_seconds % 60 << '.'      // Seconds
           << std::setw(3) << milliseconds                    // Milliseconds
           << ')';
        return ss.str();
    }
}
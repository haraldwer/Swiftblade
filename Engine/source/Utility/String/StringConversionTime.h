#pragma once

namespace Utility
{
    inline String ToStr(const std::time_t& InTimePoint, const String& InFormat = "%Y-%m-%d %H:%M:%S")
    {
        std::stringstream ss;
        ss << std::put_time(std::localtime(&InTimePoint), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    inline String ToStr(const Timepoint& InTimePoint, const String& InFormat = "%Y-%m-%d %H:%M:%S")
    {
        auto sys_tp = std::chrono::system_clock::now() +
            (InTimePoint - std::chrono::high_resolution_clock::now());
        auto sys_tp_cast =
            std::chrono::time_point_cast<std::chrono::system_clock::duration>(sys_tp);
        return ToStr(std::chrono::system_clock::to_time_t(sys_tp_cast), InFormat);
    }
    
    template <class Rep, class Period>
    String ToStr(const std::chrono::duration<Rep, Period>& InDuration)
    {
        auto total_seconds = std::chrono::duration_cast<std::chrono::seconds>(InDuration).count();
        auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(InDuration).count() % 1000;

        std::stringstream ss;
        ss << '('
           << std::setfill('0')
           << std::setw(2) << total_seconds / 60 << ':'
           << std::setw(2) << total_seconds % 60 << '.'
           << std::setw(3) << milliseconds
           << ')';
        return ss.str();
    }
}
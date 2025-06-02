#include "Utility.h"

#include "crossguid/guid.hpp"
#include "raylib.h"

void Utility::ExternalLog(const String& InStr)
{
    static String last;
    static uint32 lastC;
    static Utility::Timer timer;
    if (InStr == last)
    {
        lastC++;
        return;
    }

    if (lastC > 1)
    {
        const String msg = last + " (" + std::to_string(lastC) + ")";
        lastC = 0;
        last = String();
        ExternalLog(msg);
    }

    const double time = timer.Ellapsed();
    const uint32 minutes = static_cast<uint32>(time / 60);
    const uint32 seconds = static_cast<uint32>(time) % 60;
    const uint32 milliseconds = static_cast<uint32>(time * 1000) % 1000;
    const String t = std::to_string(minutes) + ":" + std::to_string(seconds) + ":" + std::to_string(milliseconds) + " | ";
    TraceLog(LOG_WARNING, (t + InStr).c_str());
    last = InStr;
    lastC = 0;
}

String Utility::DeviceGUID()
{
    return xg::Guid().str(); 
}

#include "Utility.h"

#include "crossguid/guid.hpp"
#include "raylib.h"

void Utility::ExternalLog(const String& InStr)
{
    TraceLog(LOG_WARNING, InStr.c_str());
}

String Utility::DeviceGUID()
{
    return xg::Guid().str(); 
}

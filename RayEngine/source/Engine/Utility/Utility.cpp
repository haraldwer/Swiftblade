#include "Utility.h"
#include "raylib.h"

void Utility::ExternalLog(const String& InStr)
{
    TraceLog(LOG_INFO, InStr.c_str());
}

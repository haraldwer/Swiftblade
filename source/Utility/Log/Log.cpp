#include "Log.h"

// TODO: Support va args in utility macro? 
void Log(const char* InMsg)
{
    TraceLog(LOG_INFO, InMsg);
}

#include "Logging.h"

#include "raylib.h"

#ifdef __EMSCRIPTEN__
#include "emscripten/console.h"
#endif

static Vector<std::function<void(const String&)>> logCallbacks;

void Log(const String& InMessage)
{
    static String last;
    static uint32 lastC;
    if (InMessage == last)
    {
        lastC++;
        return;
    }

    if (lastC > 1)
    {
        const String msg = last + " (" + std::to_string(lastC) + ")";
        lastC = 0;
        last = String();
        Log(msg);
    }

    char timeStr[64] = {};
    time_t now = time(NULL);
    tm *tm_info = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
    const String t = std::format("[{}] ", timeStr);
    const String m = t + InMessage + "\n"; 
    
#ifdef __EMSCRIPTEN__
    emscripten_console_log(m.c_str());
#else
    printf(m.c_str());
#endif
    
    for (auto& callback : logCallbacks)
        callback(m);
    
    last = InMessage;
    lastC = 0;
}

void CustomLog(int msgType, const char *text, va_list args)
{
    char buffer[1024];
    vsnprintf(buffer, sizeof(buffer), text, args); 
    
    String type = "LOG";
    switch (msgType)
    {
        case LOG_INFO: type = "INFO"; break;
        case LOG_ERROR: type = "ERROR"; break;
        case LOG_WARNING: type = "WARN"; break;
        case LOG_DEBUG: type = "DEBUG"; break;
        default: break;
    }
    Log(type + " | " + String(buffer));
}

void CustomLog(int msgType, const char *text, ...)
{
    va_list args;
    va_start(args, text);
    CustomLog(msgType, text, args);
    va_end(args);
}

void Utility::InitLogging()
{
    SetTraceLogCallback(CustomLog);
    logCallbacks.clear();
}

void Utility::ExternalLog(const String &InFile, const String &InFunc, const int InLine, const String &InText)
{
    auto filenameEnd = InFile.find_last_of(".");
    auto filenameStart = InFile.find_last_of("/") + 1;
    String filename = InFile.substr(filenameStart, filenameEnd - filenameStart); 
    Log(filename + "::" + InFunc + "::" + ToStr(InLine) + " | " + InText);
}

void Utility::AddLogCallback(const std::function<void(const String &)> &InCallback)
{
    logCallbacks.push_back(InCallback);
}

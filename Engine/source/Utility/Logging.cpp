#include "Logging.h"

#include <iostream>

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
    time_t now = time(nullptr);
    tm *tm_info = localtime(&now);
    strftime(timeStr, sizeof(timeStr), "%H:%M:%S", tm_info);
    const String t = std::format("[{}]", timeStr);
    const String m = t + InMessage; 
    
#ifdef __EMSCRIPTEN__
    emscripten_console_log(m.c_str());
#else
    std::cout << m << std::endl; // Flush
#endif
    
    for (auto& callback : logCallbacks)
        callback(m);
    
    last = InMessage;
    lastC = 0;
}

void Utility::ExternalLog(const String& InCategory, const String& InFile, const String& InFunc, const int InLine, const String& InText)
{
    auto filenameEnd = InFile.find_last_of('.');
    auto filenameStart = Math::Min(InFile.find_last_of('/'), InFile.find_last_of('\\')) + 1;
    String filename = InFile.substr(filenameStart, filenameEnd - filenameStart); 
    Log("[" + ToUpper(InCategory) + "] " + filename + "::" + InFunc + "::" + ToStr(InLine) + " | " + InText);
}

void Utility::AddLogCallback(const std::function<void(const String &)> &InCallback)
{
    logCallbacks.push_back(InCallback);
}

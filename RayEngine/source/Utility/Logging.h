#pragma once
#include <functional>

namespace Utility
{
    void InitLogging();
    void ExternalLog(const String& InFile, const String& InFunc, int InLine, const String& InText);
    void AddLogCallback(const std::function<void(const String&)>& InCallback);
}
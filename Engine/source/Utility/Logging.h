#pragma once

namespace Utility
{
    void ExternalLog(const String& InCategory, const String& InFile, const String& InFunc, int InLine, const String& InText);
    void AddLogCallback(const std::function<void(const String&)>& InCallback);
}
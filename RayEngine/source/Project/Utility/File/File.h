#pragma once

#include "Utility/Time/Timepoint.h"

namespace Utility
{
    void SetWorkingDir();
    String ReadFile(const String& InPath);
    bool WriteFile(const String& InPath, const String& InContent);
    bool FileExists(const String& InPath);
    Timepoint GetFileWriteTime(const String& InPath);
}

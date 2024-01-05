#pragma once

#include "Utility/Time/Timepoint.h"

namespace Utility
{
    void SetWorkingDir();
    String ReadFile(const String& InPath);
    bool WriteFile(const String& InPath, const String& InContent);
    Timepoint GetFileWriteTime(const String& InPath);
}

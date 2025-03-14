#pragma once

namespace Utility
{
    // TODO: Use raylib functions instead??

    void SetWorkingDir();
    String ReadFile(const String& InPath);
    bool WriteFile(const String& InPath, const String& InContent);
    bool FileExists(const String& InPath);
    Timepoint GetFileWriteTime(const String& InPath);
}

#pragma once

namespace Utility
{
    void SetWorkingDir();
    String ReadFile(const String& InPath);
    Vector<String> ListFiles(const String& InPath);
    bool WriteFile(const String& InPath, const String& InContent);
    bool FileExists(const String& InPath);
    void CreateDir(const String& InPath);
    bool DeleteFile(const String& InPath);
    Timepoint GetFileWriteTime(const String& InPath);
    String GetCachePath(const String& InPath, String InExt);

}

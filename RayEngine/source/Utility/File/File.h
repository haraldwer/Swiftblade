#pragma once

namespace Utility
{
    namespace File
    {
        // File info
        bool Exists(const String& InPath);
        Vector<String> List(const String& InPath);
        Timepoint GetWriteTime(const String& InPath);
        uint32 GetSize(const String& InPath);

        // File manipulation
        void CreateDir(const String& InPath);
        String Read(const String& InPath);
        bool Write(const String& InPath, const String& InContent);
        bool Delete(const String& InPath);
        bool Copy(const String& InFrom, const String& InTo, bool InRename = false);

        // Paths
        String GetCachePath(const String& InPath, String InExt);
        String Relative(const String& InPath);
        String Name(const String& InPath);
        void SetWorkingDir();
    }
}

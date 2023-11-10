#pragma once

#include <fstream>
#include <chrono>
#include <filesystem>

#include "Time/Timepoint.h"

namespace Utility
{
    inline String ReadFile(const String& InPath)
    {
        constexpr std::size_t read_size = 4096;
        auto stream = std::ifstream(("../content/" + InPath).data());
        stream.exceptions(std::ios_base::badbit);

        if (!stream) {
            return String();
        }
    
        auto out = std::string();
        auto buf = std::string(read_size, '\0');
        while (stream.read(buf.data(), read_size)) {
            out.append(buf, 0, stream.gcount());
        }
        out.append(buf, 0, stream.gcount());
        return out;
    }

    inline bool WriteFile(const String& InPath, const String& InContent)
    {
        std::ofstream out("../content/" + InPath);
        if (!out.is_open())
            return false;
        out.clear();
        out << InContent;
        out.close();
        return true;
    }

    inline Timepoint GetFileWriteTime(const String& InPath)
    {
        if (InPath.empty())
            return Timepoint();
        const String path = "../content/" + InPath;
        if (!std::filesystem::exists("../content/" + InPath))
            return Timepoint(); 
        const std::filesystem::file_time_type fileTime = std::filesystem::last_write_time(path);
        return Timepoint(fileTime.time_since_epoch());
    }
}
